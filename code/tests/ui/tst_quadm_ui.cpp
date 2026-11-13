// UI tests for QuadM. They create the real windows and drive them like a user would
// (typing, clicking, double-clicking), then check what is on screen.
// Build and run: see tests/README.md.
#include "QuadMGUI/mainwindow.h"
#include "QuadMGUI/metadataeditor.h"
#include "QuadMGUI/settings.h"
#include "exiftool.h"

#include <QApplication>
#include <QComboBox>
#include <QDir>
#include <QFile>
#include <QImage>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMenuBar>
#include <QMessageBox>
#include <QPushButton>
#include <QSettings>
#include <QSignalSpy>
#include <QTemporaryDir>
#include <QTimer>
#include <QtTest>

namespace {

// Closes the next QMessageBox the app opens (they block until answered) and remembers its text.
class DialogCatcher : public QObject
{
public:
    DialogCatcher()
    {
        connect(&timer, &QTimer::timeout, this, [this]() {
            if (auto *box = qobject_cast<QMessageBox *>(QApplication::activeModalWidget())) {
                text = box->text();
                seen = true;
                box->close();
                timer.stop();
            }
        });
        timer.start(30);
    }
    QTimer timer;
    QString text;
    bool seen = false;
};

QString clean(const QString &path) { return QDir::cleanPath(QDir::fromNativeSeparators(path)); }

QListWidgetItem *itemByText(QListWidget *list, const QString &text)
{
    for (int i = 0; i < list->count(); ++i) {
        if (list->item(i)->text() == text)
            return list->item(i);
    }
    return nullptr;
}

QStringList itemTexts(QListWidget *list, bool visibleOnly = false)
{
    QStringList texts;
    for (int i = 0; i < list->count(); ++i) {
        if (!visibleOnly || !list->item(i)->isHidden())
            texts << list->item(i)->text();
    }
    return texts;
}

QPoint centerOf(QListWidget *list, QListWidgetItem *item)
{
    list->scrollToItem(item);
    return list->visualItemRect(item).center();
}

void clickItem(QListWidget *list, QListWidgetItem *item)
{
    QTest::mouseClick(list->viewport(), Qt::LeftButton, Qt::NoModifier, centerOf(list, item));
}

void doubleClickItem(QListWidget *list, QListWidgetItem *item)
{
    // Qt's synthetic double click needs a preceding normal click on the same item
    clickItem(list, item);
    QTest::mouseDClick(list->viewport(), Qt::LeftButton, Qt::NoModifier, centerOf(list, item));
}

bool exifToolAvailable() { return !ExifTool::executablePath().isEmpty(); }

} // namespace

// ---------------------------------------------------------------------------------------------
// Main window: browsing folders, the file list, metadata panel, filter and settings menu
// ---------------------------------------------------------------------------------------------
class MainWindowTests : public QObject
{
    Q_OBJECT

private:
    QTemporaryDir data;  // folder full of test files
    QString dataPath;
    MainWindow *window = nullptr;

    QLineEdit *pathBar() { return window->findChild<QLineEdit *>("filePathSearchBar"); }
    QLineEdit *tagBar() { return window->findChild<QLineEdit *>("tagFilterBar"); }
    QListWidget *contents() { return window->findChild<QListWidget *>("contentsListWidget"); }
    QListWidget *roots() { return window->findChild<QListWidget *>("rootListWidget"); }
    QLabel *pathLabel() { return window->findChild<QLabel *>("currentPathLabel"); }
    QLabel *metadataLabel() { return window->findChild<QLabel *>("metadataPlaceholderLabel"); }
    QPushButton *backButton() { return window->findChild<QPushButton *>("backButton"); }
    QPushButton *expandButton() { return window->findChild<QPushButton *>("expandMetadataButton"); }

    // Types a path into the address bar and presses Enter.
    void goTo(const QString &path)
    {
        pathBar()->setText(path);
        QTest::keyClick(pathBar(), Qt::Key_Return);
    }

