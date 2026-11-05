#ifndef METADATAEDITOR_H
#define METADATAEDITOR_H

#include <QMainWindow>
#include <QString>
#include <QMap>
#include <QSet>
#include <QLineEdit>
#include <QWidget>

#include "api/metadataservice.h"
#include "backend/metadata.h"

namespace Ui {
class MetadataEditor;
}

class MetadataEditor : public QMainWindow
{
    Q_OBJECT

public:
    explicit MetadataEditor(const QString &filePath, QWidget *parent = nullptr);
    void setPath(const QString &filePath);
    ~MetadataEditor();
    signals:
    void metadataSaved(const QString &filePath);

private slots:
    void addMetadata();
    void saveMetadata();
    void deleteMetadataField(const QString &key);

private:
    Ui::MetadataEditor *ui;
    QString path;

    MetadataService service;
    Metadata metadata;

    QMap<QString, QString> originalMetadata;
    QMap<QString, QLineEdit*> metadataFields;
    QMap<QString, QWidget*> metadataRows;
    QSet<QString> deletedMetadata;

    void populateData();
    void addMetadataField(const QString &key, const QString &value);
};


#endif // METADATAEDITOR_H