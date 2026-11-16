#pragma once

#include "metadatahandler.h"

// Writes MP3 and WAV files with TagLib, because ExifTool can't write those formats.
//
// Keys are the ones ExifTool reports when reading: "ID3:Title", "RIFF:Artist", ...
// - Title, Artist, Album, Genre, Comment, Year, Track are standard fields. They are written to the
//   ID3v2 tag and, for WAV, also to the RIFF INFO chunk, whichever of the two group names is used.
// - Any other "ID3:Name" becomes a custom ID3v2 "TXXX" frame called Name (e.g. "ID3:QuadM_Rating").
// - An empty value removes the field.
// - The tags (labels, see Metadata::tags) are stored in the custom ID3v2 frame "QuadM_Tags".
class TagLibHandler : public MetadataHandler
{
public:
    // True only for .mp3/.wav; ExifTool handles reading these but can't reliably write them.
    bool canHandle(const QString& path) const override;
    // Opens the file with TagLib, applies every changed field/tag to the ID3v2 (and WAV INFO)
    // tag, then saves.
    bool write(const QString& path, const Metadata& data, QString& error) override;
};
