# ExifTool

The app reads metadata by running [ExifTool](https://exiftool.org) with `QProcess`. The ExifTool files are **not committed** (see the root `.gitignore`), so every teammate downloads them once and puts them in this folder.

## Windows

1. Download the **Windows Executable** zip (`exiftool-XX.XX_64.zip`) from https://exiftool.org (the download link goes to SourceForge).
2. Unzip it. You get `exiftool(-k).exe` and an `exiftool_files` folder.
3. Copy **both** into this folder.
4. Rename `exiftool(-k).exe` to `exiftool.exe`. (The `(-k)` name makes it wait for a keypress, which hangs under `QProcess`.)

Result:

```text
code/third_party/exiftool/
|-- exiftool.exe
`-- exiftool_files/
```

Check it from the `code` folder:

```powershell
.\third_party\exiftool\exiftool.exe -ver
```

## macOS

1. Download **Image-ExifTool-XX.XX.tar.gz** (the "Full Distribution") from https://exiftool.org.
2. Extract it and copy the **contents** of the extracted `Image-ExifTool-XX.XX` folder (at least the `exiftool` script and the `lib` folder) into this folder.

Result:

```text
code/third_party/exiftool/
|-- exiftool
`-- lib/
```

Check it from the `code` folder:

```bash
./third_party/exiftool/exiftool -ver
```

macOS includes Perl, so nothing else is needed. If you get `Permission denied`, run `chmod +x third_party/exiftool/exiftool`.

## Using it from Qt

CMake defines `EXIFTOOL_DIR` as the absolute path to this folder, so the code doesn't depend on where the app is run from:

```cpp
#ifdef Q_OS_WIN
const QString exiftool = QStringLiteral(EXIFTOOL_DIR "/exiftool.exe");
#else
const QString exiftool = QStringLiteral(EXIFTOOL_DIR "/exiftool");
#endif

QProcess p;
p.start(exiftool, {"-json", "-G", filePath});
if (!p.waitForFinished() || p.exitCode() != 0) {
    // ExifTool missing or failed: show p.errorString() / p.readAllStandardError()
}
QJsonDocument doc = QJsonDocument::fromJson(p.readAllStandardOutput());
```

- Pass arguments as a list, never as one shell string.
- `-json` output is easy to parse with `QJsonDocument`. `-G` includes the tag group name.
- On Windows, `exiftool_files` must stay in the same folder as `exiftool.exe`.
