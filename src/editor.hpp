#pragma once
#include <Qsci/qsciscintilla.h>
#include <QString>
#include <QPixmap>

class EditorWidget : public QsciScintilla {
    Q_OBJECT
public:
    explicit EditorWidget(QWidget* parent = nullptr);
    ~EditorWidget() override;

    bool loadFile(const QString& path);
    bool saveFile();
    QString filePath() const { return m_path; }
    QString displayName() const;

    void setBackgroundPixmap(const QPixmap& pm);
    void clearBackgroundPixmap();

signals:
    void cursorMoved(int line, int col);
    void modificationChanged(bool modified);

protected:
    void resizeEvent(QResizeEvent* e) override;
    void paintEvent(QPaintEvent* e) override;   // <-- nowe

private:
    void applyOLEDTheme();
    void applyLexer(const QString& ext);
    void setupMargins();
    void setupBehavior();

    QString m_path;
    QPixmap m_bgPixmap;
};
