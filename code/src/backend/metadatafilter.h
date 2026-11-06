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

// Inserts an empty value for each commonly-edited tag of `fileType` that isn't already in
// `fields`, so the GUI shows a blank, fillable row instead of no row at all. ExifTool/TagLib
// only ever report tags that already have a value, so this has to be filled in on our side.
void addExpectedTags(QMap<QString, QString> &fields, const QString &fileType);