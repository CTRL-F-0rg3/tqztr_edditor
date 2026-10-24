#pragma once
#include <QString>
#include <QList>
#include <QByteArray>

struct TabState {
    QString path;
    int line = 0;
    int col  = 0;
};

struct SessionState {
    QString rootPath;
    QList<TabState> tabs;
    int activeTab = 0;
    QByteArray geometry;
    QByteArray windowState;
    QByteArray splitterState;
    QByteArray contentSplitterState;   // <-- nowe (tabs | image)
    QString backgroundImagePath;       // <-- nowe (PNG)
};

class SessionManager {
public:
    static QString configPath();
    static SessionState load();
    static void save(const SessionState& s);
};
