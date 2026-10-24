#include "metadatafilter.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QSet>

namespace {

// Everything in these groups is dropped (the part before the colon in "Group:Tag").
// ICC_Profile is deliberately NOT here: designers need the color profile.
const QSet<QString> kIgnoredGroups = {
    "ExifTool",    // ExifTool's own version/warnings
    "System",
    "MakerNotes",  // camera-specific internals
};

// Individual tags that are dropped. Add new "useless" tags here.
const QSet<QString> kIgnoredTags = {
    "SourceFile",
    "File:Directory",
    "File:FilePermissions",
    "File:FileAccessDate",     // changes every time the file is read
    "File:FileInodeChangeDate",
    "File:FileTypeExtension",
    "File:ID3Size",
    "EXIF:ThumbnailImage",
    "EXIF:PrintIM",
};

// Shown, but ExifTool can't write them (they are part of the PNG header or computed),
// so the GUI should not offer edit controls. Gamma, sRGB and pixel density ARE writable.
const QSet<QString> kReadOnlyTags = {
    "PNG:ImageWidth",
    "PNG:ImageHeight",
    "PNG:BitDepth",
    "PNG:ColorType",
    "PNG:Compression",
    "PNG:Filter",
    "PNG:Interlace",
    "File:FileName",
    "File:FileSize",
    "File:FileType",
    "File:MIMEType",
    "File:FileModifyDate",
    "File:FileCreateDate",
    "Composite:ImageSize",
    "Composite:Megapixels",
};

// Turns any JSON value (string, number, bool, array, object) into display text.
QString valueToString(const QJsonValue &value)
{
    switch (value.type()) {
    case QJsonValue::String:
        return value.toString();
    case QJsonValue::Array:
        return QString::fromUtf8(QJsonDocument(value.toArray()).toJson(QJsonDocument::Compact));
    case QJsonValue::Object:
        return QString::fromUtf8(QJsonDocument(value.toObject()).toJson(QJsonDocument::Compact));
    default:
        return value.toVariant().toString(); // numbers and booleans
    }
}

// ExifTool prints binary data (thumbnails, previews) as text starting with one of these.
bool isBinaryPlaceholder(const QString &text)
{
    return text.startsWith(QLatin1String("(Binary data")) || text.startsWith(QLatin1String("base64:"));
}

} // namespace

bool isIgnoredTag(const QString &key)
{
    return kIgnoredTags.contains(key) || kIgnoredGroups.contains(key.section(':', 0, 0));
}

bool isReadOnlyTag(const QString &key)
{
    return kReadOnlyTags.contains(key);
}

QMap<QString, QString> filterMetadata(const QJsonObject &raw)
{
    QMap<QString, QString> result;
    for (auto it = raw.begin(); it != raw.end(); ++it) {
        if (isIgnoredTag(it.key()))
            continue;

        const QString text = valueToString(it.value());
        if (isBinaryPlaceholder(text))
            continue;

        result.insert(it.key(), text);
    }
    return result;
}