    void makeImage(const QString &path)
    {
        QImage image(8, 8, QImage::Format_RGB32);
        image.fill(Qt::red);
        QVERIFY(image.save(path));
    }

private slots:
    void initTestCase()
    {
        QVERIFY(data.isValid());
        dataPath = data.path();
        QDir dir(dataPath);
        QVERIFY(dir.mkdir("subfolder"));
        makeImage(dir.filePath("photo_alpha.png"));
        makeImage(dir.filePath("photo_beta.png"));
        makeImage(dir.filePath("subfolder/inner.png"));

        QFile notes(dir.filePath("notes.txt"));  // not media, must not be listed
        QVERIFY(notes.open(QIODevice::WriteOnly));
        notes.write("hello");
    }

    void init()
    {
        window = new MainWindow;
        window->show();
        QVERIFY(QTest::qWaitForWindowExposed(window));
    }

    void cleanup()
    {
        delete window;
        window = nullptr;
    }

    // UI-01
    void startup_showsDrivesAndPlaceholder()
    {
        auto *drives = window->findChild<QComboBox *>("driveComboBox");
        QVERIFY(drives->count() > 0);
        QVERIFY(!contents()->isVisible());           // no folder opened yet
        QVERIFY(!backButton()->isVisible());
        QVERIFY(metadataLabel()->movie() != nullptr);  // animated placeholder
        QVERIFY(pathBar()->isVisible());
    }

    // UI-02
    void addressBar_folderPath_listsFolder()
    {
        goTo(dataPath);
        QVERIFY(contents()->isVisible());
        QCOMPARE(clean(pathLabel()->text()), clean(dataPath));
    }

    // UI-03
    void fileList_foldersFirst_unsupportedFilesHidden()
    {
        goTo(dataPath);
        const QStringList shown = itemTexts(contents());
        QCOMPARE(shown, (QStringList{"subfolder", "photo_alpha.png", "photo_beta.png"}));
        QVERIFY(!shown.contains("notes.txt"));
    }

    // UI-04
    void doubleClickFolder_opensIt_backReturns()
    {
        goTo(dataPath);
        QVERIFY(!backButton()->isVisible());

        doubleClickItem(contents(), itemByText(contents(), "subfolder"));
        QCOMPARE(clean(pathLabel()->text()), clean(dataPath + "/subfolder"));
        QCOMPARE(itemTexts(contents()), QStringList{"inner.png"});
        QVERIFY(backButton()->isVisible());

        QTest::mouseClick(backButton(), Qt::LeftButton);
        QCOMPARE(clean(pathLabel()->text()), clean(dataPath));
        QVERIFY(!backButton()->isVisible());
    }

    // UI-05
    void clickFile_showsMetadataPanel()
    {
        goTo(dataPath);
        clickItem(contents(), itemByText(contents(), "photo_alpha.png"));

        const QString text = metadataLabel()->text();
        QVERIFY(text.contains("File Information"));
        QVERIFY(text.contains("photo_alpha.png"));
        QVERIFY(text.contains("PNG"));
        QVERIFY(metadataLabel()->movie() == nullptr);  // placeholder animation stopped
    }

    // UI-06
    void clickFile_withExifTool_showsMediaMetadata()
    {
        if (!exifToolAvailable())
            QSKIP("ExifTool is not next to the test executable");
        goTo(dataPath);
        clickItem(contents(), itemByText(contents(), "photo_alpha.png"));
        const QString text = metadataLabel()->text();
        QVERIFY(text.contains("Media Metadata"));
        QVERIFY(text.contains("Image Width"));
    }

    // UI-07
    void addressBar_invalidPath_changesNothing()
    {
        goTo(dataPath);
        goTo(dataPath + "/does-not-exist");
        QCOMPARE(clean(pathLabel()->text()), clean(dataPath));
        QCOMPARE(itemTexts(contents()).size(), 3);
    }

    // UI-08
    void addressBar_unsupportedFile_changesNothing()
    {
        goTo(dataPath);
        goTo(dataPath + "/notes.txt");
        QCOMPARE(clean(pathLabel()->text()), clean(dataPath));
        QCOMPARE(itemTexts(contents()).size(), 3);
    }

    // UI-09
    void addressBar_filePath_showsOnlyThatFileAndItsMetadata()
    {
        goTo(dataPath + "/photo_beta.png");
        QCOMPARE(itemTexts(contents()), QStringList{"photo_beta.png"});
        QVERIFY(metadataLabel()->text().contains("photo_beta.png"));
    }

    // UI-10
    void addressBar_emptyText_changesNothing()
    {
        goTo(dataPath);
        goTo("   ");
        QCOMPARE(clean(pathLabel()->text()), clean(dataPath));
    }

