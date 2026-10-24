#include "exiftool.h"

#include "backend/metadata.h"

#include <QCoreApplication>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QProcess>
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
    for (const QString &candidate : candidates) {
        if (QFileInfo::exists(candidate))
            return QFileInfo(candidate).absoluteFilePath();
    }
    return {};
}

bool readMetadata(const QString &file, QJsonObject &out, QString &error)
{
    const QString exe = executablePath();
    if (exe.isEmpty()) {
        error = QStringLiteral("ExifTool was not found next to the application.");
        return false;
    }

    QProcess process;
    process.start(exe, {"-json", "-G", "--", file});
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
    out = doc.array().first().toObject();
    return true;
}

const QSet<QString> &writableExtensions()
{
    static const QSet<QString> extensions = [] {
        QSet<QString> result;
        const QString exe = executablePath();
        if (exe.isEmpty())
            return result;
        QProcess process;
        process.start(exe, {"-listwf"});
        if (!process.waitForFinished(15000))
            return result;
        // Output: a heading line, then indented rows of upper-case extensions
        const QStringList lines = QString::fromUtf8(process.readAllStandardOutput()).split('\n');
        for (const QString &line : lines) {
            if (!line.startsWith(' '))
                continue;
            for (const QString &word : line.split(' ', Qt::SkipEmptyParts))
                result.insert(word.trimmed().toLower());
        }
        return result;
    }();
    return extensions;
}

namespace {

// Runs ExifTool with the given arguments, one per line in a UTF-8 argument file, so accents and
// symbols in values and paths survive on Windows. Line breaks inside an argument are not possible.
bool runWithArgFile(const QStringList &lines, int timeoutMs, QString &standardOutput,
                    QString &standardError, QString &error)
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
    for (const QString &line : lines)
        argFile.write(line.toUtf8() + "\n");
    argFile.close();

    QProcess process;
    process.start(exe, {"-charset", "utf8", "-charset", "filename=utf8", "-@", argFile.fileName()});
    if (!process.waitForFinished(timeoutMs)) {
        process.kill();
        error = QStringLiteral("ExifTool did not finish: ") + process.errorString();
        return false;
    }
    standardOutput = QString::fromUtf8(process.readAllStandardOutput());
    standardError = QString::fromUtf8(process.readAllStandardError()).trimmed();
    return true;
}

QString oneLine(QString text)
{
    return text.replace('\r', ' ').replace('\n', ' ');
}

// A JSON value that is either one string or a list of strings -> list of strings
QStringList toStringList(const QJsonValue &value)
{
    QStringList result;
    if (value.isArray()) {
        for (const QJsonValue &item : value.toArray())
            result << item.toVariant().toString();
    } else if (!value.isUndefined() && !value.isNull()) {
        result << value.toVariant().toString();
    }
    return result;
}

} // namespace

bool writeTags(const QString &file, const QMap<QString, QString> &tags,
               const std::optional<QStringList> &keywords, QString &error)
{
    QStringList lines = {QStringLiteral("-overwrite_original")};
    for (auto it = tags.begin(); it != tags.end(); ++it)
        lines << "-" + it.key() + "=" + oneLine(it.value());
    if (keywords) {
        // "=" replaces the whole list (and with no value empties it); "+=" adds to it.
        // Emptying and adding in one go doesn't work, so the first tag uses "=".
        if (keywords->isEmpty())
            lines << "-" + TagStorage::XmpWriteTag + "=";
        for (int i = 0; i < keywords->size(); ++i)
            lines << "-" + TagStorage::XmpWriteTag + (i == 0 ? "=" : "+=") + oneLine(keywords->at(i));
    }
    lines << file;

    QString output, problems;
    if (!runWithArgFile(lines, 30000, output, problems, error))
        return false;

    if (!problems.isEmpty() || !output.contains(QLatin1String("1 image files updated"))) {
        error = problems.isEmpty() ? output.trimmed() : problems;
        error += QStringLiteral("\n(Other changes may have been saved.)");
        return false;
    }
    return true;
}

bool writeTags(const QString &file, const QMap<QString, QString> &tags, QString &error)
{
    return writeTags(file, tags, std::nullopt, error);
}

bool scanTags(const QString &folder, bool recursive, QMap<QString, QStringList> &out, QString &error)
{
    out.clear();
    // Only files that have tags are printed. ExifTool exits with an error status when no file
    // matches, so the exit code is not checked; empty output simply means "nothing tagged".
    // (No -fast: it skips the ID3 chunk at the end of WAV files.)
    QStringList lines = {"-json", "-q", "-if",
                         "$" + TagStorage::XmpWriteTag + " or $" + TagStorage::Id3Name,
                         "-" + TagStorage::XmpWriteTag, "-" + TagStorage::Id3Name};
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
    for (const QJsonValue &entry : doc.array()) {
        const QJsonObject object = entry.toObject();
        QStringList tags = toStringList(object.value("Subject"));
        for (const QString &id3Tag : toStringList(object.value(TagStorage::Id3Name)).join(';').split(';', Qt::SkipEmptyParts))
            tags << id3Tag;
        if (!tags.isEmpty())
            out.insert(object.value("SourceFile").toString(), tags);
    }
    return true;
}

} // namespace ExifTool

