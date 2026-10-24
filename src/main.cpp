#include <QApplication>
#include <QStyleFactory>
#include <QPalette>
#include "main_window.hpp"

static void applyOLEDPalette(QApplication& app) {
    app.setStyle(QStyleFactory::create("Fusion"));

    QPalette p;
    p.setColor(QPalette::Window,          QColor(0x05, 0x03, 0x08));
    p.setColor(QPalette::WindowText,      QColor(0xd8, 0xcc, 0xe0));
    p.setColor(QPalette::Base,            QColor(0x00, 0x00, 0x00));
    p.setColor(QPalette::AlternateBase,   QColor(0x0a, 0x06, 0x0e));
    p.setColor(QPalette::ToolTipBase,     QColor(0x12, 0x0a, 0x14));
    p.setColor(QPalette::ToolTipText,     QColor(0xd8, 0xcc, 0xe0));
    p.setColor(QPalette::Text,            QColor(0xd8, 0xcc, 0xe0));
    p.setColor(QPalette::Button,          QColor(0x0a, 0x06, 0x0e));
    p.setColor(QPalette::ButtonText,      QColor(0xd8, 0xcc, 0xe0));
    p.setColor(QPalette::BrightText,      QColor(0xff, 0x2d, 0x95));
    p.setColor(QPalette::Highlight,       QColor(0xff, 0x2d, 0x95));   // magenta
    p.setColor(QPalette::HighlightedText, QColor(0xff, 0xff, 0xff));
    p.setColor(QPalette::Link,            QColor(0xff, 0x5e, 0xb0));

    p.setColor(QPalette::Disabled, QPalette::Text,       QColor(0x4a, 0x3e, 0x50));
    p.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(0x4a, 0x3e, 0x50));
    p.setColor(QPalette::Disabled, QPalette::WindowText, QColor(0x4a, 0x3e, 0x50));

    app.setPalette(p);
}

static const char* QSS = R"QSS(
/* ================= Base ================= */
QMainWindow, QWidget {
    background: #05030a;
    color: #d8cce0;
    font-family: "Inter", "Noto Sans", "DejaVu Sans", sans-serif;
    font-size: 10pt;
}

/* ================= TitleBar ================= */
QWidget#titleBar {
    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                                stop:0 #0d0610, stop:1 #05030a);
    border-bottom: 1px solid #ff2d95;
}

/* ================= MenuBar ================= */
QMenuBar {
    background: #0a0610;
    border-bottom: 1px solid #000000;
    padding: 2px 4px;
}
QMenuBar::item {
    padding: 5px 12px;
    border-radius: 6px;
    background: transparent;
    color: #a896b0;
}
QMenuBar::item:selected { background: #1a0d18; color: #ff2d95; }
QMenuBar::item:pressed  { background: #ff2d95; color: #ffffff; }

/* ================= Menu ================= */
QMenu {
    background: #0d0610;
    border: 1px solid #1a0d18;
    border-radius: 8px;
    padding: 4px;
}
QMenu::item {
    padding: 6px 24px 6px 12px;
    border-radius: 4px;
    color: #d8cce0;
}
QMenu::item:selected { background: #ff2d95; color: #ffffff; }
QMenu::separator { height: 1px; background: #1a0d18; margin: 4px 8px; }

/* ================= Tabs ================= */
QTabWidget::pane {
    background: #0a0610;
    border: none;
    border-top-left-radius: 8px;
    border-top-right-radius: 8px;
    padding: 4px;
}
QTabBar { background: transparent; qproperty-drawBase: 0; }
QTabBar::tab {
    background: transparent;
    padding: 6px 14px;
    margin-right: 2px;
    margin-top: 4px;
    border-top-left-radius: 6px;
    border-top-right-radius: 6px;
    color: #6a5e72;
    min-width: 80px;
}
QTabBar::tab:selected {
    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                                stop:0 #1a0d18, stop:1 #0a0610);
    color: #ff2d95;
    border-bottom: 2px solid #ff2d95;
}
QTabBar::tab:hover:!selected { background: #150a14; color: #d8cce0; }
QTabBar::close-button { subcontrol-position: right; }

/* ================= Tree ================= */
QTreeView {
    background: #05030a;
    border: none;
    outline: none;
    show-decoration-selected: 1;
}
QTreeView::item {
    padding: 3px 4px;
    border-radius: 4px;
    margin: 0 4px;
    color: #b8a8c0;
}
QTreeView::item:selected { background: #ff2d95; color: #ffffff; }
QTreeView::item:hover:!selected { background: #150a14; }

/* ================= Splitter ================= */
QSplitter::handle { background: #05030a; width: 3px; }
QSplitter::handle:hover { background: #ff2d95; }

/* ================= StatusBar ================= */
QStatusBar {
    background: #0a0610;
    color: #6a5e72;
    border-top: 1px solid #000000;
    padding: 2px 8px;
}
QStatusBar::item { border: none; }

/* ================= Dock ================= */
QDockWidget { color: #6a5e72; }
QDockWidget::title {
    background: #0a0610;
    padding: 6px 10px;
    border-top-left-radius: 8px;
    border-top-right-radius: 8px;
}

/* ================= Buttons ================= */
QPushButton {
    background: #0a0610;
    border: 1px solid #1a0d18;
    border-radius: 6px;
    padding: 4px 12px;
    color: #d8cce0;
    min-height: 20px;
}
QPushButton:hover { background: #150a14; border-color: #ff2d95; color: #ff2d95; }
QPushButton:pressed { background: #ff2d95; border-color: #ff2d95; color: #ffffff; }
QPushButton:disabled { color: #4a3e50; border-color: #0a0610; }

/* ================= LineEdit ================= */
QLineEdit {
    background: #000000;
    border: 1px solid #1a0d18;
    border-radius: 6px;
    padding: 4px 8px;
    color: #d8cce0;
    selection-background-color: #ff2d95;
    selection-color: #ffffff;
}
QLineEdit:focus { border-color: #ff2d95; }

/* ================= ScrollBars ================= */
QScrollBar:vertical { background: transparent; width: 12px; margin: 0; }
QScrollBar::handle:vertical {
    background: #1a0d18; border-radius: 6px;
    min-height: 30px; margin: 2px;
}
QScrollBar::handle:vertical:hover { background: #ff2d95; }
QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0; }
QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical { background: transparent; }

QScrollBar:horizontal { background: transparent; height: 12px; margin: 0; }
QScrollBar::handle:horizontal {
    background: #1a0d18; border-radius: 6px;
    min-width: 30px; margin: 2px;
}
QScrollBar::handle:horizontal:hover { background: #ff2d95; }
QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal { width: 0; }
QScrollBar::add-page:horizontal, QScrollBar::sub-page:horizontal { background: transparent; }

/* ================= Tooltips ================= */
QToolTip {
    background: #0d0610;
    color: #d8cce0;
    border: 1px solid #ff2d95;
    border-radius: 6px;
    padding: 4px 8px;
}
)QSS";

int main(int argc, char** argv) {
    QApplication app(argc, argv);
    app.setApplicationName("TQZTR Editor");
    app.setOrganizationName("Trangorge");
    applyOLEDPalette(app);
    app.setStyleSheet(QSS);

    MainWindow w;
    w.show();
    return app.exec();
}
