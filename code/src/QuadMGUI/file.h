#ifndef FILE_H
#define FILE_H

#include <QString>

class FileManager
{
public:
    static bool isSupportedFile(const QString &path);
};

#endif // FILE_H