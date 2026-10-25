#!/usr/bin/env bash
set -e

REPO_DIR="$(cd "$(dirname "$0")/.." && pwd)"
BIN_SRC="$REPO_DIR/build/tqztr_editor"
ICON_SRC="$REPO_DIR/assets/tqztr-editor.svg"
DESKTOP_SRC="$REPO_DIR/assets/tqztr-editor.desktop"

if [ ! -f "$BIN_SRC" ]; then
  echo "Error: $BIN_SRC not found. Build first: ./scripts/build.sh"
  exit 1
fi

if [ ! -f "$ICON_SRC" ] || [ ! -f "$DESKTOP_SRC" ]; then
  echo "Error: missing assets in $REPO_DIR/assets/"
  exit 1
fi

echo "Installing binary to ~/.local/bin..."
mkdir -p "$HOME/.local/bin"
cp "$BIN_SRC" "$HOME/.local/bin/tqztr_editor"
chmod +x "$HOME/.local/bin/tqztr_editor"

echo "Installing icon..."
ICON_DIR="$HOME/.local/share/icons/hicolor/scalable/apps"
mkdir -p "$ICON_DIR"
cp "$ICON_SRC" "$ICON_DIR/tqztr-editor.svg"

echo "Installing .desktop file..."
DESKTOP_DIR="$HOME/.local/share/applications"
mkdir -p "$DESKTOP_DIR"
cp "$DESKTOP_SRC" "$DESKTOP_DIR/tqztr-editor.desktop"
chmod +x "$DESKTOP_DIR/tqztr-editor.desktop"

echo "Updating caches..."
gtk-update-icon-cache -f -t "$HOME/.local/share/icons/hicolor" 2>/dev/null || true
update-desktop-database "$HOME/.local/share/applications" 2>/dev/null || true

echo ""
echo "Done. Search for 'TQZTR' in your app launcher."
echo "If the icon doesn't appear, log out and back in."
