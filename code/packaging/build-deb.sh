#!/usr/bin/env bash
# Builds a Release QuadM and packages it, with ExifTool, into a .deb for Ubuntu/Debian (amd64).
# Usage (from the code folder, on Ubuntu/Debian or WSL):  ./packaging/build-deb.sh [version]
# Output: code/dist/quadm_<version>_<arch>.deb
# Needs: sudo apt install build-essential cmake ninja-build git curl perl qt6-base-dev
set -euo pipefail

cd "$(dirname "$0")/.."   # the code folder

VERSION="${1:-1.0.0}"
ARCH="$(dpkg --print-architecture)"
BUILD_DIR="build/release-pkg-linux"
PKG="$(mktemp -d)"   # on a Linux filesystem so file permissions are correct (not /mnt/c under WSL)
trap 'rm -rf "$PKG"' EXIT
DEB="dist/quadm_${VERSION}_${ARCH}.deb"

echo "==> Building QuadM $VERSION (Release)"
cmake -S . -B "$BUILD_DIR" -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build "$BUILD_DIR"

echo "==> Laying out the package"
rm -rf "$PKG"
mkdir -p "$PKG/opt/quadm" "$PKG/usr/bin" "$PKG/usr/share/applications" "$PKG/DEBIAN"
install -m 755 "$BUILD_DIR/QuadM" "$PKG/opt/quadm/QuadM"
strip "$PKG/opt/quadm/QuadM"
bash packaging/fetch-exiftool-unix.sh "$PKG/opt/quadm/exiftool"   # the app looks in <install folder>/exiftool/
ln -s /opt/quadm/QuadM "$PKG/usr/bin/quadm"

cat > "$PKG/usr/share/applications/quadm.desktop" <<'EOF'
[Desktop Entry]
Type=Application
Name=QuadM
Comment=View and edit file metadata
Exec=/opt/quadm/QuadM
Terminal=false
Categories=Utility;
EOF

cat > "$PKG/DEBIAN/control" <<EOF
Package: quadm
Version: $VERSION
Section: utils
Priority: optional
Architecture: $ARCH
Depends: perl, libqt6widgets6
Maintainer: CSCI-3508 Group 6 <noreply@example.com>
Description: QuadM metadata viewer and editor
 Views and edits file metadata. Includes a bundled copy of ExifTool.
EOF

echo "==> Creating the .deb"
mkdir -p dist
rm -f "$DEB"
dpkg-deb --root-owner-group --build "$PKG" "$DEB"

echo
echo "Done: $DEB"
sha256sum "$DEB"
echo "Install with: sudo apt install ./$DEB"
