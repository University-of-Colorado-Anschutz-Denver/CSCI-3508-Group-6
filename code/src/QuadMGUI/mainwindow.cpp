
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "exiftool.h"
#include "file.h"
#include "settings.h"


#include <QDir>
#include <QFileInfo>
#include <QFileIconProvider>
#include <QListWidgetItem>
#include <QPushButton>
#include <QDebug>
#include <QMovie>
#include <QJsonObject>
#include <QJsonDocument>
#include <QByteArray>
#include <QFileInfoList>
#include <QComboBox>
#include <QStorageInfo>

#include "metadataeditor.h"
#include <QSettings>


void MainWindow::loadDefaultDrive()
{
    QSettings settings("QuadM", "QUADM");

    QString defaultDrive =settings.value("defaultDrive", QDir::rootPath()).toString();

    int index = ui->driveComboBox->findData(defaultDrive);

    if (index == -1){
        index = 0;}

    if (index >= 0)
    {ui->driveComboBox->setCurrentIndex(index);}
}

void MainWindow::getDrives() {


    ui->driveComboBox->clear();
    for (const QStorageInfo &drive : QStorageInfo::mountedVolumes()) {
        QString driveInfo;
        if (drive.isReady()) {
            if (drive.displayName().isEmpty() || drive.displayName() == "Local Disk" || drive.displayName() == drive.rootPath()) {
                driveInfo = QString("%1").arg(drive.rootPath());
            }
            else {
                driveInfo = QString("%1 (%2)").arg(drive.displayName()).arg(drive.rootPath());
            }
            ui->driveComboBox->addItem(driveInfo, drive.rootPath());
        }
    }
}

void MainWindow::loadRootFolders()
{
    currentPath.clear();
    pathHistory.clear();
    ui->rootListWidget->clear();

    QString path = ui->driveComboBox->currentData().toString(); //drive path

    // Use your home directory as the test root.
    QDir dir(path); // picks up drive path
    QFileIconProvider iconProvider;

    // List folders inside the home directory.
    QFileInfoList folders = dir.entryInfoList(
        QDir::Dirs | QDir::NoDotAndDotDot,
        QDir::Name
    );

    for (const QFileInfo& folder : folders) {
        auto* item = new QListWidgetItem(iconProvider.icon(folder), folder.fileName());

        // Store the actual directory path.
        item->setData(Qt::UserRole, folder.absoluteFilePath());

        ui->rootListWidget->addItem(item);
    }
}


void MainWindow::loadDirectory(const QString& path)
{
    QDir dir(path);

    if (!dir.exists()) {
        qDebug() << "Directory does not exist:" << path;
        return;
    }

    QString absolutePath = path;

    // Save the current directory for Back navigation.
    if (!currentPath.isEmpty() &&QDir::cleanPath(currentPath) != QDir::cleanPath(absolutePath)) {
        pathHistory.append(currentPath);
    }

    currentPath = absolutePath;

    ui->contentsListWidget->clear();
    ui->currentPathLabel->setText(currentPath);

    ui->contentsListWidget->show();
    ui->currentPathLabel->show();
    ui->backButton->setVisible(!pathHistory.isEmpty());
    ui->filePathSearchBar->show();
    ui->tagFilterBar->show();

    QFileIconProvider iconProvider;

    // Show folders first, then supported media files.
    QFileInfoList entries = dir.entryInfoList(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot, QDir::DirsFirst | QDir::Name);

    for (const QFileInfo& entry : entries) {

        // Always allow folders so users can navigate through them.
        // Only display files accepted by FileManager.
        if (entry.isFile() && !FileManager::isSupportedFile(entry.absoluteFilePath())) {
            continue;
        }

        auto* item = new QListWidgetItem(iconProvider.icon(entry), entry.fileName());

        // Keep the full path attached to the list item.
        item->setData(Qt::UserRole, entry.absoluteFilePath());

        ui->contentsListWidget->addItem(item);
    }

    // Metadata (including tags) is scanned lazily in ensureDirectoryMetadataScanned(), only once
    // the user actually types a filter, so simply navigating folders never touches ExifTool.
    directoryMetadata.clear();
    directoryMetadataScanned = false;
    applyTagFilter(ui->tagFilterBar->text());

    qDebug() << "Loaded directory:" << currentPath
        << "| Displayed items:" << ui->contentsListWidget->count();
}

