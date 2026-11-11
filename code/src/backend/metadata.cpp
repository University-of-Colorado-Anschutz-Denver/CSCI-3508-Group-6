#include "metadata.h"

// Stores the freshly-read data as both the current and the "original" state, so nothing counts
// as changed yet (m_fields == m_original, m_tags == m_originalTags).
void Metadata::load(const QString& fileType, const QMap<QString, QString>& fields,
    const QStringList& tags)
{
    m_fileType = fileType;
    m_fields = fields;
    m_original = fields;
    m_tags = tags;
    m_originalTags = tags;
}

// Just overwrites the in-memory value; nothing is written to the file until writeMetadata() runs.
void Metadata::set(const QString& key, const QString& value)
{
    m_fields.insert(key, value);
}

// Returns the fields that have been modified since the last load()/markSaved():
// - a brand-new key only counts if it got a non-empty value (an empty placeholder row that was
//   never filled in shouldn't trigger a write)
// - an existing key counts if its value differs from what was loaded
QMap<QString, QString> Metadata::changes() const
{
    QMap<QString, QString> result;
    for (auto it = m_fields.begin(); it != m_fields.end(); ++it) {
        if (!m_original.contains(it.key()) ? !it.value().isEmpty()
            : m_original.value(it.key()) != it.value())
            result.insert(it.key(), it.value());
    }
    return result;
}

// Trims and lower-cases the text, turns spaces into underscores, and drops anything that isn't a
// letter, digit, '_' or '-'. Keeps tags consistent no matter how the user typed them.
QString Metadata::normalizeTag(const QString& tag)
{
    QString result;
    for (const QChar c : tag.trimmed().toLower()) {
        if (c.isSpace())
            result += '_';
        else if (c.isLetterOrNumber() || c == '_' || c == '-')
            result += c;
    }
    return result;
}

// Normalizes the tag first, then rejects it if that leaves nothing or it's already present.
bool Metadata::addTag(const QString& tag)
{
    const QString clean = normalizeTag(tag);
    if (clean.isEmpty() || hasTag(clean))
        return false;
    m_tags.append(clean);
    return true;
}

// Removes every tag matching `tag` ignoring case (walks backwards so removing while iterating
// doesn't skip entries).
void Metadata::removeTag(const QString& tag)
{
    const QString wanted = tag.trimmed();
    for (int i = m_tags.size() - 1; i >= 0; --i) {
        if (m_tags[i].compare(wanted, Qt::CaseInsensitive) == 0)
            m_tags.removeAt(i);
    }
}

// Whether a tag (ignoring case) is currently present.
bool Metadata::hasTag(const QString& tag) const
{
    const QString wanted = tag.trimmed();
    for (const QString& existing : m_tags) {
        if (existing.compare(wanted, Qt::CaseInsensitive) == 0)
            return true;
    }
    return false;
}

// Whether the tag list differs from what was loaded.
bool Metadata::tagsChanged() const
{
    // The order of tags doesn't matter
    QStringList a = m_tags, b = m_originalTags;
    a.sort();
    b.sort();
    return a != b;
}

// Called once the handler has actually written the changes to disk.
void Metadata::markSaved()
{
    // Removed tags (empty value) are gone from the file, so drop them here too
    for (auto it = m_fields.begin(); it != m_fields.end();) {
        if (it.value().isEmpty())
            it = m_fields.erase(it);
        else
            ++it;
    }
    // Current state becomes the new baseline, so changes()/tagsChanged() report nothing until
    // the GUI edits something again.
    m_original = m_fields;
    m_originalTags = m_tags;
}
