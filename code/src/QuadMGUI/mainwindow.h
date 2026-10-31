#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QStringList>
#include <QVector>

QT_BEGIN_NAMESPACE





namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private:
    Ui::MainWindow *ui;

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

    void loadRootFolders();
    void loadDirectory(const QString &path);
    void metadataPopulate(const QString &filePath);
    void getDrives();

    
};





#endif // MAINWINDOW_H
