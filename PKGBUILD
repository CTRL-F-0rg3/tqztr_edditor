# Maintainer: ctrl <ctrl@trangorge.org>
pkgname=tqztr-editor-git
pkgver=r11.bb5a6b5
pkgrel=1
pkgdesc="A lightweight, customizable code editor built with Qt6 and QScintilla"
arch=('x86_64')
url="https://github.com/CTRL-F-0rg3/tqztr_edditor"
license=('GPL3')
depends=('qt6-base' 'qscintilla-qt6' 'qtermwidget')
makedepends=('git' 'cmake' 'pkgconf' 'jdk-openjdk')
provides=('tqztr-editor')
conflicts=('tqztr-editor')
source=("$pkgname::git+https://github.com/CTRL-F-0rg3/tqztr_edditor.git")
sha256sums=('SKIP')

pkgver() {
    cd "$srcdir/$pkgname"
    printf "r%s.%s" "$(git rev-list --count HEAD)" "$(git rev-parse --short HEAD)"
}

build() {
    cd "$srcdir/$pkgname"
    cmake -B build -S . -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr
    cmake --build build -j$(nproc)
}

package() {
    cd "$srcdir/$pkgname"
    install -Dm755 "build/tqztr_editor" "$pkgdir/usr/bin/tqztr_editor"
    install -Dm644 "assets/tqztr-editor.svg" "$pkgdir/usr/share/icons/hicolor/scalable/apps/tqztr-editor.svg"
    install -Dm644 "assets/tqztr-editor.desktop" "$pkgdir/usr/share/applications/tqztr-editor.desktop"
}
