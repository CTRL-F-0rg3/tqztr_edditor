#include "plugin_manager.hpp"
#include "main_window.hpp"
#include "editor.hpp"
#include <QApplication>
#include <QDir>
#include <QDebug>
#include <QMessageBox>
#include <QStandardPaths>
#include <QMenuBar>
#include <QMenu>

// Struktura do przekazywania akcji menu z Javy do C++
struct PluginMenuAction { QString menu; QString label; QString handlerClass; };
static QList<PluginMenuAction> g_pendingActions;

// ====== NATYWNE METODY JNI (widziane przez Kotlina) ======

extern "C" JNIEXPORT void JNICALL
Java_TqztrNative_showMessage(JNIEnv* env, jclass, jstring jmsg) {
    const char* cstr = env->GetStringUTFChars(jmsg, nullptr);
    QString msg = QString::fromUtf8(cstr);
    env->ReleaseStringUTFChars(jmsg, cstr);
    for (QWidget* w : QApplication::topLevelWidgets()) {
        if (auto* mw = qobject_cast<MainWindow*>(w)) {
            QMessageBox::information(mw, "Wtyczka", msg);
            break;
        }
    }
}

extern "C" JNIEXPORT jstring JNICALL
Java_TqztrNative_currentFilePath(JNIEnv* env, jclass) {
    for (QWidget* w : QApplication::topLevelWidgets()) {
        if (auto* mw = qobject_cast<MainWindow*>(w)) {
            if (auto* ed = mw->getCurrentEditor())
                return env->NewStringUTF(ed->filePath().toUtf8().constData());
        }
    }
    return env->NewStringUTF("");
}

extern "C" JNIEXPORT void JNICALL
Java_TqztrNative_registerAction(JNIEnv* env, jclass, jstring jmenu, jstring jlabel, jstring jclass_) {
    const char* cmenu  = env->GetStringUTFChars(jmenu, nullptr);
    const char* clabel = env->GetStringUTFChars(jlabel, nullptr);
    const char* ccls   = env->GetStringUTFChars(jclass_, nullptr);

    g_pendingActions.append({
        QString::fromUtf8(cmenu),
                            QString::fromUtf8(clabel),
                            QString::fromUtf8(ccls)
    });

    env->ReleaseStringUTFChars(jmenu, cmenu);
    env->ReleaseStringUTFChars(jlabel, clabel);
    env->ReleaseStringUTFChars(jclass_, ccls);
}

extern "C" JNIEXPORT void JNICALL
Java_TqztrNative_addDiagnostic(JNIEnv* env, jclass, jint line, jint col, jint sev, jstring jmsg) {
    const char* cstr = env->GetStringUTFChars(jmsg, nullptr);
    QString msg = QString::fromUtf8(cstr);
    env->ReleaseStringUTFChars(jmsg, cstr);

    for (QWidget* w : QApplication::topLevelWidgets()) {
        if (auto* mw = qobject_cast<MainWindow*>(w)) {
            if (auto* ed = mw->getCurrentEditor()) {
                int marker = (sev == 2) ? 20 : (sev == 1) ? 21 : 22;
                ed->markerAdd(line, marker);
                // Tutaj można by dodać tooltip z msg, ale na razie marker wystarczy
            }
            break;
        }
    }
}

extern "C" JNIEXPORT void JNICALL
Java_TqztrNative_clearDiagnostics(JNIEnv*, jclass) {
    for (QWidget* w : QApplication::topLevelWidgets()) {
        if (auto* mw = qobject_cast<MainWindow*>(w)) {
            if (auto* ed = mw->getCurrentEditor()) {
                ed->markerDeleteAll(20); // error
                ed->markerDeleteAll(21); // warning
                ed->markerDeleteAll(22); // note
            }
            break;
        }
    }
}

// ====== MANAGER ======

PluginManager::PluginManager(MainWindow* window, QObject* parent)
: QObject(parent), m_window(window) {}

PluginManager::~PluginManager() { shutdownJVM(); }

bool PluginManager::initJVM(const QString& pluginsDir) {
    QStringList jars;
    QDir dir(pluginsDir);
    if (dir.exists()) {
        for (const QFileInfo& fi : dir.entryInfoList({"*.jar"}, QDir::Files))
            jars.append(fi.absoluteFilePath());
    }
    QString classpath = jars.join(":");
    QByteArray cpOpt = "-Djava.class.path=" + classpath.toUtf8();

    JavaVMOption options[2];
    options[0].optionString = const_cast<char*>("-Xrs");
    options[1].optionString = cpOpt.data();

    JavaVMInitArgs args;
    args.version = JNI_VERSION_1_8;
    args.nOptions = 2;
    args.options = options;
    args.ignoreUnrecognized = JNI_FALSE;

    jint res = JNI_CreateJavaVM(&m_jvm, (void**)&m_env, &args);
    return res == JNI_OK;
}

void PluginManager::loadPlugins() {
    if (!m_env) return;
    g_pendingActions.clear();

    QDir pluginDir(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + "/plugins");
    if (!pluginDir.exists()) return;

    for (const QFileInfo& fi : pluginDir.entryInfoList({"*.jar"}, QDir::Files)) {
        QString className = fi.completeBaseName();
        jclass cls = m_env->FindClass(className.toUtf8().constData());
        if (!cls) { m_env->ExceptionClear(); continue; }

        jmethodID midLoad = m_env->GetStaticMethodID(cls, "onLoad", "()V");
        if (!midLoad) { m_env->ExceptionClear(); continue; }

        m_env->CallStaticVoidMethod(cls, midLoad);
        if (m_env->ExceptionCheck()) { m_env->ExceptionDescribe(); m_env->ExceptionClear(); continue; }
        m_loaded.append(className);
    }

    // Rejestracja menu
    if (!g_pendingActions.isEmpty()) {
        auto* mw = qobject_cast<QMainWindow*>(parent());
        if (!mw) return;
        QMenuBar* bar = mw->menuBar();
        QHash<QString, QMenu*> menus;

        for (const auto& a : g_pendingActions) {
            QMenu* m = menus.value(a.menu);
            if (!m) {
                m = bar->findChild<QMenu*>(a.menu);
                if (!m) m = bar->addMenu(a.menu);
                menus[a.menu] = m;
            }
            QString handler = a.handlerClass;
            JavaVM* jvm = m_jvm;
            QObject::connect(m->addAction(a.label), &QAction::triggered, mw, [jvm, handler]{
                JNIEnv* env = nullptr;
                bool attached = false;
                if (jvm->GetEnv((void**)&env, JNI_VERSION_1_8) != JNI_OK) {
                    jvm->AttachCurrentThread((void**)&env, nullptr);
                    attached = true;
                }
                jclass cls = env->FindClass(handler.toUtf8().constData());
                if (cls) {
                    jmethodID mid = env->GetStaticMethodID(cls, "onInvoke", "()V");
                    if (mid) env->CallStaticVoidMethod(cls, mid);
                    env->ExceptionClear();
                }
                if (attached) jvm->DetachCurrentThread();
            });
        }
    }
}

void PluginManager::shutdownJVM() {
    if (m_jvm) {
        m_jvm->DestroyJavaVM();
        m_jvm = nullptr;
        m_env = nullptr;
    }
}
