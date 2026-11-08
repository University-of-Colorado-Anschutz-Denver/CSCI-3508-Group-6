# Building the macOS Installer (.pkg and .dmg)

The build script produces two files in `dist/`:

- **`QuadM-<version>-macos-<arch>.pkg`**: a click-through installer wizard (Welcome → Install → Finish) that puts `QuadM.app` in `/Applications`. **This is the one to give to users.**
- **`QuadM-<version>-macos-<arch>.dmg`**: a disk image where users drag `QuadM` into Applications. A backup option.

Qt and ExifTool are bundled inside the app, so nothing else needs to be installed on the user's Mac.

**It can only be built on a Mac**, and it only runs on the same CPU type it was built on:

| Built on                    | Runs on                                        |
| --------------------------- | ---------------------------------------------- |
| Apple Silicon (M1/M2/M3/M4) | Apple Silicon Macs                             |
| Intel                       | Intel Macs (and Apple Silicon through Rosetta) |

## 1. One-time setup

1. Do steps 1 and 2 of [CMake-macOS.md](CMake-macOS.md): Xcode command line tools, Homebrew, then `brew install cmake ninja qt`.
2. Nothing to do for ExifTool. The build script downloads the correct macOS version (13.60) itself. If you already put the macOS ExifTool in `third_party/exiftool/` (`exiftool` and `lib/`), it uses that instead.

## 2. Build the installer

Open a terminal in the `code` folder and run:

Replace `1.0.0` with the version number (default `1.0.0`). It needs internet (TagLib and ExifTool are downloaded). When it finishes it prints the file location and a SHA-256 checksum:

```bash
bash packaging/build-macos.sh <version>
```



```text
code/dist/QuadM-1.0.0-macos-arm64.pkg     (x86_64 instead of arm64 on an Intel Mac)
code/dist/QuadM-1.0.0-macos-arm64.dmg
```

If CMake can't find Qt (you used the Qt online installer rather than Homebrew), set the path first:

```bash
QT_PREFIX=$HOME/Qt/6.12.0/macos bash packaging/build-macos.sh 1.0.0
```

## 3. What the script does

1. Builds a **Release** QuadM in `build/release-pkg-macos/` (separate from your normal `build/` folder).
2. Creates `QuadM.app` by hand: the executable in `Contents/MacOS/`, an `Info.plist`, and ExifTool in `Contents/Resources/exiftool/`. That is where [src/exiftool.cpp](src/exiftool.cpp) looks for it when running as an app.
3. Runs `macdeployqt` to copy the Qt frameworks and plugins into the app.
4. Signs it ad-hoc with `codesign --sign -`. Apple Silicon will not run unsigned code.
5. Packs the app and an `Applications` shortcut into a compressed `.dmg` with `hdiutil`.
6. Builds the `.pkg`: `pkgbuild` wraps the app (installs to `/Applications`), then `productbuild` adds the wizard using [packaging/macos/distribution.xml](packaging/macos/distribution.xml) and the welcome/finish text in [packaging/macos/resources/](packaging/macos/resources). Edit those two `.html` files to change the wizard text.

## 4. Test it before you share it

1. Double-click the `.pkg` and click through the wizard (Welcome → Install, enter your Mac password → Close). Then launch QuadM from Applications or Spotlight. Also try the `.dmg` once (drag `QuadM` to Applications).
2. Open a file with metadata (JPG, PNG, MP3). If metadata appears, ExifTool was found. If you see "ExifTool was not found", right-click the app → Show Package Contents and check that `Contents/Resources/exiftool/exiftool` exists.
3. If anything looks wrong, run `/Applications/QuadM.app/Contents/MacOS/QuadM` from a terminal and read the output.
4. Ideally also try it on a Mac that has no Qt or Homebrew.

## 5. "Can't be opened" warnings

Nothing is notarized (that needs a paid Apple Developer account), so Gatekeeper warns on first use of a downloaded file:

- **The `.pkg`**: if double-clicking says it "can't be opened because Apple cannot check it", Control-click the `.pkg` → **Open** → **Open**. On newer macOS, go to System Settings → Privacy & Security, scroll down, and click **Open Anyway**.
- **The app**: an app installed by the `.pkg` normally opens without a warning. If you get one (for example after using the `.dmg`), Control-click `QuadM` in Applications → **Open** → **Open**, or run `xattr -dr com.apple.quarantine /Applications/QuadM.app`.

## 6. Publishing

Attach the `.pkg` (and the `.dmg` if you want it) with their checksums to the GitHub Release for the version tag, next to the Windows `.msi` and Linux `.deb`. The download page ([website/js/release-data.js](website/js/release-data.js)) only looks for a Windows `.exe` and Linux `.AppImage` today, so it needs updating before it will list these files.

## Troubleshooting

- **`Qt not found`**: run `brew install qt`, or set `QT_PREFIX` as above.
- **Build fails after upgrading Qt**: delete `build/release-pkg-macos` and run again.
- **`curl` download of ExifTool fails**: check your connection, or put the macOS ExifTool into `third_party/exiftool/` as described in its [README](third_party/exiftool/README.md).
- **Remove QuadM**: delete `/Applications/QuadM.app`. A `.pkg` has no uninstaller; this is normal on macOS.
