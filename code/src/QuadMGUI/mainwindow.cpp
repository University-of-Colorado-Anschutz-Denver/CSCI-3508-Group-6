
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "exiftool.h"
#include "file.h"

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


void MainWindow::loadRootFolders()
{
    ui->rootListWidget->clear();

    // Use your home directory as the test root.
    QDir home(QDir::homePath());

    QFileIconProvider iconProvider;

    // List folders inside the home directory.
    QFileInfoList folders = home.entryInfoList(
        QDir::Dirs | QDir::NoDotAndDotDot,
        QDir::Name
    );

    for (const QFileInfo &folder : folders) {
        auto *item = new QListWidgetItem(
            iconProvider.icon(folder),
            folder.fileName()
        );

        // Store the actual directory path.
        item->setData(Qt::UserRole, folder.absoluteFilePath());

        ui->rootListWidget->addItem(item);
    }
}


void MainWindow::loadDirectory(const QString &path)
{
    QDir dir(path);

    if (!dir.exists()) {
        qDebug() << "Directory does not exist:" << path;
        return;
    }

    QString absolutePath = dir.absolutePath();

    // Save the current directory for Back navigation.
    if (!currentPath.isEmpty() && currentPath != absolutePath) {
        pathHistory.append(currentPath);
    }

    currentPath = absolutePath;

    const bool atHome =
        QDir::cleanPath(currentPath) ==
        QDir::cleanPath(QDir::homePath());

    ui->contentsListWidget->clear();
    ui->currentPathLabel->setText(currentPath);

    ui->contentsListWidget->show();
    ui->currentPathLabel->show();
    ui->backButton->setVisible(!atHome);

    QFileIconProvider iconProvider;

    // Show folders first, then supported media files.
    QFileInfoList entries = dir.entryInfoList(
        QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot,
        QDir::DirsFirst | QDir::Name
    );

    for (const QFileInfo &entry : entries) {

        // Always allow folders so users can navigate through them.
        // Only display files accepted by FileManager.
        if (entry.isFile() &&
            !FileManager::isSupportedFile(entry.absoluteFilePath())) {
            continue;
        }

        auto *item = new QListWidgetItem(
            iconProvider.icon(entry),
            entry.fileName()
        );

        // Keep the full path attached to the list item.
        item->setData(Qt::UserRole, entry.absoluteFilePath());

        ui->contentsListWidget->addItem(item);
    }

    qDebug() << "Loaded directory:" << currentPath
             << "| Displayed items:" << ui->contentsListWidget->count();
}


