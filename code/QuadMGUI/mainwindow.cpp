#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMovie> // placeholder gif include


#include <QFile>
#include <QTextStream>
#include <QListWidgetItem>
#include <QStyle>
#include <QDebug>
#include <QPushButton>



void MainWindow::loadData() {
    // clear entries to ensure that there is no previous enties stored

    Entries.clear();

    // open file as read only 

    QFile file(":/icons/testdata.txt"); // have the file hardcoded as I don't expect it to change


    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Failed to open file!";
        return;
    }
    // read file data
    QTextStream data(&file);

    // add stuff to the Entry vector

    while (!data.atEnd()) {
        //_______________________NEEDS REFACTORING IF THE .txt FORMAT CHANGES_________________
        QStringList parts = data.readLine().split('|');
        if (parts.size() != 3)
            continue;

        Entry entry;
        entry.directory = (parts[0] == "DIR");
        entry.root = parts[1];
        entry.name = parts[2];
        Entries.append(entry);
        //_________________________________________________________________________________
    }

    file.close();

    // for each entry populate the files/ folders on the top level

    ui->rootListWidget->clear();

    for (int i = 0; i < Entries.size(); i++){
        Entry entry = Entries[i];
        if (entry.directory && entry.root == "/") {
            auto *item = new QListWidgetItem(style()->standardIcon(QStyle::SP_DirIcon), entry.name);
        
            item->setData(Qt::UserRole, "/" + entry.name);
            ui->rootListWidget->addItem(item);
        }

    }
    currentPath.clear();
    pathHistory.clear();
    loadDirectory("/");

}

void MainWindow::loadDirectory(const QString &path) {

    currentPath = path;

    ui->contentsListWidget->clear();
    ui->currentPathLabel->setText(path);

    for (int i = 0; i < Entries.size(); i++){
        const Entry &entry = Entries[i];

        if (entry.root != path)
            continue;

        auto *item = new QListWidgetItem(style()->standardIcon(entry.directory? QStyle::SP_DirIcon: QStyle::SP_FileIcon),entry.name);

        QString fullPath = (path == "/")
            ? "/" + entry.name
            : path + "/" + entry.name;

        item->setData(Qt::UserRole, fullPath);
        ui->contentsListWidget->addItem(item);
    }

    qDebug() << "path:" << path<< "| Entries:" << ui->contentsListWidget->count();


}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QMovie *catMovie = new QMovie(":/icons/oiia_cat.gif", QByteArray(), this);

    ui->metadataPlaceholderLabel->setMovie(catMovie);
    catMovie->start();

    loadData();

    // populate contents after loading the data
    connect(ui->rootListWidget, &QListWidget::itemClicked, this, [this](QListWidgetItem *item) {
        loadDirectory(item->data(Qt::UserRole).toString());
    });

    // double-click a folder to navigate into it.
    connect(ui->contentsListWidget, &QListWidget::itemDoubleClicked, this, [this](QListWidgetItem *item) {
        QString name = item->text();

        for (int i = 0; i < Entries.size(); i++) {
            const Entry &entry = Entries[i];

            if (entry.root == currentPath && entry.name == name && entry.directory) {
                pathHistory.append(currentPath);
                loadDirectory(item->data(Qt::UserRole).toString());
                return;
            }
        }

        qDebug() << "Selected dummy file:" << name;
    });

    // Navigate backward.
    connect(ui->backButton, &QPushButton::clicked, this, [this]() {
        if (!pathHistory.isEmpty())
            loadDirectory(pathHistory.takeLast());
    });

    
}

MainWindow::~MainWindow()
{
    delete ui;
}

