#pragma once

#include <QJsonObject>
#include <QString>

namespace ExifTool {

// Full path to the bundled ExifTool executable, or an empty string if it isn't found.
QString executablePath();

// Reads all metadata of `file` (grouped tag names, e.g. "EXIF:Make") into `out`.
// On failure returns false and puts a readable message in `error`.
bool readMetadata(const QString &file, QJsonObject &out, QString &error);

} // namespace ExifTool