void MainWindow::ensureDirectoryMetadataScanned()
{
    if (directoryMetadataScanned)
        return;
    directoryMetadataScanned = true;

    QString error;
    service.scanFolderMetadata(currentPath, false, directoryMetadata, error);
}

void MainWindow::applyTagFilter(const QString& query)
{
    const QString wanted = query.trimmed();
    if (!wanted.isEmpty())
        ensureDirectoryMetadataScanned();

    for (int i = 0; i < ui->contentsListWidget->count(); ++i) {
        QListWidgetItem* item = ui->contentsListWidget->item(i);
        const QString itemPath = item->data(Qt::UserRole).toString();

        if (wanted.isEmpty() || QFileInfo(itemPath).isDir()) {
            item->setHidden(false);
            continue;
        }

        bool matches = false;
        const QMap<QString, QString>& fields = directoryMetadata.value(itemPath);
        for (auto it = fields.constBegin(); it != fields.constEnd(); ++it) {
            // Match the field's name (e.g. "FileName") or its value (tags live here too, as
            // XMP:Subject/ID3:QuadM_Tags).
            if (it.key().section(':', -1).contains(wanted, Qt::CaseInsensitive)
                || it.value().contains(wanted, Qt::CaseInsensitive)) {
                matches = true;
                break;
            }
        }
        item->setHidden(!matches);
    }
}

QString MainWindow::formatMetadataKey(const QString& key)
{
    QString result;

    for (int i = 0; i < key.length(); ++i) {
        QChar current = key[i];
        if (i > 0 && current.isUpper() && key[i - 1].isLower()) {
            result += ' ';
        }
        result += current;
    }

    return result;
}

void MainWindow::metadataPopulate(const QString& filePath)
{
    QFileInfo fileInfo(filePath);

    // Stop the OIIA placeholder animation
    QMovie* catMovie = ui->metadataPlaceholderLabel->movie();

    if (catMovie) {
        catMovie->stop();
        ui->metadataPlaceholderLabel->setMovie(nullptr);
    }

    if (!fileInfo.exists() || !fileInfo.isFile()) {
        ui->metadataPlaceholderLabel->setText("File not found.");
        return;
    }

    if (!FileManager::isSupportedFile(filePath)) {
        ui->metadataPlaceholderLabel->setText(
            "This file format is not supported."
        );
        return;
    }

    QString html;

    // Basic file information
    html += "<h3>File Information</h3>";

    html += "<b>Name:</b> " + fileInfo.fileName().toHtmlEscaped() + "<br>";
    html += "<b>Type:</b> " + fileInfo.suffix().toUpper() + "<br>";
    html += "<b>Size:</b> " + QString::number(fileInfo.size() / 1024.0, 'f', 2) + " KB<br>";

    html += "<b>Modified:</b> " + fileInfo.lastModified().toString("yyyy-MM-dd hh:mm:ss") + "<br>";

    html += "<hr>";

    // File path
    html += "<h3>File Path</h3>";
    html += fileInfo.absoluteFilePath().toHtmlEscaped();

    html += "<hr>";

    // Extract media metadata
    QJsonObject metadata;
    QString error;

    bool success = ExifTool::readMetadata(
        fileInfo.absoluteFilePath(), metadata, error
    );

    QMap<QString, QMap<QString, QString>> groupedMetadata; //map to hold the categories and their fields

    html += "<h3>Media Metadata</h3>";

    if (success && !metadata.isEmpty()) {

        for (auto it = metadata.constBegin();it != metadata.constEnd(); ++it) {
            QStringList parts = it.key().split(':'); // split category
            if (parts.size() > 1) {
                groupedMetadata[parts[0]][parts[1]] = it.value().toVariant().toString().toHtmlEscaped(); // add the key value pair with category
                //qDebug() << groupedMetadata[parts[0]]<< " "<<groupedMetadata[parts[0]][parts[1]];
            }
            else {
                QString key = it.key().toHtmlEscaped();
                QString value = it.value().toVariant().toString().toHtmlEscaped();
                html += "<b>" + key + ":</b> " + value + "<br>";
            }
        }

        for (auto ctg = groupedMetadata.constBegin(); ctg != groupedMetadata.constEnd(); ++ctg) {
            html += "<h3>" + formatMetadataKey(ctg.key()).toHtmlEscaped() + "</h3>";

            for (auto p0 = ctg.value().constBegin(); p0 != ctg.value().constEnd(); ++p0) {
                html += "<b>" + formatMetadataKey(p0.key()).toHtmlEscaped() + ":</b> " + p0.value().toHtmlEscaped() + "<br>";
            }
        }

    }
    else if (success) {
        html += "No metadata found.";
    }
    else {
        html += "Could not extract metadata.<br>";
        html += error.toHtmlEscaped();

        qDebug() << "ExifTool metadata error:" << error;
    }

    ui->metadataPlaceholderLabel->setText(html);
    ui->metadataPlaceholderLabel->setTextFormat(Qt::RichText);
    ui->metadataPlaceholderLabel->setWordWrap(true);
    ui->metadataPlaceholderLabel->setTextInteractionFlags(
        Qt::TextSelectableByMouse
    );

    qDebug() << "Metadata displayed for:" << filePath;
}


