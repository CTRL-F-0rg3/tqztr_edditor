#pragma once
#include <QWidget>
#include <QString>

class QTermWidget;

class TerminalWidget : public QWidget {
    Q_OBJECT
public:
    explicit TerminalWidget(QWidget* parent = nullptr);
    ~TerminalWidget();

    void setWorkingDirectory(const QString& dir);
    void startShell();
    void stopShell();

protected:
    bool eventFilter(QObject* obj, QEvent* e) override;

private:
    QTermWidget* m_term = nullptr;
    QString      m_workDir;
    bool         m_started = false;
    qint64       m_lastBackspaceMs = 0;
};
