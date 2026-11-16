#pragma once

#include <QMap>
#include <QString>
#include <QStringList>

// Where a file's tags (labels such as "forest_zone_1") are stored inside the file.
namespace TagStorage {
    // ExifTool formats (PNG, JPG, MP4, ...): the standard XMP keywords list.
    inline const QString XmpReadKey = QStringLiteral("XMP:Subject");     // name when reading with -G
    inline const QString XmpWriteTag = QStringLiteral("XMP-dc:Subject"); // name when writing
    // MP3 and WAV: one custom ID3v2 frame holding the tags separated by ';'.
    inline const QString Id3Name = QStringLiteral("QuadM_Tags");
    inline const QString Id3ReadKey = QStringLiteral("ID3:QuadM_Tags");
} // namespace TagStorage

// The metadata of one file: its type, a dictionary of "Group:Tag" -> text value, and a list of
// tags (labels the user attaches to find files again). It remembers what it was loaded with,
// so it can report which fields and tags were edited.
class Metadata
{
public:
    // The file type as reported by ExifTool, e.g. "MP3", "PNG" (used to pick expected tags).
    QString fileType() const { return m_fileType; }

    // Fills the object with freshly read data and forgets any edits (m_fields becomes equal to
    // m_original, so changes() starts out empty until set()/addTag()/removeTag() are called).
    void load(const QString& fileType, const QMap<QString, QString>& fields, const QStringList& tags);

    // Whether a field key (e.g. "ID3:Title") currently has a value.
    bool contains(const QString& key) const { return m_fields.contains(key); }
    // The current value of a field, or an empty string if it isn't set.
    QString get(const QString& key) const { return m_fields.value(key); }
    // Every current field, including ones that haven't changed since load().
    const QMap<QString, QString>& fields() const { return m_fields; }

    // Sets a value. An empty value means "remove this tag from the file".
    void set(const QString& key, const QString& value);

    // Only the fields that were added or changed since load()/markSaved(); this is what actually
    // gets written to disk, so unchanged fields are never rewritten.
    QMap<QString, QString> changes() const;

    // --- Tags (labels the user attaches to a file, e.g. "forest_zone_1"; shown in the GUI as rows) ---
    // The current list of tags.
    QStringList tags() const { return m_tags; }

    // Cleans a tag: trims, lower-cases, spaces become '_', anything except letters, digits,
    // '_' and '-' is removed. "Forest Zone 1" -> "forest_zone_1". Empty result = invalid.
    static QString normalizeTag(const QString& tag);

    // Adds a tag (after normalizing it). Returns false if it is invalid or already present.
    bool addTag(const QString& tag);
    // Removes a tag (compared without regard to upper/lower case).
    void removeTag(const QString& tag);
    // Whether a tag (compared without regard to upper/lower case) is currently present.
    bool hasTag(const QString& tag) const;
    // Whether the tag list differs from what was loaded (order doesn't count).
    bool tagsChanged() const;

    // True if any field or the tags were edited since load()/markSaved().
    bool hasChanges() const { return !changes().isEmpty() || tagsChanged(); }

    // Call after the changes were written to the file successfully: makes the current state the
    // new "original", so changes()/hasChanges()/tagsChanged() go back to reporting nothing changed.
    void markSaved();

private:
    QString m_fileType;
    QMap<QString, QString> m_fields;
    QMap<QString, QString> m_original;
    QStringList m_tags;
    QStringList m_originalTags;
};
