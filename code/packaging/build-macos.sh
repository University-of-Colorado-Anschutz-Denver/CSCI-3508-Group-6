#!/usr/bin/env bash
# Builds a self-contained QuadM.app, then a .pkg installer (with a wizard) and a .dmg, on macOS.
# Usage (from the code folder):  bash packaging/build-macos.sh [version]
# Output: code/dist/QuadM-<version>-macos-<arch>.pkg and .dmg
# Needs: Xcode command line tools and `brew install cmake ninja qt` (see Packaging-macOS.md).
set -euo pipefail

cd "$(dirname "$0")/.."   # the code folder

VERSION="${1:-1.0.0}"
ARCH="$(uname -m)"        # arm64 (Apple Silicon) or x86_64 (Intel)
BUILD_DIR="build/release-pkg-macos"
APP="$BUILD_DIR/QuadM.app"
DMG="dist/QuadM-$VERSION-macos-$ARCH.dmg"
PKG="dist/QuadM-$VERSION-macos-$ARCH.pkg"

if [[ "$(uname -s)" != "Darwin" ]]; then
    echo "This script must be run on macOS." >&2
    exit 1
fi

QT_PREFIX="${QT_PREFIX:-$(brew --prefix qt 2>/dev/null || true)}"
if [[ -z "$QT_PREFIX" || ! -x "$QT_PREFIX/bin/macdeployqt" ]]; then
    echo "Qt not found. Run 'brew install qt', or set QT_PREFIX, e.g. QT_PREFIX=\$HOME/Qt/6.12.0/macos" >&2
    exit 1
fi

echo "==> Building QuadM $VERSION ($ARCH, Release)"
cmake -S . -B "$BUILD_DIR" -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="$QT_PREFIX"
cmake --build "$BUILD_DIR"

echo "==> Creating QuadM.app"
rm -rf "$APP"
mkdir -p "$APP/Contents/MacOS" "$APP/Contents/Resources"
cp "$BUILD_DIR/QuadM" "$APP/Contents/MacOS/QuadM"
bash packaging/fetch-exiftool-unix.sh "$APP/Contents/Resources/exiftool"   # the app looks here (see src/exiftool.cpp)

cat > "$APP/Contents/Info.plist" <<EOF
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>CFBundleName</key><string>QuadM</string>
    <key>CFBundleDisplayName</key><string>QuadM</string>
    <key>CFBundleIdentifier</key><string>edu.cuanschutz.quadm</string>
    <key>CFBundleExecutable</key><string>QuadM</string>
    <key>CFBundlePackageType</key><string>APPL</string>
    <key>CFBundleVersion</key><string>$VERSION</string>
    <key>CFBundleShortVersionString</key><string>$VERSION</string>
    <key>NSHighResolutionCapable</key><true/>
</dict>
</plist>
EOF

echo "==> Bundling Qt into the app (macdeployqt)"
"$QT_PREFIX/bin/macdeployqt" "$APP"

echo "==> Ad-hoc signing (Apple Silicon refuses to run unsigned code)"
codesign --force --deep --sign - "$APP"

echo "==> Creating the .dmg"
STAGING="$BUILD_DIR/dmg-staging"
rm -rf "$STAGING" "$DMG"
mkdir -p "$STAGING" dist
cp -R "$APP" "$STAGING/"
ln -s /Applications "$STAGING/Applications"
hdiutil create -volname "QuadM" -srcfolder "$STAGING" -ov -format UDZO "$DMG"

echo "==> Creating the .pkg installer (Welcome -> Install -> Finish wizard)"
PKG_ID="edu.cuanschutz.quadm.pkg"
PKG_ROOT="$BUILD_DIR/pkg-root"
PKG_DIR="$BUILD_DIR/pkg"
rm -rf "$PKG_ROOT" "$PKG_DIR" "$PKG"
mkdir -p "$PKG_ROOT" "$PKG_DIR"
cp -R "$APP" "$PKG_ROOT/"

# Non-relocatable: always install into /Applications instead of "updating" some other copy of QuadM.app
pkgbuild --analyze --root "$PKG_ROOT" "$PKG_DIR/components.plist" >/dev/null
/usr/libexec/PlistBuddy -c "Set :0:BundleIsRelocatable false" "$PKG_DIR/components.plist"
pkgbuild --root "$PKG_ROOT" --component-plist "$PKG_DIR/components.plist" \
    --install-location /Applications --identifier "$PKG_ID" --version "$VERSION" \
    "$PKG_DIR/QuadM-component.pkg"

sed "s/@VERSION@/$VERSION/g; s/@PKG_ID@/$PKG_ID/g" packaging/macos/distribution.xml > "$PKG_DIR/distribution.xml"
productbuild --distribution "$PKG_DIR/distribution.xml" --resources packaging/macos/resources \
    --package-path "$PKG_DIR" "$PKG"

echo
echo "Done:"
echo "  $PKG   (installer wizard)"
echo "  $DMG   (drag-and-drop disk image)"
shasum -a 256 "$PKG" "$DMG"
