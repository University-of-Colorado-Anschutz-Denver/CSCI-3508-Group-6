# ExifTool

The app reads metadata by running [ExifTool](https://exiftool.org) with `QProcess`. The ExifTool files are **not committed** (see the root `.gitignore`), so every teammate downloads them once and puts them in this folder.

## Windows

1. Download the **Windows Executable** zip (`exiftool-XX.XX_64.zip`) from https://exiftool.org (the download link goes to SourceForge).
2. Unzip it. Inside its `exiftool-XX.XX_64` folder you get `exiftool(-k).exe` and an `exiftool_files` folder.
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

## macOS / Linux

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

macOS and most Linux distros include Perl, so nothing else is needed. If you get `Permission denied`, run `chmod +x third_party/exiftool/exiftool`.

## How the app finds it

The build copies this folder to `build/exiftool/`, next to the app. The installer must ship the same layout, so the app finds ExifTool the same way when installed:

```text
Windows / Linux:  <install folder>/QuadM.exe  and  <install folder>/exiftool/exiftool.exe (+ exiftool_files/)
macOS .app:       QuadM.app/Contents/Resources/exiftool/exiftool (+ lib/)
```

Don't hardcode a path. Use [src/exiftool.h](../../src/exiftool.h), which looks relative to the running app:

```cpp
#include "exiftool.h"

QJsonObject tags;
QString error;
if (ExifTool::readMetadata(filePath, tags, error)) {
    QString make = tags.value("EXIF:Make").toString();   // keys look like "Group:Tag"
} else {
    // show `error` to the user
}
```

To run ExifTool with other arguments, get the executable from `ExifTool::executablePath()` and pass the arguments to `QProcess` as a list, never as one shell string.

## Installer checklist

- Include the whole `exiftool` folder (the `exiftool_files` or `lib` folder too), placed as shown above.
- Keep the executable bit on `exiftool` for macOS and Linux.
- On Linux packages, declare `perl` as a dependency.
- Install into the program folder (`Program Files`, `/opt`, the `.app` bundle), not AppData or ProgramData.
