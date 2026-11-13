#include "exiftool.h"

#include "backend/metadata.h"

#include <QCoreApplication>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QProcess>
#include <QRegularExpression>
#include <QSet>
#include <QStringList>
#include <QTemporaryFile>

namespace ExifTool {

    QString executablePath()
    {
#ifdef Q_OS_WIN
        const QString name = QStringLiteral("exiftool.exe");
#else
        const QString name = QStringLiteral("exiftool");
#endif
        const QString appDir = QCoreApplication::applicationDirPath();

        // Same layout for the build folder and the installed app; the second entry is the macOS .app bundle
        const QStringList candidates = {
            appDir + "/exiftool/" + name,
            appDir + "/../Resources/exiftool/" + name,
        };
        for (const QString& candidate : candidates) {
            if (QFileInfo::exists(candidate))
                return QFileInfo(candidate).absoluteFilePath();
        }
        return {};
    }

    // Spawns "exiftool -json -G -- file" and parses its one-object JSON array into `out`.
    // "-G" prefixes each key with its group (e.g. "EXIF:Make"), which is what lets the rest of
    // the app tell apart same-named tags from different formats.
    bool readMetadata(const QString& file, QJsonObject& out, QString& error)
    {
        const QString exe = executablePath();
        if (exe.isEmpty()) {
            error = QStringLiteral("ExifTool was not found next to the application.");
            return false;
        }

        QProcess process;
        process.start(exe, { "-json", "-G", "--", file });
        if (!process.waitForFinished(15000)) {
            process.kill();
            error = QStringLiteral("ExifTool did not finish: ") + process.errorString();
            return false;
        }
        if (process.exitStatus() != QProcess::NormalExit || process.exitCode() != 0) {
            error = QString::fromUtf8(process.readAllStandardError()).trimmed();
            return false;
        }

        QJsonParseError parseError;
        const QJsonDocument doc = QJsonDocument::fromJson(process.readAllStandardOutput(), &parseError);
        if (parseError.error != QJsonParseError::NoError || !doc.isArray() || doc.array().isEmpty()) {
            error = QStringLiteral("Could not read ExifTool output.");
            return false;
        }
        // ExifTool always returns a JSON array, one object per input file; we only ever pass one file.
        out = doc.array().first().toObject();
        return true;
    }

    // Runs "exiftool -listwf" once (lazily, via the static initializer) and caches the result for
    // the rest of the program's life, since the writable-extensions list never changes at runtime.
    const QSet<QString>& writableExtensions()
    {
        static const QSet<QString> extensions = [] {
            QSet<QString> result;
            const QString exe = executablePath();
            if (exe.isEmpty())
                return result;
            QProcess process;
            process.start(exe, { "-listwf" });
            if (!process.waitForFinished(15000))
                return result;
            // Output: a heading line, then indented rows of upper-case extensions
            const QStringList lines = QString::fromUtf8(process.readAllStandardOutput()).split('\n');
            for (const QString& line : lines) {
                if (!line.startsWith(' '))
                    continue;
                for (const QString& word : line.split(' ', Qt::SkipEmptyParts))
                    result.insert(word.trimmed().toLower());
            }
            return result;
            }();
        return extensions;
    }

    namespace {

        // Runs ExifTool with the given arguments, one per line in a UTF-8 argument file, so accents and
        // symbols in values and paths survive on Windows. Line breaks inside an argument are not possible.
        // `configFile` (optional) is an ExifTool config; it has to come before every other option.
        bool runWithArgFile(const QStringList& lines, int timeoutMs, QString& standardOutput,
            QString& standardError, QString& error, const QString& configFile = QString())
        {
            const QString exe = executablePath();
            if (exe.isEmpty()) {
                error = QStringLiteral("ExifTool was not found next to the application.");
                return false;
            }

            QTemporaryFile argFile;
            if (!argFile.open()) {
                error = QStringLiteral("Could not create a temporary file.");
                return false;
            }
            for (const QString& line : lines)
                argFile.write(line.toUtf8() + "\n");
            argFile.close();

            // "-@ argfile" tells ExifTool to read its arguments from the file instead of argv
            QStringList arguments;
            if (!configFile.isEmpty())
                arguments << "-config" << configFile;
            arguments << "-charset" << "utf8" << "-charset" << "filename=utf8" << "-@" << argFile.fileName();

            QProcess process;
            process.start(exe, arguments);
            if (!process.waitForFinished(timeoutMs)) {
                process.kill();
                error = QStringLiteral("ExifTool did not finish: ") + process.errorString();
                return false;
            }
            standardOutput = QString::fromUtf8(process.readAllStandardOutput());
            standardError = QString::fromUtf8(process.readAllStandardError()).trimmed();
            return true;
        }