    // UI-11
    void tagFilter_hidesNonMatchingFiles_keepsFolders_andClears()
    {
        if (!exifToolAvailable())
            QSKIP("ExifTool is not next to the test executable");
        goTo(dataPath);

        QTest::keyClicks(tagBar(), "alpha");
        QTRY_COMPARE_WITH_TIMEOUT(itemTexts(contents(), true),
                                  (QStringList{"subfolder", "photo_alpha.png"}), 15000);

        tagBar()->clear();
        QTRY_COMPARE_WITH_TIMEOUT(itemTexts(contents(), true).size(), 3, 15000);
    }

    // UI-12
    void tagFilter_noMatch_hidesAllFiles()
    {
        if (!exifToolAvailable())
            QSKIP("ExifTool is not next to the test executable");
        goTo(dataPath);

        QTest::keyClicks(tagBar(), "zzz-no-such-tag");
        QTRY_COMPARE_WITH_TIMEOUT(itemTexts(contents(), true), QStringList{"subfolder"}, 15000);
    }

    // UI-13
    void editButton_withoutSelectedFile_doesNothing()
    {
        QTest::mouseClick(expandButton(), Qt::LeftButton);
        QVERIFY(window->findChildren<MetadataEditor *>().isEmpty());
    }

    // UI-14
    void editButton_withSelectedFile_opensEditor()
    {
        goTo(dataPath);
        clickItem(contents(), itemByText(contents(), "photo_alpha.png"));
        QTest::mouseClick(expandButton(), Qt::LeftButton);

        const auto editors = window->findChildren<MetadataEditor *>();
        QCOMPARE(editors.size(), 1);
        QVERIFY(editors.first()->isVisible());
        QCOMPARE(editors.first()->windowTitle(), QString("QuadM - Metadata Editor"));
        QCOMPARE(editors.first()->findChild<QLabel *>("fileNameLabel")->text(),
                 QString("photo_alpha.png"));
    }

    // UI-15
    void settingsMenu_opensSettingsWindow()
    {
        QAction *open = nullptr;
        for (QAction *menuAction : window->menuBar()->actions()) {
            if (menuAction->text() == "Settings" && menuAction->menu()) {
                for (QAction *a : menuAction->menu()->actions()) {
                    if (a->text().remove('&') == "Open Window")
                        open = a;
                }
            }
        }
        QVERIFY2(open, "Settings > Open Window menu entry not found");
        open->trigger();

        Settings *dialog = nullptr;
        for (QWidget *w : QApplication::topLevelWidgets()) {
            if (auto *s = qobject_cast<Settings *>(w))
                dialog = s;
        }
        QVERIFY(dialog);
        QVERIFY(dialog->isVisible());
        dialog->close();
    }

    // UI-16
    void rootFolderList_clickLoadsFolder()
    {
        if (roots()->count() == 0)
            QSKIP("The selected drive has no folders");
        QListWidgetItem *first = roots()->item(0);
        const QString path = first->data(Qt::UserRole).toString();
        QTest::mouseClick(roots()->viewport(), Qt::LeftButton, Qt::NoModifier,
                          centerOf(roots(), first));
        QCOMPARE(clean(pathLabel()->text()), clean(path));
        QVERIFY(contents()->isVisible());
    }
};

// ---------------------------------------------------------------------------------------------
// Metadata editor window
// ---------------------------------------------------------------------------------------------
class MetadataEditorTests : public QObject
{
    Q_OBJECT

private:
    QTemporaryDir data;
    QString imagePath;

    QString newImage(const QString &name)
    {
        const QString path = data.filePath(name);
        QImage image(8, 8, QImage::Format_RGB32);
        image.fill(Qt::blue);
        image.save(path);
        return path;
    }

    static QLineEdit *fieldWithText(QWidget *editor, const QString &text)
    {
        for (QLineEdit *edit : editor->findChildren<QLineEdit *>()) {
            if (edit->text() == text)
                return edit;
        }
        return nullptr;
    }

private slots:
    void initTestCase() { QVERIFY(data.isValid()); }

    void init() { imagePath = newImage("editor_test.png"); }

