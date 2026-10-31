
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
#include <QDir>
#include <QFileInfoList>
#include <QComboBox>
#include <QStorageInfo>


void MainWindow::getDrives() {

    for (const QStorageInfo &drive : QStorageInfo::mountedVolumes()) {
        if (drive.isReady()) {
            ui->driveComboBox->addItem(drive.displayName(),drive.rootPath());
        }
    }
}

void MainWindow::loadRootFolders()
{
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

    for (const QFileInfo &folder : folders) {
        auto *item = new QListWidgetItem( iconProvider.icon(folder),folder.fileName());

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

    // Stop the OIIA placeholder animation
    QMovie *catMovie = ui->metadataPlaceholderLabel->movie();

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
    html += "<b>Size:</b> " +
            QString::number(fileInfo.size() / 1024.0, 'f', 2) + " KB<br>";

    html += "<b>Modified:</b> " +
            fileInfo.lastModified().toString("yyyy-MM-dd hh:mm:ss") + "<br>";

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
                qDebug() << groupedMetadata[parts[0]]<< " "<<groupedMetadata[parts[0]][parts[1]];
            }
            else {
                QString key = it.key().toHtmlEscaped();
                QString value = it.value().toVariant().toString().toHtmlEscaped();
                html += "<b>" + key + ":</b> " + value + "<br>";
            }
        }

        for (auto ctg = groupedMetadata.constBegin(); ctg != groupedMetadata.constEnd(); ++ctg) {
            html += "<h4>" + ctg.key().toHtmlEscaped() + "</h4>";

            for (auto p0 = ctg.value().constBegin(); p0 != ctg.value().constEnd(); ++p0){
                html += "<b>" + p0.key().toHtmlEscaped() + ":</b> "+ p0.value().toHtmlEscaped() + "<br>";
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


MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

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
    QLabel *metadataLabel = ui->metadataPlaceholderLabel;
    QWidget *metadataParent = metadataLabel->parentWidget();

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

    QMovie *catMovie = new QMovie(":/icons/oiia_cat.gif", QByteArray(), this);

    ui->metadataPlaceholderLabel->setMovie(catMovie);
    catMovie->start();

    getDrives();
    // Populate the Root panel and load the initial directory.
    loadRootFolders();

    connect(ui->driveComboBox, &QComboBox::currentIndexChanged,this, [this]() {
            loadRootFolders();
        });
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
