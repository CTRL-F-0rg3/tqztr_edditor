#pragma once
#include <QString>
#include <QStringList>
#include <QObject>
#include <Qsci/qscilexer.h>
#include <Qsci/qscilexercpp.h>

struct LanguageDef {
    QStringList keywords;   // set 1
    QStringList types;      // set 2
};

// Dziedziczy po QsciLexerCPP, ale NIE ma Q_OBJECT — nie potrzebuje MOC,
// bo nie definiuje własnych sygnałów/slotów. To eliminuje problemy z moc_*.
class KeywordLexer : public QsciLexerCPP {
public:
    explicit KeywordLexer(const LanguageDef& def, QObject* parent = nullptr);
    const char* keywords(int set) const override;

private:
    QByteArray m_kw;
    QByteArray m_types;
};

QsciLexer* makeLexerForExtension(const QString& ext, QObject* parent);
void applyOLEDThemeToLexer(QsciLexer* lexer);