    // UI-20
    void opens_showsFileNameAndType()
    {
        MetadataEditor editor(imagePath);
        editor.show();
        QVERIFY(QTest::qWaitForWindowExposed(&editor));
        QCOMPARE(editor.findChild<QLabel *>("fileNameLabel")->text(), QString("editor_test.png"));
        QCOMPARE(editor.findChild<QLabel *>("fileTypeLabel")->text(), QString("PNG"));
        QCOMPARE(editor.windowTitle(), QString("QuadM - Metadata Editor"));
    }

    // UI-21
    void missingFile_showsNotFound()
    {
        MetadataEditor editor(data.filePath("missing.png"));
        QCOMPARE(editor.findChild<QLabel *>("fileNameLabel")->text(), QString("File not found."));
    }

    // UI-22
    void cancelButton_closesWindow()
    {
        MetadataEditor editor(imagePath);
        editor.show();
        QVERIFY(QTest::qWaitForWindowExposed(&editor));
        QTest::mouseClick(editor.findChild<QPushButton *>("cancelButton"), Qt::LeftButton);
        QVERIFY(!editor.isVisible());
    }

    // UI-23
    void fields_listed_withEditableFileName()
    {
        if (!exifToolAvailable())
            QSKIP("ExifTool is not next to the test executable");
        MetadataEditor editor(imagePath);
        QLineEdit *name = fieldWithText(&editor, "editor_test.png");
        QVERIFY2(name, "File name field not shown");
        QVERIFY(!name->isReadOnly());
        QVERIFY(editor.findChildren<QLineEdit *>().size() > 1);
    }

    // UI-24
    void readOnlyFields_haveDisabledDeleteButton()
    {
        if (!exifToolAvailable())
            QSKIP("ExifTool is not next to the test executable");
        MetadataEditor editor(imagePath);
        bool foundReadOnly = false;
        for (QLineEdit *edit : editor.findChildren<QLineEdit *>()) {
            if (!edit->isReadOnly())
                continue;
            foundReadOnly = true;
            auto *delButton = edit->parentWidget()->findChild<QPushButton *>();
            QVERIFY(delButton);
            QVERIFY(!delButton->isEnabled());
        }
        QVERIFY2(foundReadOnly, "Expected at least one read-only field (e.g. image size)");
    }

    // UI-25
    void deleteButton_removesRowFromEditor()
    {
        if (!exifToolAvailable())
            QSKIP("ExifTool is not next to the test executable");
        MetadataEditor editor(imagePath);
        editor.show();
        QVERIFY(QTest::qWaitForWindowExposed(&editor));

        QPushButton *target = nullptr;
        for (QPushButton *b : editor.findChildren<QPushButton *>()) {
            if (b->text() == "Delete" && b->isEnabled())
                target = b;
        }
        if (!target)
            QSKIP("No deletable field in this file");
        QWidget *row = target->parentWidget();
        const int before = editor.findChildren<QLineEdit *>().size();
        QTest::mouseClick(target, Qt::LeftButton);
        QTRY_COMPARE(editor.findChildren<QLineEdit *>().size(), before - 1);
        Q_UNUSED(row);
    }

    // UI-26
    void save_withoutChanges_saysNoChanges()
    {
        if (!exifToolAvailable())
            QSKIP("ExifTool is not next to the test executable");
        MetadataEditor editor(imagePath);
        editor.show();
        QVERIFY(QTest::qWaitForWindowExposed(&editor));
        QSignalSpy saved(&editor, &MetadataEditor::metadataSaved);

        DialogCatcher dialog;
        QTest::mouseClick(editor.findChild<QPushButton *>("saveMetadataButton"), Qt::LeftButton);
        QVERIFY(dialog.seen);
        QCOMPARE(dialog.text, QString("No changes to save."));
        QCOMPARE(saved.count(), 0);
    }

    // UI-27
    void save_renamedFile_renamesOnDisk()
    {
        if (!exifToolAvailable())
            QSKIP("ExifTool is not next to the test executable");
        MetadataEditor editor(imagePath);
        editor.show();
        QVERIFY(QTest::qWaitForWindowExposed(&editor));
        QSignalSpy saved(&editor, &MetadataEditor::metadataSaved);

        QLineEdit *name = fieldWithText(&editor, "editor_test.png");
        QVERIFY(name);
        name->setText("renamed.png");

        DialogCatcher dialog;
        QTest::mouseClick(editor.findChild<QPushButton *>("saveMetadataButton"), Qt::LeftButton);
        QVERIFY(dialog.seen);
        QVERIFY(dialog.text.contains("saved successfully"));
        QCOMPARE(saved.count(), 1);
        QVERIFY(QFile::exists(data.filePath("renamed.png")));
        QVERIFY(!QFile::exists(imagePath));
    }

