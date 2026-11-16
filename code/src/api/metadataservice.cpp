#include "metadataservice.h"

#include "backend/exiftoolhandler.h"
#include "backend/metadatafilter.h"
#include "backend/taglibhandler.h"
#include "exiftool.h"

#include <QJsonArray>

MetadataService::MetadataService()
{
    // Order matters: the first handler whose canHandle() returns true wins, and TagLib must
    // claim MP3/WAV before the generic ExifTool handler gets a chance to.
    m_handlers.push_back(std::make_unique<TagLibHandler>());   // MP3, WAV
    m_handlers.push_back(std::make_unique<ExifToolHandler>()); // everything else ExifTool can write
}

// Reads a file's metadata and turns it into a GUI-ready Metadata object: strips noise, adds
// blank placeholder fields, and separates out the tags (labels) from ordinary fields.
bool MetadataService::readMetadata(const QString& path, Metadata& out, QString& error) const
{
    QJsonObject raw;
    if (!ExifTool::readMetadata(path, raw, error))
        return false;

    // The tags are kept apart from the normal fields (the GUI shows them as their own rows, not
    // mixed in with the rest of the metadata table)
    QMap<QString, QString> fields = filterMetadata(raw);
    const QString fileType = raw.value(QStringLiteral("File:FileType")).toString();
    addExpectedTags(fields, fileType);
    QStringList tags;
    // XMP tags come back as either a JSON array (multiple) or a single string/number (just one)
    const QJsonValue xmp = raw.value(TagStorage::XmpReadKey);
    if (xmp.isArray()) {
        for (const QJsonValue& item : xmp.toArray())
            tags << item.toVariant().toString();
    }
    else if (xmp.isString() || xmp.isDouble()) {
        tags << xmp.toVariant().toString();
    }
    // ID3 tags are one ';'-joined string instead
    tags << raw.value(TagStorage::Id3ReadKey).toString().split(';', Qt::SkipEmptyParts);
    // Tags live in these two fields on disk, but shouldn't also show up as editable metadata rows
    fields.remove(TagStorage::XmpReadKey);
    fields.remove(TagStorage::Id3ReadKey);
    tags.removeDuplicates();

    out.load(fileType, fields, tags);
    return true;
}

// Validates and writes only the changed fields of `data`, through whichever handler claims
// this file type.
bool MetadataService::writeMetadata(const QString& path, Metadata& data, QString& error) const
{
    if (!data.hasChanges())
        return true;

    // Reject any attempt to write a read-only/composite tag before touching the file at all
    const QMap<QString, QString> changes = data.changes();
    for (auto it = changes.begin(); it != changes.end(); ++it) {
        if (!isEditable(it.key())) {
            error = QStringLiteral("%1 can't be edited.").arg(it.key());
            return false;
        }
    }

    MetadataHandler* handler = getHandler(path);
    if (!handler) {
        error = QStringLiteral("This file type can't be written.");
        return false;
    }
    if (!handler->write(path, data, error))
        return false;

    // Only mark as saved once the handler actually wrote successfully
    data.markSaved();
    return true;
}

bool MetadataService::scanFolderTags(const QString& folder, bool recursive,
    QMap<QString, QStringList>& out, QString& error) const
{
    return ExifTool::scanTags(folder, recursive, out, error);
}

// Batch-reads every file in the folder in one ExifTool call and runs each result through the
// same filterMetadata() used for single-file reads, so the filter bar sees the same fields
// as the editor would show.
bool MetadataService::scanFolderMetadata(const QString& folder, bool recursive,
    QMap<QString, QMap<QString, QString>>& out, QString& error) const
{
    out.clear();
    QMap<QString, QJsonObject> raw;
    if (!ExifTool::readFolderMetadata(folder, recursive, raw, error))
        return false;

    for (auto it = raw.begin(); it != raw.end(); ++it) {
        QMap<QString, QString> fields = filterMetadata(it.value());
        // Same blank "expected" tags the editor shows, so filtering by a tag name (e.g. "Artist")
        // also matches files of that type that don't have a value for it yet
        addExpectedTags(fields, it.value().value(QStringLiteral("File:FileType")).toString());
        out.insert(it.key(), fields);
    }
    return true;
}

bool MetadataService::findByTag(const QString& folder, const QString& tag, bool recursive,
    QStringList& files, QString& error) const
{
    files.clear();
    QMap<QString, QStringList> tagged;
    if (!ExifTool::scanTags(folder, recursive, tagged, error))
        return false;

    const QString wanted = Metadata::normalizeTag(tag);
    for (auto it = tagged.begin(); it != tagged.end(); ++it) {
        for (const QString& fileTag : it.value()) {
            // Files tagged by other programs may not be normalized, so compare normalized forms
            if (Metadata::normalizeTag(fileTag) == wanted) {
                files << it.key();
                break;
            }
        }
    }
    return true;
}

bool MetadataService::canWrite(const QString& path) const
{
    return getHandler(path) != nullptr;
}

bool MetadataService::isEditable(const QString& key)
{
    // Composite tags are calculated by ExifTool, so they can't be written either
    return !isReadOnlyTag(key) && !key.startsWith(QLatin1String("Composite:"));
}

// Returns the first registered handler that claims this file, or nullptr if none does.
MetadataHandler* MetadataService::getHandler(const QString& path) const
{
    for (const auto& handler : m_handlers) {
        if (handler->canHandle(path))
            return handler.get();
    }
    return nullptr;
}
