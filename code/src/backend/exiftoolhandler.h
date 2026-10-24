#pragma once

#include "metadatahandler.h"

// Writes images, MP4/MOV and everything else ExifTool is able to write.
class ExifToolHandler : public MetadataHandler
{
public:
    bool canHandle(const QString &path) const override;
    bool write(const QString &path, const Metadata &data, QString &error) override;
};
