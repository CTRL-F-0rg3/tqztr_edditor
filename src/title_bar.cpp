#include "title_bar.hpp"
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QMouseEvent>
#include <QWindow>
#include <QPainter>
#include <QPaintEvent>
#include <QSizePolicy>

// ===== macOS-style traffic light button =====
class TrafficLight : public QPushButton {
public:
    TrafficLight(const QColor& bg, const QColor& hoverBg,
                 const QString& symbol, QWidget* parent = nullptr)
    : QPushButton(parent), m_bg(bg), m_hoverBg(hoverBg), m_symbol(symbol)
    {
        setFixedSize(12, 12);
        setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        setCursor(Qt::PointingHandCursor);
        setFlat(true);
        setAttribute(Qt::WA_Hover);
        setFocusPolicy(Qt::NoFocus);

        // Zerujemy WSZYSTKO, co globalny QSS mógł narzucić
        // (padding, min-height — to one robiły z kółek elipsy)
        setStyleSheet(
            "QPushButton {"
            "  background: transparent;"
            "  border: none;"
            "  padding: 0px;"
            "  margin: 0px;"
            "  min-width: 12px;"
            "  max-width: 12px;"
            "  min-height: 12px;"
            "  max-height: 12px;"
            "}"
        );
    }

protected:
    void paintEvent(QPaintEvent*) override {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);

        bool hovered = underMouse();
        p.setBrush(hovered ? m_hoverBg : m_bg);
        p.setPen(Qt::NoPen);
        p.drawEllipse(QRectF(0.5, 0.5, width() - 1.0, height() - 1.0));

        if (hovered && !m_symbol.isEmpty()) {
            QFont f = font();
            f.setPointSizeF(6.5);
            f.setBold(true);
            p.setFont(f);
            p.setPen(QColor(0, 0, 0, 180));
            p.drawText(rect(), Qt::AlignCenter, m_symbol);
        }
    }

private:
    QColor m_bg, m_hoverBg;
    QString m_symbol;
};

TitleBar::TitleBar(QWidget* parent) : QWidget(parent) {
    setFixedHeight(32);
    setObjectName("titleBar");
    setAttribute(Qt::WA_StyledBackground, true);

    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(12, 0, 12, 0);
    layout->setSpacing(8);

    auto* closeBtn = new TrafficLight(
        QColor(0xff, 0x5f, 0x57), QColor(0xff, 0x7a, 0x72),
                                      QString::fromUtf8("\xC3\x97"), this);      // ×

    auto* minBtn = new TrafficLight(
        QColor(0xfe, 0xbc, 0x2e), QColor(0xff, 0xd0, 0x60),
                                    QString::fromUtf8("\xE2\x88\x92"), this);  // −

    auto* maxBtn = new TrafficLight(
        QColor(0x28, 0xc8, 0x40), QColor(0x4d, 0xe0, 0x60),
                                    QString::fromUtf8("\x2B"), this);          // +

    layout->addWidget(closeBtn);
    layout->addWidget(minBtn);
    layout->addWidget(maxBtn);
    layout->addStretch();

    m_title = new QLabel("TQZTR Editor", this);
    m_title->setAlignment(Qt::AlignCenter);
    m_title->setStyleSheet("QLabel { color: #8a7a95; font-weight: 500; background: transparent; }");
    layout->addWidget(m_title);

    layout->addStretch();
    auto* spacer = new QWidget(this);
    spacer->setFixedWidth(12 * 3 + 8 * 2);
    spacer->setStyleSheet("background: transparent;");
    layout->addWidget(spacer);

    connect(closeBtn, &QPushButton::clicked, this, [this]{ window()->close(); });
    connect(minBtn,   &QPushButton::clicked, this, [this]{ window()->showMinimized(); });
    connect(maxBtn,   &QPushButton::clicked, this, [this]{
        if (window()->isMaximized()) window()->showNormal();
        else window()->showMaximized();
    });
}

void TitleBar::setTitle(const QString& title) {
    m_title->setText(title);
}

void TitleBar::mousePressEvent(QMouseEvent* e) {
    if (e->button() == Qt::LeftButton && window()->windowHandle())
        window()->windowHandle()->startSystemMove();
}

void TitleBar::mouseDoubleClickEvent(QMouseEvent* e) {
    if (e->button() != Qt::LeftButton) return;
    if (window()->isMaximized()) window()->showNormal();
    else window()->showMaximized();
}
