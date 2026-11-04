#!/usr/bin/env bash
# Puts a Linux/macOS ExifTool (the `exiftool` script plus its `lib` folder) into the folder given as $1.
# Uses third_party/exiftool when it already holds the Unix version; otherwise downloads a pinned release.
# Used by build-deb.sh and build-macos.sh.
set -euo pipefail

DEST="$1"
EXIFTOOL_VERSION="${EXIFTOOL_VERSION:-13.60}"
CODE_DIR="$(cd "$(dirname "$0")/.." && pwd)"
LOCAL="$CODE_DIR/third_party/exiftool"

rm -rf "$DEST"
mkdir -p "$DEST"

if [[ -f "$LOCAL/exiftool" && -d "$LOCAL/lib" ]]; then
    echo "==> Using ExifTool from third_party/exiftool"
    cp -R "$LOCAL/." "$DEST/"
else
    echo "==> Downloading ExifTool $EXIFTOOL_VERSION"
    curl -fsSL "https://github.com/exiftool/exiftool/archive/refs/tags/$EXIFTOOL_VERSION.tar.gz" \
        | tar -xz -C "$DEST" --strip-components=1
fi

rm -f "$DEST/README.md" "$DEST/.gitkeep"
chmod +x "$DEST/exiftool"
"$DEST/exiftool" -ver >/dev/null   # fails the build here if ExifTool or Perl is broken
