#include "taglibhandler.h"

#include <QFile>
#include <QFileInfo>
#include <QStringList>

#include <id3v2framefactory.h>
#include <id3v2tag.h>
#include <infotag.h>
#include <mpegfile.h>
#include <textidentificationframe.h>
#include <wavfile.h>

namespace {

TagLib::String toTagLib(const QString &text)
{
    return TagLib::String(text.toStdWString());
}

// TagLib wants a wide-character path on Windows and a byte path elsewhere.
#ifdef Q_OS_WIN
using NativePath = std::wstring;
NativePath nativePath(const QString &file) { return file.toStdWString(); }
const wchar_t *cstr(const NativePath &p) { return p.c_str(); }
#else
using NativePath = QByteArray;
NativePath nativePath(const QString &file) { return QFile::encodeName(file); }
const char *cstr(const NativePath &p) { return p.constData(); }
#endif

// Sets one of the fields every tag type has. `name` must be lower case. Returns false if it isn't one.
bool setStandardField(TagLib::Tag *tag, const QString &name, const QString &text)
{
    if (name == "title")        tag->setTitle(toTagLib(text));
    else if (name == "artist")  tag->setArtist(toTagLib(text));
    else if (name == "album")   tag->setAlbum(toTagLib(text));
    else if (name == "genre")   tag->setGenre(toTagLib(text));
    else if (name == "comment") tag->setComment(toTagLib(text));
    else if (name == "year" || name == "recordingtime") tag->setYear(text.toUInt());
    else if (name == "track")   tag->setTrack(text.toUInt());
    else return false;
    return true;
}

bool isStandardName(const QString &name)
{
    static const QStringList names = {"title", "artist", "album", "genre", "comment",
                                      "year", "recordingtime", "track"};
    return names.contains(name);
}

// Custom fields are ID3v2 "TXXX" frames: a name (description) plus a value.
void setCustomField(TagLib::ID3v2::Tag *tag, const QString &name, const QString &value)
{
    const TagLib::String description = toTagLib(name);
    auto *frame = TagLib::ID3v2::UserTextIdentificationFrame::find(tag, description);
    if (value.isEmpty()) {
        if (frame)
            tag->removeFrame(frame, true);
        return;
    }
    if (!frame) {
        frame = new TagLib::ID3v2::UserTextIdentificationFrame(TagLib::String::UTF8);
        frame->setDescription(description);
        tag->addFrame(frame); // the tag takes ownership
    }
    frame->setText(toTagLib(value));
}

// Applies the edits (fields and tags) to the ID3v2 tag and (for WAV) the INFO tag. Returns false
// and sets `error` for a key that can't be written to this kind of tag.
bool applyChanges(TagLib::ID3v2::Tag *id3, TagLib::Tag *info, const Metadata &data, QString &error)
{
    const QMap<QString, QString> changes = data.changes();
    for (auto it = changes.begin(); it != changes.end(); ++it) {
        const bool hasGroup = it.key().contains(':');
        const QString group = hasGroup ? it.key().section(':', 0, 0).toUpper() : QStringLiteral("ID3");
        const QString name = it.key().section(':', hasGroup ? 1 : 0);

        if (isStandardName(name.toLower())) {
            setStandardField(id3, name.toLower(), it.value());
            if (info)
                setStandardField(info, name.toLower(), it.value());
        } else if (group == "ID3") {
            setCustomField(id3, name, it.value());
        } else {
            error = QStringLiteral("%1 can't be edited in this file type.").arg(it.key());
            return false;
        }
    }

    // Our tags (labels) are one custom frame; an empty list removes it
    if (data.tagsChanged())
        setCustomField(id3, TagStorage::Id3Name, data.tags().join(';'));
    return true;
}

} // namespace

bool TagLibHandler::canHandle(const QString &path) const
{
    const QString ext = QFileInfo(path).suffix().toLower();
    return ext == "mp3" || ext == "wav";
}

bool TagLibHandler::write(const QString &path, const Metadata &data, QString &error)
{
    if (!QFileInfo(path).isWritable()) {
        error = QStringLiteral("The file does not exist or is read-only.");
        return false;
    }

    // Write text as UTF-16 so accented/non-Latin characters survive in every player
    TagLib::ID3v2::FrameFactory::instance()->setDefaultTextEncoding(TagLib::String::UTF16);

    const NativePath native = nativePath(path);
    bool saved = false;

    if (QFileInfo(path).suffix().compare("wav", Qt::CaseInsensitive) == 0) {
        TagLib::RIFF::WAV::File f(cstr(native));
        if (!f.isValid() || !f.ID3v2Tag()) {
            error = QStringLiteral("Could not read this WAV file.");
            return false;
        }
        if (!applyChanges(f.ID3v2Tag(), f.InfoTag(), data, error))
            return false;
        saved = f.save();
    } else {
        TagLib::MPEG::File f(cstr(native));
        if (!f.isValid()) {
            error = QStringLiteral("Could not read this MP3 file.");
            return false;
        }
        if (!applyChanges(f.ID3v2Tag(true), nullptr, data, error)) // true = create if missing
            return false;
        // ID3v2.3 is understood by more players than 2.4 (e.g. Windows Explorer)
        saved = f.save(TagLib::MPEG::File::ID3v2, TagLib::File::StripNone, TagLib::ID3v2::v3);
    }

    if (!saved)
        error = QStringLiteral("Could not save the file (is it open in another program?).");
    return saved;
}