void MainWindow::openMetadataEditor(const QString& filePath)
{
    if (!metadataEditor) {
        metadataEditor = new MetadataEditor(filePath, this);

        connect(metadataEditor, &MetadataEditor::metadataSaved, this, [this](const QString& savedPath) {
            qDebug() << "Metadata saved:" << savedPath;
            metadataPopulate(savedPath);
            loadDirectory(currentPath);
            });
    }
    else {
        metadataEditor->setPath(filePath);
    }

    metadataEditor->show();
    metadataEditor->raise();
    metadataEditor->activateWindow();
}


void MainWindow::handleOpenWindow() {

    Settings *subWindow = new Settings(); 

    connect(subWindow, &Settings::settingsSave, this, [this]() {
        loadDefaultDrive();
        loadRootFolders();
    });
    
    subWindow->setAttribute(Qt::WA_DeleteOnClose); 
    subWindow->show();
}

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    filterDebounceTimer.setSingleShot(true);
    filterDebounceTimer.setInterval(250);
    connect(&filterDebounceTimer, &QTimer::timeout, this, [this]() {
        applyTagFilter(ui->tagFilterBar->text());
        });

    ui->contentLayout->setStretch(0, 25);
    ui->contentLayout->setStretch(1, 30);
    ui->contentLayout->setStretch(2, 45);

    ui->metadataScrollArea->setWidgetResizable(true);

    ui->metadataFrame->setSizePolicy(
        QSizePolicy::Expanding,
        QSizePolicy::Expanding
    );

    ui->metadataScrollArea->setSizePolicy(
        QSizePolicy::Expanding,
        QSizePolicy::Expanding
    );

    ui->metadataPanelLayout->setStretch(1, 1);

    // back button debug statements
    // qDebug() << ui->backButton;
    // qDebug() << ui->backButton->isEnabled();
    // qDebug() << ui->backButton->isVisible();
    // qDebug() << ui->backButton->geometry();

    // Scroll logic
    QLabel* metadataLabel = ui->metadataPlaceholderLabel;
    QWidget* metadataParent = metadataLabel->parentWidget();

    // QScrollArea *metadataScrollArea = new QScrollArea(metadataParent);
    // metadataScrollArea->setWidgetResizable(true);
    // metadataScrollArea->setFrameShape(QFrame::NoFrame);
    // metadataScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    // metadataScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    // if (metadataParent->layout()) {
    //     metadataParent->layout()->replaceWidget(
    //         metadataLabel,
    //         metadataScrollArea
    //     );
    // } else {
    //     metadataScrollArea->setGeometry(metadataLabel->geometry());
    // }

    // metadataScrollArea->setWidget(metadataLabel);

    // metadataLabel->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    // metadataLabel->setWordWrap(true);
    // //________________________________________________________________________________

    QMovie* catMovie = new QMovie(":/icons/oiia_cat.gif", QByteArray(), this);

    ui->metadataPlaceholderLabel->setMovie(catMovie);
    catMovie->start();

    getDrives();
    loadDefaultDrive();
    // Populate the Root panel and load the initial directory.
    loadRootFolders();

    connect(ui->driveComboBox, &QComboBox::currentIndexChanged, this, [this]() {
        loadRootFolders();
        });

    QMenu* fileMenu = menuBar()->addMenu("Settings");
    QAction* openAct = fileMenu->addAction("&Open Window");

    // 2. Connect the action's triggered signal to your slot
    connect(openAct, &QAction::triggered, this, &MainWindow::handleOpenWindow);
    //loadDirectory(QDir::homePath());

    ui->contentsListWidget->hide();
    ui->currentPathLabel->hide();
    ui->backButton->hide();
   // ui->filePathSearchBar->hide();
    ui->tagFilterBar->hide();

    connect(ui->tagFilterBar, &QLineEdit::textChanged, this, [this]() {
        // Wait for a pause in typing before (possibly) scanning metadata and filtering,
        // so holding down a key doesn't trigger it on every single character.
        filterDebounceTimer.start();
        });

    // Clicking a root folder loads its contents.
    connect(ui->rootListWidget, &QListWidget::itemClicked, this, [this](QListWidgetItem* item) {

        //reset the path history and current path when a root folder is clicked to avoid confusion in back navigation
        pathHistory.clear();
        currentPath.clear();


        loadDirectory(item->data(Qt::UserRole).toString());
        });

    // Single-click a supported file to display its metadata.
    connect(ui->contentsListWidget, &QListWidget::itemClicked, this, [this](QListWidgetItem* item) {
        QString path = item->data(Qt::UserRole).toString();
        QFileInfo info(path);

        if (info.isFile() && FileManager::isSupportedFile(path)) {
            filePath = info.absoluteFilePath();

            qDebug() << "File path:" << filePath;

            // Read metadata when the user selects a supported file.
            metadataPopulate(filePath);
        }
        });

    connect(ui->filePathSearchBar, &QLineEdit::returnPressed, this, [this]() {
        QString searchPath = ui->filePathSearchBar->text().trimmed();
        if (searchPath.isEmpty()) {
            return;
        }

        QUrl url(searchPath);

        if (url.isLocalFile()) {
            searchPath = url.toLocalFile();
        }
        QFileInfo info(searchPath);

        if (!info.exists()) {
            qDebug() << "Path does not exist:" << searchPath;
            return;
        }

        if (info.isDir()) {
            loadDirectory(info.absoluteFilePath());

        }
        else if (info.isFile() && FileManager::isSupportedFile(searchPath)) {

            filePath = info.absoluteFilePath();

            // Load the parent directory.
            loadDirectory(info.absolutePath());
            ui->contentsListWidget->clear();

            QFileIconProvider iconProvider;

            auto* item = new QListWidgetItem(iconProvider.icon(QFileInfo(filePath)), QFileInfo(filePath).fileName());

            item->setData(Qt::UserRole, filePath);


            ui->contentsListWidget->addItem(item);
            ui->contentsListWidget->setCurrentItem(item);

            // Populate metadata.
            metadataPopulate(filePath);
        }
        else {
            qDebug() << "Unsupported file type or invalid path:" << searchPath;
        }
        });

    ui->contentsListWidget->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(ui->contentsListWidget, &QListWidget::customContextMenuRequested, this, [this](const QPoint& pos) {

        QListWidgetItem* item = ui->contentsListWidget->itemAt(pos);

        if (!item)
            return;

        QMenu menu(this);
        QAction* copyPathAction = menu.addAction("Copy File Path");

        QAction* selectedAction = menu.exec(ui->contentsListWidget->mapToGlobal(pos));

        if (selectedAction == copyPathAction) {
            QString filePath = item->data(Qt::UserRole).toString();
            QApplication::clipboard()->setText(filePath);
        }
        });

    // Folder logic. Double click to display its content
    connect(ui->contentsListWidget, &QListWidget::itemDoubleClicked, this, [this](QListWidgetItem* item) {
        QString path = item->data(Qt::UserRole).toString();
        QFileInfo info(path);

        if (info.isDir()) {
            loadDirectory(path);
        }
        });

    connect(ui->expandMetadataButton, &QPushButton::clicked, this, [this]() {
        if (!filePath.isEmpty()) {
            openMetadataEditor(filePath);
        }
        });

    //Navigate backward
    connect(ui->backButton, &QPushButton::clicked, this, [this]() {

        if (pathHistory.isEmpty())
            return;
    
        QString previousPath = pathHistory.takeLast();
    
        // Prevent loadDirectory() from adding the current path again.
        currentPath.clear();
    
        loadDirectory(previousPath);
    
    });

    // connect(ui->backButton, &QPushButton::clicked, this, []() {
    //     qDebug() << "click";
    // });
}


MainWindow::~MainWindow()
{
    delete ui;
}
