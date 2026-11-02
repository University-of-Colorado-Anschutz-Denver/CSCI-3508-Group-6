#pragma once

#include <QJsonObject>
#include <QMap>
#include <QSet>
#include <QString>
#include <QStringList>

#include <optional>

namespace ExifTool {

// Full path to the bundled ExifTool executable, or an empty string if it isn't found.
QString executablePath();

// Reads all metadata of `file` (grouped tag names, e.g. "EXIF:Make") into `out`.
// On failure returns false and puts a readable message in `error`.
bool readMetadata(const QString &file, QJsonObject &out, QString &error);

// File extensions (lower case, e.g. "png") that ExifTool can write. Asked from ExifTool once, then cached.
const QSet<QString> &writableExtensions();

// Writes `tags` ("Group:Tag" -> value) into `file`. An empty value removes the tag.
// If `keywords` is set, the file's keyword list (our tags) is replaced by it.
// Any problem ExifTool reports (unknown or unwritable tag, ...) makes this return false;
// tags that were fine may already have been written in that case.
bool writeTags(const QString &file, const QMap<QString, QString> &tags,
               const std::optional<QStringList> &keywords, QString &error);
bool writeTags(const QString &file, const QMap<QString, QString> &tags, QString &error);

// Finds every file in `folder` (and its subfolders if `recursive`) that has tags and
// returns "file path" -> its tags. Files without tags are left out. Can take a while for big folders.
bool scanTags(const QString &folder, bool recursive, QMap<QString, QStringList> &out, QString &error);

} // namespace ExifTool
