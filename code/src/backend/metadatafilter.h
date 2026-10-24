#pragma once

#include <QJsonObject>
#include <QMap>
#include <QString>

// Turns the raw ExifTool output into the dictionary the GUI shows:
// useless tags are removed and every remaining value is converted to a string.
// Keys keep ExifTool's "Group:Tag" form (e.g. "PNG:ImageWidth").
QMap<QString, QString> filterMetadata(const QJsonObject &raw);

// True if `key` ("Group:Tag") is one of the tags we never show.
bool isIgnoredTag(const QString &key);

// True if `key` is shown but must not get edit controls (ExifTool can't write it).
// Tags not in the ignored or read-only lists are treated as editable.
bool isReadOnlyTag(const QString &key);
