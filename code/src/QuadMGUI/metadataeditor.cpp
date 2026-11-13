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
    categoryHeaders.clear();
    originalMetadata.clear();
    deletedMetadata.clear();

    if (!service.readMetadata(path, metadata, error)) {
        ui->metadataLayout->addWidget(new QLabel(error));
        return;
    }

    //TEMPORARY DEBUG
    // qDebug() << "Fields returned by service:" << metadata.fields().size();

    // for (auto it = metadata.fields().constBegin();
    //     it != metadata.fields().constEnd(); ++it) {
    //     qDebug() << it.key() << "=" << it.value();
    // }

    // Store original metadata, including empty and ignored fields
    for (auto it = metadata.fields().constBegin();
         it != metadata.fields().constEnd(); ++it) {
        originalMetadata[it.key()] = it.value();
    }

    // Populate metadata fields (addMetadataField adds the group headings)
    for (auto it = originalMetadata.constBegin();
         it != originalMetadata.constEnd(); ++it) {
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
    
    const bool readOnly =  isReadOnlyTag(key) && key != "File:FileName";



    if (readOnly) {
        valueEdit->setReadOnly(true);

        qDebug() << "Read-only metadata field:" << key;


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

    // Put the row under its group heading (creating the heading if it is new), not at the very bottom
    QVBoxLayout *layout = ui->metadataLayout;
    const QString category = key.contains(':') ? key.section(':', 0, 0) : "Other";
    int index;
    if (QLabel *existingHeader = categoryHeaders.value(category)) {
        index = layout->indexOf(existingHeader) + 1;
        while (index < layout->count() && layout->itemAt(index)->widget()
               && metadataRows.values().contains(layout->itemAt(index)->widget()))
            ++index;
    } else {
        auto *header = new QLabel(category);
        header->setStyleSheet("font-size: 16px; font-weight: bold;");
        categoryHeaders[category] = header;
        index = layout->count();
        if (index > 0 && layout->itemAt(index - 1)->spacerItem()) // stay above the trailing stretch
            --index;
        layout->insertWidget(index++, header);
    }
    layout->insertWidget(index, row);

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

    // MP3/WAV keep every kind of ID3 tag in one place and ExifTool shows it as "ID3", so file
    // "ID3v2:Hello" under the ID3 group right away instead of after the next save
    const QString suffix = QFileInfo(path).suffix().toLower();
    if ((suffix == "mp3" || suffix == "wav") && key.contains(':')
        && key.section(':', 0, 0).startsWith("ID3", Qt::CaseInsensitive))
        key = "ID3:" + key.section(':', 1);

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
        if (it.key() == "File:FileName")
            continue;

        metadata.set(it.key(), it.value()->text());
    }

    QString newName = metadataFields.value("File:FileName")
                          ? metadataFields.value("File:FileName")->text().trimmed()
                          : QFileInfo(path).fileName();

    const bool nameChanged = newName != QFileInfo(path).fileName();

    // Deleting a tag = saving it with an empty value; every handler removes the tag for that
    for (const QString &key : deletedMetadata)
        metadata.set(key, QString());

    if (!metadata.hasChanges() && !nameChanged) {
        QMessageBox::information(this, "Metadata Editor", "No changes to save.");
        return;
    }

    // Rename file if filename changed
    if (nameChanged) {
        QFileInfo info(path);
        QString newPath = info.dir().filePath(newName);

        if (newName.isEmpty() || newName == "." || newName == ".." ||
            QFileInfo(newName).fileName() != newName || QFileInfo::exists(newPath)) {
            QMessageBox::warning(this, "Rename Error",
                                 "Invalid filename or a file with that name already exists.");
            return;
        }

        if (!QFile::rename(path, newPath)) {
            QMessageBox::warning(this, "Rename Error", "Could not rename the file.");
            return;
        }

        path = newPath;
    }

    QString error;

    if (metadata.hasChanges()) {
        if (!service.writeMetadata(path, metadata, error)) {
            QMessageBox::critical(this, "Save Error", error);
            return;
        }
    }

    QMessageBox::information(this, "Metadata Editor", "Metadata changes saved successfully.");

    populateData();
    emit metadataSaved(path);
}