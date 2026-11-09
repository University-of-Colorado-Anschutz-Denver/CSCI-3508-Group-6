#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QStringList>
#include <QTimer>
#include <QVector>

#include "api/metadataservice.h"

QT_BEGIN_NAMESPACE





namespace Ui {

    class MainWindow;
}
class MetadataEditor;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

private:
    Ui::MainWindow* ui;

    // A simple structure to store a formatted version of the directory path.
    struct Entry
    {
        QString root;
        QString name;
        bool directory; // a simple yes or no
        QString actualPath;
    };

    QVector<Entry> Entries;
    QString currentPath;
    QStringList pathHistory;
    QString filePath;

    MetadataService service;
    // Metadata fields (including tags, as XMP:Subject/ID3:QuadM_Tags) of every file in the
    // currently loaded directory. Scanned lazily, only once the user actually types a filter,
    // since reading every file's metadata is slow and would stutter on every folder navigation.
    QMap<QString, QMap<QString, QString>> directoryMetadata;
    bool directoryMetadataScanned = false;
    QTimer filterDebounceTimer;

    void loadRootFolders();
    void loadDirectory(const QString& path);
    void metadataPopulate(const QString& filePath);
    void getDrives();
    void openMetadataEditor(const QString& filePath);
    void applyTagFilter(const QString& query);
    void ensureDirectoryMetadataScanned();
    void loadDefaultDrive();
    QString formatMetadataKey(const QString& key);

    void handleOpenWindow();

    MetadataEditor* metadataEditor = nullptr;

signals:
    void metadataSaved(const QString& filePath);


};





#endif // MAINWINDOW_H
