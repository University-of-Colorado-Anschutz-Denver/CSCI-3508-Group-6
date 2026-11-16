#include "file.h"

#include <QFileInfo>
#include <QMimeDatabase>

bool FileManager::isSupportedFile(const QString &path)
{
    QFileInfo info(path);

    if (!info.isFile() || !info.isReadable())
        return false;

    QMimeDatabase mimeDatabase;
    QString mime = mimeDatabase.mimeTypeForFile(info).name();

    return mime.startsWith("image/") ||
           mime.startsWith("audio/") ||
           mime.startsWith("video/");
}