        // ExifTool argument files can't contain literal line breaks, so collapse any into spaces.
        QString oneLine(QString text)
        {
            return text.replace('\r', ' ').replace('\n', ' ');
        }

        // A JSON value that is either one string or a list of strings -> list of strings
        QStringList toStringList(const QJsonValue& value)
        {
            QStringList result;
            if (value.isArray()) {
                for (const QJsonValue& item : value.toArray())
                    result << item.toVariant().toString();
            }
            else if (!value.isUndefined() && !value.isNull()) {
                result << value.toVariant().toString();
            }
            return result;
        }

        // XMP tags ExifTool doesn't know (like "XMP:Hello") can't be written as they are. They are
        // stored instead in QuadM's own XMP namespace, which ExifTool still reads back as "XMP:Hello".
        const QString kCustomXmpGroup = QStringLiteral("XMP-quadm");

        // The keys ExifTool reported as "Tag 'X' is not defined" that we can store that way
        QSet<QString> customXmpKeys(const QString& problems, const QMap<QString, QString>& tags)
        {
            static const QRegularExpression undefinedTag("Tag '([^']+)' is not defined");
            static const QRegularExpression validName("^[A-Za-z][A-Za-z0-9]*$");
            QSet<QString> keys;
            for (auto match = undefinedTag.globalMatch(problems); match.hasNext();) {
                const QString key = match.next().captured(1);
                const bool hasGroup = key.contains(':');
                const QString group = key.section(':', 0, 0);
                const QString name = key.section(':', hasGroup ? 1 : 0);
                if (tags.contains(key) && validName.match(name).hasMatch()
                    && (!hasGroup || group.compare(QLatin1String("XMP"), Qt::CaseInsensitive) == 0))
                    keys.insert(key);
            }
            return keys;
        }

        // An ExifTool config file that declares `names` as writable text tags in the custom namespace
        bool writeCustomXmpConfig(QTemporaryFile& file, const QStringList& names)
        {
            if (!file.open())
                return false;
            QString text = QStringLiteral(
                "%Image::ExifTool::UserDefined = ('Image::ExifTool::XMP::Main' => "
                "{ quadm => { SubDirectory => { TagTable => 'Image::ExifTool::UserDefined::quadm' } } });\n"
                "%Image::ExifTool::UserDefined::quadm = (\n"
                "  GROUPS => { 0 => 'XMP', 1 => '%1', 2 => 'Other' },\n"
                "  NAMESPACE => { 'quadm' => 'https://quadm.app/ns/1.0/' },\n"
                "  WRITABLE => 'string',\n").arg(kCustomXmpGroup);
            for (const QString& name : names)
                text += "  " + name + " => { },\n";
            text += QStringLiteral(");\n1;\n");
            file.write(text.toUtf8());
            file.close();
            return true;
        }

    } // namespace

    // Writes `tags` as "-Group:Tag=value" arguments (an empty value deletes that tag), and
    // optionally replaces the keyword list (our tags) with `keywords`.
    bool writeTags(const QString& file, const QMap<QString, QString>& tags,
        const std::optional<QStringList>& keywords, QString& error)
    {
        // `custom` = keys to store in the QuadM XMP namespace instead of under their own name
        auto buildLines = [&](const QSet<QString>& custom) {
            QStringList lines = { QStringLiteral("-overwrite_original") };
            for (auto it = tags.begin(); it != tags.end(); ++it) {
                const QString name = custom.contains(it.key()) ? kCustomXmpGroup + ":" + it.key().section(':', -1)
                                                               : it.key();
                lines << "-" + name + "=" + oneLine(it.value());
            }
            if (keywords) {
                // "=" replaces the whole list (and with no value empties it); "+=" adds to it.
                // Emptying and adding in one go doesn't work, so the first tag uses "=".
                if (keywords->isEmpty())
                    lines << "-" + TagStorage::XmpWriteTag + "=";
                for (int i = 0; i < keywords->size(); ++i)
                    lines << "-" + TagStorage::XmpWriteTag + (i == 0 ? "=" : "+=") + oneLine(keywords->at(i));
            }
            lines << file;
            return lines;
        };

        QString output, problems;
        if (!runWithArgFile(buildLines({}), 30000, output, problems, error))
            return false;

        // Some tags are unknown to ExifTool: write those again through a temporary config
        // (everything else is simply rewritten with the same values)
        const QSet<QString> custom = customXmpKeys(problems, tags);
        if (!custom.isEmpty()) {
            QStringList names;
            for (const QString& key : custom)
                names << key.section(':', -1);
            QTemporaryFile config;
            if (!writeCustomXmpConfig(config, names)) {
                error = QStringLiteral("Could not create a temporary file.");
                return false;
            }
            output.clear();
            problems.clear();
            if (!runWithArgFile(buildLines(custom), 30000, output, problems, error, config.fileName()))
                return false;
        }
        // ExifTool can exit 0 even when some tags failed to write, so also check its own
        // "N image files updated" summary line and anything printed to stderr.
        if (!problems.isEmpty() || !output.contains(QLatin1String("1 image files updated"))) {
            error = problems.isEmpty() ? output.trimmed() : problems;
            error += QStringLiteral("\n(Other changes may have been saved.)");
            return false;
        }
        return true;
    }

