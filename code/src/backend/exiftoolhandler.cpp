#include "exiftoolhandler.h"

#include "exiftool.h"

#include <QFileInfo>

bool ExifToolHandler::canHandle(const QString &path) const
{
    return ExifTool::writableExtensions().contains(QFileInfo(path).suffix().toLower());
}

bool ExifToolHandler::write(const QString &path, const Metadata &data, QString &error)
{
    // The tags are stored as the file's keyword list; only rewrite it if the tags changed
    std::optional<QStringList> keywords;
    if (data.tagsChanged())
        keywords = data.tags();
    return ExifTool::writeTags(path, data.changes(), keywords, error);
}
