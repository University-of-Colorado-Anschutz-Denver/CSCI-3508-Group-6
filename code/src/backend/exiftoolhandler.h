#pragma once

#include "metadatahandler.h"

// Writes images, MP4/MOV and everything else ExifTool is able to write.
class ExifToolHandler : public MetadataHandler
{
public:
    // True if ExifTool reports this file's extension as writable (see ExifTool::writableExtensions()).
    bool canHandle(const QString& path) const override;
    // Forwards the changed fields (and tags, if changed) to ExifTool::writeTags().
    bool write(const QString& path, const Metadata& data, QString& error) override;
};
