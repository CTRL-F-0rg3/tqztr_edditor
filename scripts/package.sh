#!/usr/bin/env bash
set -e
VERSION="${1:-v1.0}"
ARCHIVE_NAME="tqztr-editor-linux-${VERSION}.tar.gz"
DIST_DIR="dist"

mkdir -p "$DIST_DIR"
cd "$DIST_DIR"

# Struktura wewnątrz archiwum
mkdir -p "tqztr-editor/bin"
mkdir -p "tqztr-editor/plugins"
mkdir -p "tqztr-editor/src"

# Kopiujemy binarkę
cp ../build/tqztr_editor "tqztr-editor/bin/"
# Kopiujemy źródła (żeby ktoś mógł se zbudować)
cp -r ../src ../CMakeLists.txt "tqztr-editor/src/"
# Kopiujemy skompilowane wtyczki jeśli są
if [ -d "$HOME/.config/tqztr_editor/plugins" ]; then
  cp "$HOME/.config/tqztr_editor/plugins/"*.jar "tqztr-editor/plugins/" 2>/dev/null || true
fi

# Pakujemy
tar -czf "$ARCHIVE_NAME" tqztr-editor
rm -rf tqztr-editor

echo "=== Spakowano: dist/$ARCHIVE_NAME ==="
