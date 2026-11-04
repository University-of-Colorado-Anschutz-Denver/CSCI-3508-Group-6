#include "metadataeditor.h"
#include "ui_metadataeditor.h"
#include "exiftool.h"

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
    QJsonObject metadata;
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

    if (!ExifTool::readMetadata(path, metadata, error)) {
        ui->metadataLayout->addWidget(new QLabel(error));
        return;
    }

    // Store original metadata
    for (auto it = metadata.constBegin(); it != metadata.constEnd(); ++it)
        originalMetadata[it.key()] = it.value().toVariant().toString();

    // Populate metadata fields
    QSet<QString> categories;

    for (auto it = originalMetadata.constBegin(); it != originalMetadata.constEnd(); ++it) {
        QString category = it.key().contains(':') ? it.key().section(':', 0, 0) : "Other";

        if (!categories.contains(category)) {
            auto *categoryLabel = new QLabel(category);
            categoryLabel->setStyleSheet("font-size: 16px; font-weight: bold;");
            ui->metadataLayout->addWidget(categoryLabel);
            categories.insert(category);
        }

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

    valueEdit->setMinimumWidth(300);

    rowLayout->addWidget(propertyLabel);
    rowLayout->addWidget(valueEdit);
    rowLayout->addWidget(deleteButton);

    metadataFields[key] = valueEdit;
    metadataRows[key] = row;

    ui->metadataLayout->insertWidget(ui->metadataLayout->count() - 1, row);

    connect(deleteButton, &QPushButton::clicked, this, [this, key]() {
        deleteMetadataField(key);
    });
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
    QMap<QString, QString> changedTags;

    for (auto it = metadataFields.constBegin(); it != metadataFields.constEnd(); ++it) {
        QString currentValue = it.value()->text();

        if (!originalMetadata.contains(it.key()) || originalMetadata.value(it.key()) != currentValue)
            changedTags[it.key()] = currentValue;
    }

    if (changedTags.isEmpty() && deletedMetadata.isEmpty()) {
        QMessageBox::information(this, "Metadata Editor", "No changes to save.");
        return;
    }

    if (!deletedMetadata.isEmpty()) {
        QMessageBox::warning(this, "Deletion Not Supported", "Deleted fields have been removed from the editor, but your current ExifTool writeTags() function does not support deleting tags from the file.");
    }

    if (changedTags.isEmpty()) {
        QMessageBox::information(this, "Metadata Editor", "There are no additions or edits to save.");
        return;
    }

    QString error;

    if (!ExifTool::writeTags(path, changedTags, error)) {
        QMessageBox::critical(this, "Save Error", error);
        return;
    }

    QMessageBox::information(this, "Metadata Editor", "Metadata changes saved successfully.");
    populateData();
    emit metadataSaved(path);
}