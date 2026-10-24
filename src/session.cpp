#include "session.hpp"
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

QString SessionManager::configPath() {
    QString dir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    QDir().mkpath(dir);
    return dir + "/session.json";
}

SessionState SessionManager::load() {
    SessionState s;
    QFile f(configPath());
    if (!f.open(QIODevice::ReadOnly)) return s;

    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(f.readAll(), &err);
    if (err.error != QJsonParseError::NoError) return s;

    QJsonObject o = doc.object();
    s.rootPath             = o.value("rootPath").toString();
    s.activeTab            = o.value("activeTab").toInt();
    s.geometry             = QByteArray::fromBase64(o.value("geometry").toString().toLatin1());
    s.windowState          = QByteArray::fromBase64(o.value("windowState").toString().toLatin1());
    s.splitterState        = QByteArray::fromBase64(o.value("splitterState").toString().toLatin1());
    s.contentSplitterState = QByteArray::fromBase64(o.value("contentSplitterState").toString().toLatin1());
    s.backgroundImagePath  = o.value("backgroundImagePath").toString();

    for (const auto& v : o.value("tabs").toArray()) {
        QJsonObject t = v.toObject();
        TabState ts;
        ts.path = t.value("path").toString();
        ts.line = t.value("line").toInt();
        ts.col  = t.value("col").toInt();
        if (!ts.path.isEmpty()) s.tabs.append(ts);
    }
    return s;
}

void SessionManager::save(const SessionState& s) {
    QJsonObject o;
    o["rootPath"]             = s.rootPath;
    o["activeTab"]            = s.activeTab;
    o["geometry"]             = QString::fromLatin1(s.geometry.toBase64());
    o["windowState"]          = QString::fromLatin1(s.windowState.toBase64());
    o["splitterState"]        = QString::fromLatin1(s.splitterState.toBase64());
    o["contentSplitterState"] = QString::fromLatin1(s.contentSplitterState.toBase64());
    o["backgroundImagePath"]  = s.backgroundImagePath;

    QJsonArray tabs;
    for (const auto& t : s.tabs) {
        QJsonObject to;
        to["path"] = t.path;
        to["line"] = t.line;
        to["col"]  = t.col;
        tabs.append(to);
    }
    o["tabs"] = tabs;

    QFile f(configPath());
    if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        qWarning() << "Cannot save session to" << configPath();
        return;
    }
    f.write(QJsonDocument(o).toJson(QJsonDocument::Indented));
}
