#include "metadataeditor.h"
#include "ui_metadataeditor.h"
#include "exiftool.h"

#include "api/metadataservice.h"
#include "backend/metadata.h"
#include "backend/metadatafilter.h" 

#include <QFileInfo>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QMessageBox>
#include <QLayout>

#include <QJsonObject>
#include <QJsonValue>
#include <QSet>



// NEW: Run ExifTool and parse its XML tag database
#include <QProcess>
#include <QXmlStreamReader>
#include <QStandardPaths>
#include <QSet>
#include <QRegularExpression>

MetadataEditor::MetadataEditor(const QString &filePath, QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MetadataEditor)
{
    ui->setupUi(this);
    setWindowTitle("QuadM - Metadata Editor");
    resize(800, 600);

    connect(ui->addMetadataButton, &QPushButton::clicked, this, &MetadataEditor::addMetadata);
    connect(ui->saveMetadataButton, &QPushButton::clicked, this, &MetadataEditor::saveMetadata);
    connect(ui->cancelButton, &QPushButton::clicked, this, &QWidget::close);

    setPath(filePath);
}

MetadataEditor::~MetadataEditor()
{
    delete ui;
}

void MetadataEditor::setPath(const QString &filePath)
{
    path = filePath;
    populateData();
}


// NEW: Store metadata tag names and their writability
struct ExifTagInfo {
    bool writable;
};

// NEW: Retrieve ExifTool's complete metadata tag database
static QMap<QString, ExifTagInfo> getExifToolTags()
{
    QMap<QString, ExifTagInfo> tags;

    QString executable = QStandardPaths::findExecutable("exiftool");

    if (executable.isEmpty())
        return tags;

    QProcess process;
    process.start(executable, {"-listx"});

    if (!process.waitForFinished(30000) || process.exitCode() != 0)
        return tags;

    QXmlStreamReader xml(process.readAllStandardOutput());

    QString currentTable;

    while (!xml.atEnd()) {
        xml.readNext();

        if (xml.isStartElement()) {

            // NEW: Identify the current ExifTool table
            if (xml.name() == "table") {
                currentTable = xml.attributes().value("name").toString();
            }

            // NEW: Read each tag and its writable attribute
            else if (xml.name() == "tag") {
                QString name = xml.attributes().value("name").toString();

                QString key = currentTable + ":" + name;

                bool writable =
                    xml.attributes().value("writable") == "true";

                if (!name.isEmpty() &&
                    !currentTable.isEmpty() &&
                    !isIgnoredTag(key)) {

                    tags.insert(key, {writable});
                }
            }
        }

        // NEW: Reset table when its XML element closes
        else if (xml.isEndElement() && xml.name() == "table") {
            currentTable.clear();
        }
    }

    return tags;
}


void MetadataEditor::populateData()
{
    //QJsonObject metadata;
    QString error;
    QFileInfo fileInfo(path);

    if (!fileInfo.exists() || !fileInfo.isFile()) {
        ui->fileNameLabel->setText("File not found.");
        return;
    }

    ui->fileNameLabel->setText(fileInfo.fileName());
    ui->fileTypeLabel->setText(fileInfo.suffix().toUpper());

    // Clear existing widgets
    while (QLayoutItem *item = ui->metadataLayout->takeAt(0)) {
        if (QWidget *widget = item->widget())
            widget->deleteLater();
        delete item;
    }

    metadataFields.clear();
    metadataRows.clear();
    originalMetadata.clear();
    deletedMetadata.clear();

    if (!service.readMetadata(path, metadata, error)) {
        ui->metadataLayout->addWidget(new QLabel(error));
        return;
    }

    // TEMPORARY DEBUG
    qDebug() << "Fields returned by service:" << metadata.fields().size();

    for (auto it = metadata.fields().constBegin();
        it != metadata.fields().constEnd(); ++it) {
        qDebug() << it.key() << "=" << it.value();
    }

    // Store original metadata, including empty and ignored fields
    for (auto it = metadata.fields().constBegin();
         it != metadata.fields().constEnd(); ++it) {
        originalMetadata[it.key()] = it.value();
    }

    // Populate metadata fields
    QSet<QString> categories;

    for (auto it = originalMetadata.constBegin();
         it != originalMetadata.constEnd(); ++it) {

        QString category = it.key().contains(':')
            ? it.key().section(':', 0, 0)
            : "Other";

        if (!categories.contains(category)) {
            auto *categoryLabel = new QLabel(category);
            categoryLabel->setStyleSheet(
                "font-size: 16px; font-weight: bold;"
            );

            ui->metadataLayout->addWidget(categoryLabel);
            categories.insert(category);
        }

        // Display every field, including ignored and empty fields
        addMetadataField(it.key(), it.value());
    }

    ui->metadataLayout->addStretch();
}