    // UI-28
    void save_renameToExistingName_isRejected()
    {
        if (!exifToolAvailable())
            QSKIP("ExifTool is not next to the test executable");
        const QString other = newImage("already_taken.png");
        MetadataEditor editor(imagePath);
        editor.show();
        QVERIFY(QTest::qWaitForWindowExposed(&editor));
        QSignalSpy saved(&editor, &MetadataEditor::metadataSaved);

        QLineEdit *name = fieldWithText(&editor, "editor_test.png");
        QVERIFY(name);
        name->setText("already_taken.png");

        DialogCatcher dialog;
        QTest::mouseClick(editor.findChild<QPushButton *>("saveMetadataButton"), Qt::LeftButton);
        QVERIFY(dialog.seen);
        QVERIFY(dialog.text.contains("already exists"));
        QCOMPARE(saved.count(), 0);
        QVERIFY(QFile::exists(imagePath));
        QVERIFY(QFile::exists(other));
    }
};

// ---------------------------------------------------------------------------------------------
// Settings window
// ---------------------------------------------------------------------------------------------
class SettingsTests : public QObject
{
    Q_OBJECT

private slots:
    // UI-30
    void themeAndDriveChoices_available()
    {
        Settings settings;
        settings.show();
        QVERIFY(QTest::qWaitForWindowExposed(&settings));
        auto *theme = settings.findChild<QComboBox *>("themeComboBox");
        QCOMPARE(theme->count(), 2);
        QVERIFY(theme->findData("Dark") >= 0);
        QVERIFY(theme->findData("Light") >= 0);
        QVERIFY(settings.findChild<QComboBox *>("defaultDriveComboBox")->count() > 0);
    }

    // UI-31
    void save_storesChoices_emitsSignal_andCloses()
    {
        QSettings("QUADM", "QUADM").clear();

        Settings settings;
        settings.show();
        QVERIFY(QTest::qWaitForWindowExposed(&settings));
        QSignalSpy saved(&settings, &Settings::settingsSave);

        auto *theme = settings.findChild<QComboBox *>("themeComboBox");
        theme->setCurrentIndex(theme->findData("Light"));
        QTest::mouseClick(settings.findChild<QPushButton *>("saveButton"), Qt::LeftButton);

        QCOMPARE(saved.count(), 1);
        QVERIFY(!settings.isVisible());
        QCOMPARE(QSettings("QUADM", "QUADM").value("theme").toString(), QString("Light"));
    }

    // UI-32
    void reopen_showsSavedTheme()
    {
        QSettings("QUADM", "QUADM").setValue("theme", "Light");

        Settings settings;
        auto *theme = settings.findChild<QComboBox *>("themeComboBox");
        QCOMPARE(theme->currentData().toString(), QString("Light"));
    }
};

// Runs one test class; its results go to stdout and to <exe folder>/ui-test-results/<name>.txt
template <typename T>
int runClass(const char *name, int argc, char *argv[])
{
    const QString dir = QCoreApplication::applicationDirPath() + "/ui-test-results";
    QDir().mkpath(dir);
    const QByteArray file = (dir + "/" + name + ".txt,txt").toLocal8Bit();

    QList<QByteArray> args;
    for (int i = 0; i < argc; ++i)
        args << argv[i];
    args << "-o" << file << "-o" << "-,txt";
    QList<char *> ptrs;
    for (QByteArray &a : args)
        ptrs << a.data();

    T test;
    return QTest::qExec(&test, ptrs.size(), ptrs.data());
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Keep the tests away from the real user settings (registry / ~/.config)
    QTemporaryDir settingsDir;
    QSettings::setDefaultFormat(QSettings::IniFormat);
    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, settingsDir.path());

    int failures = 0;
    failures += runClass<MainWindowTests>("MainWindowTests", argc, argv);
    failures += runClass<MetadataEditorTests>("MetadataEditorTests", argc, argv);
    failures += runClass<SettingsTests>("SettingsTests", argc, argv);
    return failures;
}

#include "tst_quadm_ui.moc"
