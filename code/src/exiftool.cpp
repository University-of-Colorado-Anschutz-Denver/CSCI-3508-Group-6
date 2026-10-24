#include "exiftool.h"

#include <QCoreApplication>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QProcess>
#include <QStringList>

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

} // namespace ExifTool
