#pragma once
#include <QMainWindow>
#include <QString>
#include <QPoint>
#include "session.hpp"

class QTreeView;
class QFileSystemModel;
class QTabWidget;
class QSplitter;
class QLabel;
class QDockWidget;
class EditorWidget;
class TitleBar;
class PluginManager;
class ImagePanel;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

    EditorWidget* getCurrentEditor() const { return currentEditor(); }

protected:
    void changeEvent(QEvent* e) override;
    void closeEvent(QCloseEvent* e) override;
    bool eventFilter(QObject* obj, QEvent* e) override;

private slots:
    void openFolder();
    void openFile();
    void onTreeDoubleClicked(const QModelIndex& idx);
    void onTreeContextMenu(const QPoint& pos);
    void closeTab(int index);
    void onTabChanged(int index);
    void onCursorMoved(int line, int col);
    void addTerminal();

    void openBackgroundImageDialog();
    void clearBackgroundImage();

private:
    void setupUi();
    void setupMenu();
    void setupStatusBar();
    void setupTerminalDock();
    EditorWidget* openInTab(const QString& path);
    EditorWidget* currentEditor() const;

    void newFile(const QString& dir);
    void newFolder(const QString& dir);
    void renameEntry(const QModelIndex& idx);
    void deleteEntry(const QModelIndex& idx);

    void setBackgroundImage(const QString& path);
    void applyBackgroundImage(const QString& path);

    void restoreSession(const SessionState& s);
    SessionState captureSession() const;

    TitleBar* m_titleBar = nullptr;
    QSplitter* m_splitter = nullptr;
    QSplitter* m_contentSplitter = nullptr;
    QTreeView* m_tree = nullptr;
    QFileSystemModel* m_model = nullptr;
    QTabWidget* m_tabs = nullptr;
    ImagePanel* m_imagePanel = nullptr;
    QLabel* m_posLabel = nullptr;
    QLabel* m_encLabel = nullptr;
    QLabel* m_langLabel = nullptr;
    QDockWidget* m_terminalDock = nullptr;
    QTabWidget* m_terminalTabs = nullptr;
    PluginManager* m_pluginManager = nullptr;
    QString m_rootPath;
    QString m_backgroundImagePath;
};
