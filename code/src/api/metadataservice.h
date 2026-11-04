#pragma once

#include "backend/metadata.h"
#include "backend/metadatahandler.h"

#include <QString>

#include <memory>
#include <vector>

// The one entry point the GUI uses to read and write a file's metadata.
class MetadataService
{
public:
    MetadataService();

    // Reads the file with ExifTool and hides the tags we never show (see metadatafilter.cpp).
    bool readMetadata(const QString &path, Metadata &out, QString &error) const;

    // Writes the edited fields of `data` to the file. On success the edits count as saved.
    // Fails with a readable message for file types nobody can write or for read-only tags.
    bool writeMetadata(const QString &path, Metadata &data, QString &error) const;

    // Whether the file's type can be written at all (the GUI can disable editing if not).
    bool canWrite(const QString &path) const;

    // Whether the GUI may offer an edit control for this tag.
    static bool isEditable(const QString &key);

    // Finds the files in `folder` (and subfolders if `recursive`) that carry the tag, e.g. "forest_zone_1".
    // `files` gets their full paths. This runs ExifTool over the folder, so it can take a while.
    bool findByTag(const QString &folder, const QString &tag, bool recursive, QStringList &files,
                   QString &error) const;

    // Every tagged file in the folder with its tags ("path" -> tags). Useful to build a list of
    // existing tags for suggestions, or an index so searching doesn't need to scan again.
    bool scanFolderTags(const QString &folder, bool recursive, QMap<QString, QStringList> &out,
                        QString &error) const;

private:
    MetadataHandler *getHandler(const QString &path) const;

    // Checked in order; the first handler that can handle the file wins.
    std::vector<std::unique_ptr<MetadataHandler>> m_handlers;
};
