#include "metadata.h"

void Metadata::load(const QString &fileType, const QMap<QString, QString> &fields,
                    const QStringList &tags)
{
    m_fileType = fileType;
    m_fields = fields;
    m_original = fields;
    m_tags = tags;
    m_originalTags = tags;
}

void Metadata::set(const QString &key, const QString &value)
{
    m_fields.insert(key, value);
}

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

QString Metadata::normalizeTag(const QString &tag)
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

bool Metadata::addTag(const QString &tag)
{
    const QString clean = normalizeTag(tag);
    if (clean.isEmpty() || hasTag(clean))
        return false;
    m_tags.append(clean);
    return true;
}

void Metadata::removeTag(const QString &tag)
{
    const QString wanted = tag.trimmed();
    for (int i = m_tags.size() - 1; i >= 0; --i) {
        if (m_tags[i].compare(wanted, Qt::CaseInsensitive) == 0)
            m_tags.removeAt(i);
    }
}

bool Metadata::hasTag(const QString &tag) const
{
    const QString wanted = tag.trimmed();
    for (const QString &existing : m_tags) {
        if (existing.compare(wanted, Qt::CaseInsensitive) == 0)
            return true;
    }
    return false;
}

bool Metadata::tagsChanged() const
{
    // The order of tags doesn't matter
    QStringList a = m_tags, b = m_originalTags;
    a.sort();
    b.sort();
    return a != b;
}

void Metadata::markSaved()
{
    // Removed tags (empty value) are gone from the file, so drop them here too
    for (auto it = m_fields.begin(); it != m_fields.end();) {
        if (it.value().isEmpty())
            it = m_fields.erase(it);
        else
            ++it;
    }
    m_original = m_fields;
    m_originalTags = m_tags;
}