void MainWindow::metadataPopulate(const QString &filePath)
{
    QFileInfo fileInfo(filePath);

    // placeholder if nothing is being displayed
    QMovie *catMovie = ui->metadataPlaceholderLabel->movie();

    if (catMovie) {
        catMovie->stop();
        ui->metadataPlaceholderLabel->setMovie(nullptr);
    }

    ui->metadataPlaceholderLabel->setWordWrap(true);
    ui->metadataPlaceholderLabel->setAlignment(
        Qt::AlignLeft | Qt::AlignTop
    );
    ui->metadataPlaceholderLabel->setTextInteractionFlags(
        Qt::TextSelectableByMouse
    );

    if (!fileInfo.exists() || !fileInfo.isFile()) {
        ui->metadataPlaceholderLabel->setText("File not found.");
        return;
    }

    // Wrong file type
    if (!FileManager::isSupportedFile(filePath)) {
        ui->metadataPlaceholderLabel->setText(
            "This file format is not supported."
        );
        return;
    }

    QString metadataText;

    // Display basic file information.
    metadataText += "FILE INFORMATION\n\n";
    metadataText += "Name: " + fileInfo.fileName() + "\n";
    metadataText += "Type: " + fileInfo.suffix().toUpper() + "\n";
    metadataText += "Size: " + QString::number(fileInfo.size() / 1024.0, 'f', 2) +" KB\n";
    metadataText += "Modified: " +fileInfo.lastModified().toString("yyyy-MM-dd hh:mm:ss") +"\n\n";

    metadataText += "FILE PATH\n\n";
    metadataText += fileInfo.absoluteFilePath() + "\n\n";

    // Read the actual media metadata using ExifTool.
    QJsonObject metadata;
    QString error;

    const bool success = ExifTool::readMetadata(fileInfo.absoluteFilePath(),metadata,error);

    metadataText += "MEDIA METADATA\n\n";

    if (success) {

        if (metadata.isEmpty()) {
            metadataText += "No metadata was returned for this file.";
        } 
        else 
        {
            // Format the metadata as readable, indented JSON.
            for (auto it = metadata.constBegin(); it != metadata.constEnd(); ++it) {
                metadataText += it.key() + ": " + it.value().toVariant().toString() + "\n";
            }
        }

    } else {
        metadataText += "Could not extract metadata.\n\n";
        metadataText += error;

        qDebug() << "ExifTool metadata error:" << error;
    }

    ui->metadataPlaceholderLabel->setText(metadataText);

    qDebug() << "Metadata displayed for:"
             << fileInfo.absoluteFilePath();
}


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // back button debug statements
    // qDebug() << ui->backButton;
    // qDebug() << ui->backButton->isEnabled();
    // qDebug() << ui->backButton->isVisible();
    // qDebug() << ui->backButton->geometry();

    // Scroll logic
    QLabel *metadataLabel = ui->metadataPlaceholderLabel;
    QWidget *metadataParent = metadataLabel->parentWidget();

    QScrollArea *metadataScrollArea = new QScrollArea(metadataParent);
    metadataScrollArea->setWidgetResizable(true);
    metadataScrollArea->setFrameShape(QFrame::NoFrame);
    metadataScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    metadataScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    if (metadataParent->layout()) {
        metadataParent->layout()->replaceWidget(
            metadataLabel,
            metadataScrollArea
        );
    } else {
        metadataScrollArea->setGeometry(metadataLabel->geometry());
    }

    metadataScrollArea->setWidget(metadataLabel);

    metadataLabel->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    metadataLabel->setWordWrap(true);
    //________________________________________________________________________________

    QMovie *catMovie = new QMovie(":/icons/oiia_cat.gif", QByteArray(), this);

    ui->metadataPlaceholderLabel->setMovie(catMovie);
    catMovie->start();

    // Remove the Add button; users browse the filesystem directly.

    // Populate the Root panel and load the initial directory.
    loadRootFolders();
    //loadDirectory(QDir::homePath());

    ui->contentsListWidget->hide();
    ui->currentPathLabel->hide();
    ui->backButton->hide();

    // Clicking a root folder loads its contents.
    connect(ui->rootListWidget, &QListWidget::itemClicked, this, [this](QListWidgetItem *item) {
        loadDirectory(item->data(Qt::UserRole).toString());
    });

    // Single-click a supported file to display its metadata.
    connect(ui->contentsListWidget, &QListWidget::itemClicked, this, [this](QListWidgetItem *item) {
        QString path = item->data(Qt::UserRole).toString();
        QFileInfo info(path);

        if (info.isFile() && FileManager::isSupportedFile(path)) {
            filePath = info.absoluteFilePath();

            qDebug() << "File path:" << filePath;

            // Read metadata when the user selects a supported file.
            metadataPopulate(filePath);
        }
    });

    // Folder logic. Double click to display its content
    connect(ui->contentsListWidget, &QListWidget::itemDoubleClicked, this, [this](QListWidgetItem *item) {
        QString path = item->data(Qt::UserRole).toString();
        QFileInfo info(path);

        if (info.isDir()) {
            loadDirectory(path);
        }
    });

    //Navigate backward
    connect(ui->backButton, &QPushButton::clicked, this, [this]() {
        if (pathHistory.isEmpty()) {
            return;
        }

        QString previousPath = pathHistory.takeLast();

        // Avoid recording the current path again.
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
