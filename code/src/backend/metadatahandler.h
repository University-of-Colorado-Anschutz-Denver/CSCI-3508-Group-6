#pragma once

#include "metadata.h"

#include <QString>

// One way of writing metadata to files. Reading is always done by ExifTool (it understands every
// format), so handlers only decide who can write which file and how.
class MetadataHandler
{
public:
    virtual ~MetadataHandler() = default;

    virtual bool canHandle(const QString &path) const = 0;

    // Writes data.changes() to the file. On failure returns false and fills `error`.
    virtual bool write(const QString &path, const Metadata &data, QString &error) = 0;
};
