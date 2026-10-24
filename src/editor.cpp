#include "editor.hpp"
#include "languages.hpp"
#include <QFile>
#include <QFileInfo>
#include <QFontDatabase>
#include <QPainter>
#include <QResizeEvent>
#include <QRegion>

EditorWidget::EditorWidget(QWidget* parent) : QsciScintilla(parent) {
    applyOLEDTheme();
    setupMargins();
    setupBehavior();

    connect(this, &QsciScintilla::cursorPositionChanged, this,
            [this](int line, int col) { emit cursorMoved(line + 1, col + 1); });
    connect(this, &QsciScintilla::modificationChanged, this,
            [this](bool m) { emit modificationChanged(m); });
}

EditorWidget::~EditorWidget() = default;

void EditorWidget::applyOLEDTheme() {
    setColor(QColor(0xd8, 0xcc, 0xe0));
    setPaper(QColor(0x00, 0x00, 0x00));

    QFont f("JetBrains Mono", 11);
    auto families = QFontDatabase::families();
    if (!families.contains("JetBrains Mono")) {
        f = QFont("Fira Code", 11);
        if (!families.contains("Fira Code"))
            f = QFont("DejaVu Sans Mono", 11);
    }
    f.setStyleHint(QFont::Monospace);
    setFont(f);

    // Magenta caret + selection
    setCaretForegroundColor(QColor(0xff, 0x2d, 0x95));
    setCaretLineVisible(true);
    setCaretLineBackgroundColor(QColor(0x0d, 0x02, 0x0a));
    setCaretWidth(2);

    setSelectionBackgroundColor(QColor(0x33, 0x0a, 0x24));
    setSelectionForegroundColor(QColor(0xff, 0xff, 0xff));

    setIndentationGuidesBackgroundColor(QColor(0x00, 0x00, 0x00));
    setIndentationGuidesForegroundColor(QColor(0x1a, 0x0d, 0x18));
}

void EditorWidget::setupMargins() {
    QFont mf = font();
    mf.setPointSize(10);

    setMarginType(0, QsciScintilla::NumberMargin);
    setMarginWidth(0, "000000");
    setMarginLineNumbers(0, true);
    setMarginsBackgroundColor(QColor(0x00, 0x00, 0x00));
    setMarginsForegroundColor(QColor(0x45, 0x47, 0x5a));
    setMarginsFont(mf);

    setMarginType(1, QsciScintilla::SymbolMargin);
    setMarginWidth(1, 14);
    setFolding(QsciScintilla::BoxedTreeFoldStyle, 1);
    setFoldMarginColors(QColor(0x00, 0x00, 0x00), QColor(0x00, 0x00, 0x00));
}

void EditorWidget::setupBehavior() {
    setUtf8(true);
    setEolMode(QsciScintilla::EolUnix);
    setTabWidth(4);
    setIndentationsUseTabs(false);
    setTabIndents(true);
    setBackspaceUnindents(true);
    setAutoIndent(true);
    setBraceMatching(QsciScintilla::SloppyBraceMatch);
    setWrapMode(QsciScintilla::WrapNone);
    setEdgeMode(QsciScintilla::EdgeNone);
    setAutoCompletionSource(QsciScintilla::AcsAll);
    setAutoCompletionThreshold(2);
    setAutoCompletionCaseSensitivity(false);
}

void EditorWidget::applyLexer(const QString& ext) {
    auto* lexer = makeLexerForExtension(ext, this);
    if (lexer) {
        applyOLEDThemeToLexer(lexer);
        setLexer(lexer);
    } else {
        setLexer(nullptr);
    }
}

bool EditorWidget::loadFile(const QString& path) {
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly)) return false;
    QByteArray data = f.readAll();
    f.close();

    setText(QString::fromUtf8(data));
    m_path = path;

    if (data.size() > 2 * 1024 * 1024)
        setAutoCompletionSource(QsciScintilla::AcsNone);

    QString ext = QFileInfo(path).suffix().toLower();
    if (ext.isEmpty()) {
        QString base = QFileInfo(path).fileName();
        if (base == "Makefile" || base == "makefile") ext = "makefile";
        else if (base == "CMakeLists.txt")            ext = "cmake";
        else if (base == "Dockerfile")                ext = "dockerfile";
    }
    applyLexer(ext);

    setModified(false);
    setCursorPosition(0, 0);
    ensureLineVisible(0);
    return true;
}

bool EditorWidget::saveFile() {
    if (m_path.isEmpty()) return false;
    QFile f(m_path);
    if (!f.open(QIODevice::WriteOnly)) return false;
    f.write(text().toUtf8());
    f.close();
    setModified(false);
    return true;
}

QString EditorWidget::displayName() const {
    if (m_path.isEmpty()) return "Untitled";
    return QFileInfo(m_path).fileName();
}

void EditorWidget::setBackgroundPixmap(const QPixmap& pm) {
    m_bgPixmap = pm;
    viewport()->update();
}

void EditorWidget::clearBackgroundPixmap() {
    m_bgPixmap = QPixmap();
    viewport()->update();
}

void EditorWidget::paintEvent(QPaintEvent* e) {
    // 1. Scintilla maluje swoje (czarne tło + tekst + marginesy)
    QsciScintilla::paintEvent(e);

    // 2. Nakładamy obrazek na viewport przy niskim opacity.
    //    Tekst pozostaje nietknięty (bo tekst już jest pod spodem),
    //    obrazek widoczny jako delikatna mgiełka w tle edytora.
    if (m_bgPixmap.isNull()) return;

    QPainter p(viewport());
    p.setRenderHint(QPainter::SmoothPixmapTransform, true);
    p.setOpacity(0.14);   // 14% — widoczne, ale nie przeszkadza w czytaniu

    QPixmap scaled = m_bgPixmap.scaled(
        viewport()->size(),
                                       Qt::KeepAspectRatioByExpanding,
                                       Qt::SmoothTransformation);

    int x = (viewport()->width()  - scaled.width())  / 2;
    int y = (viewport()->height() - scaled.height()) / 2;
    p.drawPixmap(x, y, scaled);
}

void EditorWidget::resizeEvent(QResizeEvent* e) {
    QsciScintilla::resizeEvent(e);

    const int R = 10;   // <-- większy promień, żeby było widać
    const int W = width();
    const int H = height();
    if (W < 2 * R || H < 2 * R) return;

    QRegion region(0, 0, W, H);
    region -= QRegion(0,    0,    R, R);
    region -= QRegion(W-R,  0,    R, R);
    region -= QRegion(0,    H-R,  R, R);
    region -= QRegion(W-R,  H-R,  R, R);

    QRegion tlEll(0,   0,   2*R, 2*R, QRegion::Ellipse);
    QRegion trEll(W-2*R, 0,   2*R, 2*R, QRegion::Ellipse);
    QRegion blEll(0,   H-2*R, 2*R, 2*R, QRegion::Ellipse);
    QRegion brEll(W-2*R, H-2*R, 2*R, 2*R, QRegion::Ellipse);

    region += tlEll.intersected(QRegion(0,   0,   R, R));
    region += trEll.intersected(QRegion(W-R, 0,   R, R));
    region += blEll.intersected(QRegion(0,   H-R, R, R));
    region += brEll.intersected(QRegion(W-R, H-R, R, R));

    setMask(region);
}
