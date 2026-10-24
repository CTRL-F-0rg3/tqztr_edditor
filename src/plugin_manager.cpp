#include "plugin_manager.hpp"
#include "main_window.hpp"
#include "editor.hpp"
#include <QDir>
#include <QDebug>
#include <QMessageBox>
#include <QStandardPaths>
#include <QApplication>
// ====== NATYWNE API DLA WTYCZEK ======
// Wtyczki (Java/Kotlin) deklarują: private native void nativeShowMessage(String msg);
// i wołają to przez JNI. Tutaj implementujemy te funkcje w C++.

extern "C" JNIEXPORT void JNICALL
Java_TqztrPlugin_nativeShowMessage(JNIEnv* env, jobject, jstring jmsg) {
    const char* cstr = env->GetStringUTFChars(jmsg, nullptr);
    QString msg = QString::fromUtf8(cstr);
    env->ReleaseStringUTFChars(jmsg, cstr);

    // Znajdź MainWindow przez QApplication (uproszczenie — w produkcji trzymaj wskaźnik)
    for (QWidget* w : QApplication::topLevelWidgets()) {
        if (auto* mw = qobject_cast<MainWindow*>(w)) {
            QMessageBox::information(mw, "Plugin", msg);
            break;
        }
    }
}

extern "C" JNIEXPORT void JNICALL
Java_TqztrPlugin_nativeInsertText(JNIEnv* env, jobject, jstring jtext) {
    const char* cstr = env->GetStringUTFChars(jtext, nullptr);
    QString text = QString::fromUtf8(cstr);
    env->ReleaseStringUTFChars(jtext, cstr);

    for (QWidget* w : QApplication::topLevelWidgets()) {
        if (auto* mw = qobject_cast<MainWindow*>(w)) {
            if (auto* ed = mw->getCurrentEditor()) ed->insert(text);
            break;
        }
    }
}

// ====== MANAGER ======

PluginManager::PluginManager(MainWindow* window, QObject* parent)
    : QObject(parent), m_window(window) {}

PluginManager::~PluginManager() { shutdownJVM(); }

bool PluginManager::initJVM(const QString& pluginsDir) {
    // Zbierz wszystkie JAR-y z katalogu plugins/
    QStringList jars;
    QDir dir(pluginsDir);
    if (dir.exists()) {
        for (const QFileInfo& fi : dir.entryInfoList({"*.jar"}, QDir::Files))
            jars.append(fi.absoluteFilePath());
    }

    // classpath = wszystkie JAR-y + katalog z klasami pomocniczymi
    QString classpath = jars.join(":");

    // Opcje JVM:
    // -Xrs: nie przechwytuj sygnałów (żeby nie kolidowało z Qt)
    // -Djava.class.path: ścieżka do JAR-ów
    JavaVMOption options[2];
    options[0].optionString = const_cast<char*>("-Xrs");
    QByteArray cpOpt = "-Djava.class.path=" + classpath.toUtf8();
    options[1].optionString = cpOpt.data();

    JavaVMInitArgs args;
    args.version = JNI_VERSION_1_8;
    args.nOptions = 2;
    args.options = options;
    args.ignoreUnrecognized = JNI_FALSE;

    jint res = JNI_CreateJavaVM(&m_jvm, (void**)&m_env, &args);
    if (res != JNI_OK) {
        qWarning() << "JNI_CreateJavaVM failed, code =" << res;
        return false;
    }
    return true;
}

void PluginManager::loadPlugins() {
    if (!m_env) return;

    // Zakładamy, że każda wtyczka ma klasę główną z metodą statyczną:
    // public static void onLoad();
    // oraz public static String getName();
    //
    // W praktyce lepiej użyć interfejsu, ale JNI nie ma refleksji po interfejsach
    // w prosty sposób — dlatego szukamy klas po nazwie z manifestu lub konwencji.
    // Tutaj: każdy JAR musi mieć plik META-INF/tqztr-plugin.txt z nazwą klasy.

    QDir pluginDir(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + "/plugins");
    if (!pluginDir.exists()) return;

    for (const QFileInfo& fi : pluginDir.entryInfoList({"*.jar"}, QDir::Files)) {
        // W prawdziwym systemie: rozpakuj JAR, przeczytaj manifest, znajdź klasę.
        // Tutaj uproszczenie: zakładamy, że klasa nazywa się tak jak plik bez .jar
        QString className = fi.completeBaseName();
        QByteArray clsName = className.toUtf8();

        jclass cls = m_env->FindClass(clsName.constData());
        if (!cls) {
            m_env->ExceptionDescribe();
            m_env->ExceptionClear();
            qWarning() << "Cannot find class:" << className;
            continue;
        }

        jmethodID midLoad = m_env->GetStaticMethodID(cls, "onLoad", "()V");
        if (!midLoad) {
            qWarning() << "Class" << className << "has no static onLoad()";
            m_env->ExceptionClear();
            continue;
        }

        m_env->CallStaticVoidMethod(cls, midLoad);
        if (m_env->ExceptionCheck()) {
            m_env->ExceptionDescribe();
            m_env->ExceptionClear();
            qWarning() << "Exception in" << className << "::onLoad()";
            continue;
        }

        m_loaded.append(className);
    }
}

void PluginManager::shutdownJVM() {
    if (m_jvm) {
        m_jvm->DestroyJavaVM();
        m_jvm = nullptr;
        m_env = nullptr;
    }
}