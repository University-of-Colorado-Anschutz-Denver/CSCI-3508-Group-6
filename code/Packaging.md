# Building Installers (Windows and Linux)

All scripts are in [packaging/](packaging). Each one builds a Release build, bundles Qt and ExifTool, and writes the installer to `code/dist/` (git-ignored). macOS has its own guide: [Packaging-macOS.md](Packaging-macOS.md).

| Platform | Command (from the `code` folder) | Output |
| --- | --- | --- |
| Windows | `.\packaging\build-windows-msi.ps1 -Version 1.0.0` | `dist\QuadM-1.0.0-windows-x64.msi` |
| Ubuntu/Debian | `bash packaging/build-deb.sh 1.0.0` | `dist/quadm_1.0.0_amd64.deb` |
| macOS | see the macOS guide | `dist/QuadM-1.0.0-macos-<arch>.dmg` |

Each installer must be built on its own platform. Use a different version number for every release.

## Windows (MSI)

Needs the Qt 6.12 MinGW setup from [CMake.md](CMake.md) and the ExifTool Windows files in `third_party\exiftool` (see its [README](third_party/exiftool/README.md)). The script installs the WiX tool (version 5.0.2) with `dotnet tool` the first time; the .NET SDK must be installed.

It installs to `C:\Program Files\QuadM\` (with `exiftool\` next to `QuadM.exe`) and adds a Start menu shortcut. The installer is a short wizard: Welcome, an Options page with a **Create a desktop shortcut** checkbox (checked by default), Install, then a **Finish** page. Silent installs can skip the shortcut with `msiexec /i QuadM.msi /qn DESKTOP_SHORTCUT=0`. Installing a newer version upgrades the old one. The package and wizard are defined in [packaging/QuadM.wxs](packaging/QuadM.wxs); the script adds the `WixToolset.UI.wixext` WiX extension on first run.

## Ubuntu/Debian (.deb)

Install the build tools once (also works in WSL):

```bash
sudo apt install build-essential cmake ninja-build git curl perl qt6-base-dev
```

The script downloads the Linux ExifTool itself (or uses `third_party/exiftool` if it already has the Linux version). Install the result with:

```bash
sudo apt install ./dist/quadm_1.0.0_amd64.deb
```

The app goes to `/opt/quadm/` (ExifTool in `/opt/quadm/exiftool/`), with a `quadm` command and an application menu entry. `apt` installs the `perl` and Qt dependencies automatically. Remove it with `sudo apt remove quadm`.
