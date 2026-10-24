#pragma once
#include <QObject>
#include <QStringList>
#include <jni.h>

class MainWindow;

class PluginManager : public QObject {
    Q_OBJECT
public:
    explicit PluginManager(MainWindow* window, QObject* parent = nullptr);
    ~PluginManager() override;

    bool initJVM(const QString& pluginsDir);
    void loadPlugins();
    void shutdownJVM();

    QStringList loadedPlugins() const { return m_loaded; }

private:
    MainWindow* m_window;
    JavaVM*     m_jvm  = nullptr;
    JNIEnv*     m_env  = nullptr;
    QStringList m_loaded;
};