    bool writeTags(const QString& file, const QMap<QString, QString>& tags, QString& error)
    {
        return writeTags(file, tags, std::nullopt, error);
    }

    // Finds every tagged file in `folder` in one ExifTool call ("-if" filters to files that have
    // either tag field set), instead of reading each file individually.
    bool scanTags(const QString& folder, bool recursive, QMap<QString, QStringList>& out, QString& error)
    {
        out.clear();
        // Only files that have tags are printed. ExifTool exits with an error status when no file
        // matches, so the exit code is not checked; empty output simply means "nothing tagged".
        // (No -fast: it skips the ID3 chunk at the end of WAV files.)
        QStringList lines = { "-json", "-q", "-if",
                             "$" + TagStorage::XmpWriteTag + " or $" + TagStorage::Id3Name,
                             "-" + TagStorage::XmpWriteTag, "-" + TagStorage::Id3Name };
        if (recursive)
            lines << "-r";
        lines << folder;

        QString output, problems;
        if (!runWithArgFile(lines, 120000, output, problems, error))
            return false;
        if (output.trimmed().isEmpty())
            return true;

        QJsonParseError parseError;
        const QJsonDocument doc = QJsonDocument::fromJson(output.toUtf8(), &parseError);
        if (parseError.error != QJsonParseError::NoError || !doc.isArray()) {
            error = problems.isEmpty() ? QStringLiteral("Could not read ExifTool output.") : problems;
            return false;
        }
        for (const QJsonValue& entry : doc.array()) {
            const QJsonObject object = entry.toObject();
            // XMP tags come back as a JSON array ("Subject"); ID3 tags as one ';'-joined string
            QStringList tags = toStringList(object.value("Subject"));
            for (const QString& id3Tag : toStringList(object.value(TagStorage::Id3Name)).join(';').split(';', Qt::SkipEmptyParts))
                tags << id3Tag;
            if (!tags.isEmpty())
                out.insert(object.value("SourceFile").toString(), tags);
        }
        return true;
    }

    // Reads every supported file's full metadata in one ExifTool call, instead of spawning a
    // process per file; used by the GUI's folder-wide metadata filter.
    bool readFolderMetadata(const QString& folder, bool recursive, QMap<QString, QJsonObject>& out,
        QString& error)
    {
        out.clear();
        // No -fast: it skips the end of the file, where WAV files keep their ID3 chunk (and MP3s
        // their ID3v1 tag), so the filter would never see those tags.
        QStringList lines = { "-json", "-G" };
        if (recursive)
            lines << "-r";
        lines << folder;

        QString output, problems;
        if (!runWithArgFile(lines, 120000, output, problems, error))
            return false;

        QJsonParseError parseError;
        const QJsonDocument doc = QJsonDocument::fromJson(output.toUtf8(), &parseError);
        if (parseError.error != QJsonParseError::NoError || !doc.isArray()) {
            error = problems.isEmpty() ? QStringLiteral("Could not read ExifTool output.") : problems;
            return false;
        }
        for (const QJsonValue& entry : doc.array()) {
            const QJsonObject object = entry.toObject();
            out.insert(object.value("SourceFile").toString(), object);
        }
        return true;
    }

} // namespace ExifTool