void MetadataEditor::addMetadataField(const QString &key, const QString &value)
{
    QWidget *row = new QWidget(ui->metadataScrollContents);
    QHBoxLayout *rowLayout = new QHBoxLayout(row);

    QLabel *propertyLabel = new QLabel(key.section(':', -1), row);
    QLineEdit *valueEdit = new QLineEdit(value, row);
    QPushButton *deleteButton = new QPushButton("Delete", row);
    
    const bool readOnly = !MetadataService::isEditable(key) || isReadOnlyTag(key);

    if (readOnly) {
        valueEdit->setReadOnly(true);

        valueEdit->setStyleSheet("QLineEdit {"
            "background-color: #e0e0e0;"
            "color: #777777;"
            "border: 1px solid #c0c0c0;""}"
        );

        deleteButton->setEnabled(false);
        deleteButton->setToolTip("This metadata field is read-only.");
}

    valueEdit->setMinimumWidth(300);

    rowLayout->addWidget(propertyLabel);
    rowLayout->addWidget(valueEdit);
    rowLayout->addWidget(deleteButton);

    metadataFields[key] = valueEdit;
    metadataRows[key] = row;

    ui->metadataLayout->insertWidget(ui->metadataLayout->count() - 1, row);

    // delte only if not read-only
    if (!readOnly) {
        connect(deleteButton, &QPushButton::clicked, this, [this, key]() {
            deleteMetadataField(key);
        });
    }
}

void MetadataEditor::addMetadata()
{
    bool ok;
    QString key = QInputDialog::getText(this, "Add Metadata", "Tag name (e.g. XMP:Description):", QLineEdit::Normal, "", &ok).trimmed();

    if (!ok || key.isEmpty())
        return;

    if (metadataFields.contains(key)) {
        QMessageBox::warning(this, "Duplicate Metadata", "This metadata tag already exists.");
        return;
    }

    if (key.startsWith('-') || key.contains('=') || key.contains('\n') || key.contains('\r')) {
        QMessageBox::warning(this, "Invalid Tag", "Please enter a valid metadata tag name.");
        return;
    }

    QString value = QInputDialog::getText(this, "Add Metadata", "Value:", QLineEdit::Normal, "", &ok);

    if (!ok)
        return;

    deletedMetadata.remove(key);
    addMetadataField(key, value);
}

void MetadataEditor::deleteMetadataField(const QString &key)
{
    QWidget *row = metadataRows.value(key, nullptr);

    if (!row)
        return;

    if (originalMetadata.contains(key))
        deletedMetadata.insert(key);

    ui->metadataLayout->removeWidget(row);
    metadataFields.remove(key);
    metadataRows.remove(key);

    row->deleteLater();
}

void MetadataEditor::saveMetadata()
{
   //QMap<QString, QString> changedTags;

    for (auto it = metadataFields.constBegin(); it != metadataFields.constEnd(); ++it) {
        metadata.set(it.key(), it.value()->text());
    }

    if (!metadata.hasChanges() && deletedMetadata.isEmpty()) {
        QMessageBox::information(this, "Metadata Editor", "No changes to save.");
        return;
    }

    if (!deletedMetadata.isEmpty()) {
        QMessageBox::warning(this, "Deletion Not Supported", "Deleted fields have been removed from the editor, but your current ExifTool writeTags() function does not support deleting tags from the file.");
    }

    if (!metadata.hasChanges()) {
        QMessageBox::information(this, "Metadata Editor", "There are no additions or edits to save.");
        return;
    }
    QString newName = ui->fileNameLabel->text().trimmed();

    if (newName.isEmpty() || newName != QFileInfo(path).fileName()) {
        QFileInfo info(path);
        QString newPath = info.dir().filePath(newName);

        if (newName.isEmpty() || QFileInfo::exists(newPath)) {
            QMessageBox::warning(this, "Rename Error",
                                "Invalid filename or a file with that name already exists.");
            return;
        }

        if (!QFile::rename(path, newPath)) {
            QMessageBox::warning(this, "Rename Error",
                                "Could not rename the file.");
            return;
        }

        path = newPath;
    }

    QString error;

    if (!service.writeMetadata(path, metadata, error)) {
        QMessageBox::critical(this, "Save Error", error);
        return;
    }

    QMessageBox::information(this, "Metadata Editor", "Metadata changes saved successfully.");
    populateData();
    emit metadataSaved(path);
}