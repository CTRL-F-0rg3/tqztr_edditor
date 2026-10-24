#include "terminal.hpp"
#include <QVBoxLayout>
#include <QDir>
#include <QFont>
#include <QKeyEvent>
#include <QDateTime>
#include <qtermwidget.h>

TerminalWidget::TerminalWidget(QWidget* parent) : QWidget(parent) {
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    m_term = new QTermWidget(0, this);
    m_term->setTerminalFont(QFont("JetBrains Mono", 11));
    m_term->setScrollBarPosition(QTermWidget::ScrollBarRight);
    m_term->setColorScheme("WhiteOnBlack");
    m_term->setTerminalSizeHint(false);
    m_term->setBlinkingCursor(true);
    m_term->installEventFilter(this);
    layout->addWidget(m_term);
}

TerminalWidget::~TerminalWidget() {
    stopShell();
}

void TerminalWidget::setWorkingDirectory(const QString& dir) {
    m_workDir = dir;
}

void TerminalWidget::startShell() {
    if (m_started) return;
    m_started = true;

    QString workDir = m_workDir;
    if (workDir.isEmpty() || !QDir(workDir).exists())
        workDir = QDir::homePath();
    if (workDir.isEmpty()) workDir = "/";

    m_term->setWorkingDirectory(workDir);

    // TERM — naprawia część problemów z klawiszami
    qputenv("TERM", "xterm-256color");

    QString shell = qEnvironmentVariable("SHELL");
    if (shell.isEmpty()) shell = "/bin/bash";

    m_term->setShellProgram(shell);
    m_term->startShellProgram();
    m_term->setFocus();
}

void TerminalWidget::stopShell() {
    if (!m_term || !m_started) return;
    m_started = false;
}

bool TerminalWidget::eventFilter(QObject* obj, QEvent* e) {
    if (obj == m_term && e->type() == QEvent::KeyPress) {
        auto* ke = static_cast<QKeyEvent*>(e);
        // Throttle backspace auto-repeat do ~12.5 Hz
        if (ke->key() == Qt::Key_Backspace && ke->isAutoRepeat()) {
            qint64 now = QDateTime::currentMSecsSinceEpoch();
            if (now - m_lastBackspaceMs < 80)
                return true;   // swallow excessive repeats
                m_lastBackspaceMs = now;
        }
    }
    return QWidget::eventFilter(obj, e);
}
