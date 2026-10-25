#!/usr/bin/env bash
set -e

VERSION="${1:-v0.1.0}"
REPO_DIR="$(cd "$(dirname "$0")/.." && pwd)"
PKG_NAME="tqztr-editor-linux-${VERSION}"
PKG_DIR="${REPO_DIR}/dist/${PKG_NAME}"
ARCHIVE="${REPO_DIR}/dist/${PKG_NAME}.tar.gz"

if [ ! -f "${REPO_DIR}/build/tqztr_editor" ]; then
    echo "Error: build/tqztr_editor not found. Run ./scripts/build.sh first."
    exit 1
fi

if [ ! -f "${REPO_DIR}/assets/tqztr-editor.svg" ] || \
   [ ! -f "${REPO_DIR}/assets/tqztr-editor.desktop" ]; then
    echo "Error: missing assets in assets/"
    exit 1
fi

echo "=== Building package ${PKG_NAME} ==="

rm -rf "${PKG_DIR}"
mkdir -p "${PKG_DIR}/bin"
mkdir -p "${PKG_DIR}/share/applications"
mkdir -p "${PKG_DIR}/share/icons/hicolor/scalable/apps"
mkdir -p "${PKG_DIR}/src"
mkdir -p "${PKG_DIR}/plugins"
mkdir -p "${PKG_DIR}/scripts"

cp "${REPO_DIR}/build/tqztr_editor" "${PKG_DIR}/bin/"
chmod +x "${PKG_DIR}/bin/tqztr_editor"

cp "${REPO_DIR}/assets/tqztr-editor.desktop" "${PKG_DIR}/share/applications/"
cp "${REPO_DIR}/assets/tqztr-editor.svg"     "${PKG_DIR}/share/icons/hicolor/scalable/apps/"

cp -r "${REPO_DIR}/src/"* "${PKG_DIR}/src/"
cp "${REPO_DIR}/CMakeLists.txt" "${PKG_DIR}/src/"
cp "${REPO_DIR}/README.md" "${PKG_DIR}/" 2>/dev/null || true
cp "${REPO_DIR}/LICENSE"   "${PKG_DIR}/" 2>/dev/null || true

if [ -d "${HOME}/.config/tqztr_editor/plugins" ]; then
    cp "${HOME}/.config/tqztr_editor/plugins/"*.jar "${PKG_DIR}/plugins/" 2>/dev/null || true
fi

cp "${REPO_DIR}/scripts/build.sh"        "${PKG_DIR}/scripts/"
cp "${REPO_DIR}/scripts/build_plugin.sh" "${PKG_DIR}/scripts/" 2>/dev/null || true

cat > "${PKG_DIR}/install.sh" << 'INSTALL_EOF'
#!/usr/bin/env bash
set -e

PKG_DIR="$(cd "$(dirname "$0")" && pwd)"

echo "=== TQZTR Editor - Installer ==="
echo ""

echo "Checking dependencies..."
MISSING=()
command -v cmake >/dev/null 2>&1 || MISSING+=("cmake")
[ -f /usr/lib/libQt6Widgets.so* ] || ls /usr/lib/libQt6Widgets.so* >/dev/null 2>&1 || MISSING+=("qt6")
pkg-config --exists Qt6Widgets 2>/dev/null || MISSING+=("qt6-base-dev")
find /usr/lib /usr/local/lib -name "libqscintilla*" 2>/dev/null | grep -q . || MISSING+=("qscintilla")
find /usr/lib /usr/local/lib -name "libjvm.so" 2>/dev/null | grep -q . || MISSING+=("jdk")

if [ ${#MISSING[@]} -gt 0 ]; then
    echo "Warning: some dependencies might be missing:"
    for dep in "${MISSING[@]}"; do echo "  - $dep"; done
    echo ""
    echo "On Arch: sudo pacman -S qt6-base qscintilla-qt6 jre-openjdk"
    echo "On Debian: sudo apt install qt6-base-dev libqscintilla2-qt6-dev default-jdk"
    echo ""
    read -p "Continue anyway? [y/N] " -n 1 -r
    echo
    [[ $REPLY =~ ^[Yy]$ ]] || exit 1
fi

echo "Installing to ~/.local/..."
mkdir -p "$HOME/.local/bin"
mkdir -p "$HOME/.local/share/applications"
mkdir -p "$HOME/.local/share/icons/hicolor/scalable/apps"
mkdir -p "$HOME/.config/tqztr_editor/plugins"

cp "$PKG_DIR/bin/tqztr_editor" "$HOME/.local/bin/"
chmod +x "$HOME/.local/bin/tqztr_editor"

cp "$PKG_DIR/share/applications/tqztr-editor.desktop" "$HOME/.local/share/applications/"
chmod +x "$HOME/.local/share/applications/tqztr-editor.desktop"

cp "$PKG_DIR/share/icons/hicolor/scalable/apps/tqztr-editor.svg" \
   "$HOME/.local/share/icons/hicolor/scalable/apps/"

if [ -d "$PKG_DIR/plugins" ] && [ "$(ls -A "$PKG_DIR/plugins" 2>/dev/null)" ]; then
    cp "$PKG_DIR/plugins/"*.jar "$HOME/.config/tqztr_editor/plugins/" 2>/dev/null || true
fi

echo "Updating caches..."
gtk-update-icon-cache -f -t "$HOME/.local/share/icons/hicolor" 2>/dev/null || true
update-desktop-database "$HOME/.local/share/applications" 2>/dev/null || true

if ! echo "$PATH" | tr ':' '\n' | grep -q "$HOME/.local/bin"; then
    echo ""
    echo "WARNING: ~/.local/bin is not in your PATH."
    echo "Add this to your ~/.bashrc or ~/.zshrc:"
    echo "  export PATH=\"\$HOME/.local/bin:\$PATH\""
    echo ""
fi

echo ""
echo "=== Installation complete ==="
echo "Run: tqztr_editor"
echo "Or search 'TQZTR' in your app launcher."
INSTALL_EOF

chmod +x "${PKG_DIR}/install.sh"

cat > "${PKG_DIR}/uninstall.sh" << 'UNINSTALL_EOF'
#!/usr/bin/env bash
set -e
echo "Uninstalling TQZTR Editor..."
rm -f "$HOME/.local/bin/tqztr_editor"
rm -f "$HOME/.local/share/applications/tqztr-editor.desktop"
rm -f "$HOME/.local/share/icons/hicolor/scalable/apps/tqztr-editor.svg"
gtk-update-icon-cache -f -t "$HOME/.local/share/icons/hicolor" 2>/dev/null || true
update-desktop-database "$HOME/.local/share/applications" 2>/dev/null || true
echo "Done. Config (~/.config/tqztr_editor) left untouched."
echo "Remove it manually if you want: rm -rf ~/.config/tqztr_editor"
UNINSTALL_EOF

chmod +x "${PKG_DIR}/uninstall.sh"

cd "${REPO_DIR}/dist"
tar -czf "${PKG_NAME}.tar.gz" "${PKG_NAME}"
cd "${REPO_DIR}"

echo ""
echo "=== Package ready ==="
echo "  ${ARCHIVE}"
ls -lh "${ARCHIVE}"
echo ""
echo "Contents:"
tar -tzf "${ARCHIVE}" | head -20
