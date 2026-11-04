#include "metadataservice.h"

#include "backend/exiftoolhandler.h"
#include "backend/metadatafilter.h"
#include "backend/taglibhandler.h"
#include "exiftool.h"

#include <QJsonArray>

MetadataService::MetadataService()
{
    m_handlers.push_back(std::make_unique<TagLibHandler>());   // MP3, WAV
    m_handlers.push_back(std::make_unique<ExifToolHandler>()); // everything else ExifTool can write
}

bool MetadataService::readMetadata(const QString &path, Metadata &out, QString &error) const
{
    QJsonObject raw;
    if (!ExifTool::readMetadata(path, raw, error))
        return false;

    // The tags are kept apart from the normal fields (the GUI shows them as chips, not as rows)
    QMap<QString, QString> fields = filterMetadata(raw);
    QStringList tags;
    const QJsonValue xmp = raw.value(TagStorage::XmpReadKey);
    if (xmp.isArray()) {
        for (const QJsonValue &item : xmp.toArray())
            tags << item.toVariant().toString();
    } else if (xmp.isString() || xmp.isDouble()) {
        tags << xmp.toVariant().toString();
    }
    tags << raw.value(TagStorage::Id3ReadKey).toString().split(';', Qt::SkipEmptyParts);
    fields.remove(TagStorage::XmpReadKey);
    fields.remove(TagStorage::Id3ReadKey);
    tags.removeDuplicates();

    out.load(raw.value(QStringLiteral("File:FileType")).toString(), fields, tags);
    return true;
}

bool MetadataService::writeMetadata(const QString &path, Metadata &data, QString &error) const
{
    if (!data.hasChanges())
        return true;

    const QMap<QString, QString> changes = data.changes();
    for (auto it = changes.begin(); it != changes.end(); ++it) {
        if (!isEditable(it.key())) {
            error = QStringLiteral("%1 can't be edited.").arg(it.key());
            return false;
        }
    }

    MetadataHandler *handler = getHandler(path);
    if (!handler) {
        error = QStringLiteral("This file type can't be written.");
        return false;
    }
    if (!handler->write(path, data, error))
        return false;

    data.markSaved();
    return true;
}

bool MetadataService::scanFolderTags(const QString &folder, bool recursive,
                                     QMap<QString, QStringList> &out, QString &error) const
{
    return ExifTool::scanTags(folder, recursive, out, error);
}

bool MetadataService::findByTag(const QString &folder, const QString &tag, bool recursive,
                                QStringList &files, QString &error) const
{
    files.clear();
    QMap<QString, QStringList> tagged;
    if (!ExifTool::scanTags(folder, recursive, tagged, error))
        return false;

    const QString wanted = Metadata::normalizeTag(tag);
    for (auto it = tagged.begin(); it != tagged.end(); ++it) {
        for (const QString &fileTag : it.value()) {
            // Files tagged by other programs may not be normalized, so compare normalized forms
            if (Metadata::normalizeTag(fileTag) == wanted) {
                files << it.key();
                break;
            }
        }
    }
    return true;
}

bool MetadataService::canWrite(const QString &path) const
{
    return getHandler(path) != nullptr;
}

bool MetadataService::isEditable(const QString &key)
{
    // Composite tags are calculated by ExifTool, so they can't be written either
    return !isReadOnlyTag(key) && !key.startsWith(QLatin1String("Composite:"));
}

MetadataHandler *MetadataService::getHandler(const QString &path) const
{
    for (const auto &handler : m_handlers) {
        if (handler->canHandle(path))
            return handler.get();
    }
    return nullptr;
}
