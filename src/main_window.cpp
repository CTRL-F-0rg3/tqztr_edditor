#include "main_window.hpp"
#include "editor.hpp"
#include "title_bar.hpp"
#include "terminal.hpp"
#include "plugin_manager.hpp"
#include <QTreeView>
#include <QFileSystemModel>
#include <QTabWidget>
#include <QSplitter>
#include <QLabel>
#include <QMenuBar>
#include <QMenu>
#include <QStatusBar>
#include <QAction>
#include <QFileDialog>
#include <QKeySequence>
#include <QHeaderView>
#include <QMessageBox>
#include <QDockWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QCloseEvent>
#include <QApplication>
#include <QWindow>
#include <QTimer>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QInputDialog>
#include <QLineEdit>
#include <QStandardPaths>
#include <QPixmap>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    setupUi();
    setupStatusBar();
    setupTerminalDock();
    setupMenu();

    QString pluginDir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + "/plugins";
    m_pluginManager = new PluginManager(this);
    if (m_pluginManager->initJVM(pluginDir)) {
        m_pluginManager->loadPlugins();
    }

    qApp->installEventFilter(this);
    resize(1400, 900);
    setWindowTitle("TQZTR Editor");

    SessionState s = SessionManager::load();
    restoreSession(s);

    QTimer::singleShot(50, this, &MainWindow::addTerminal);
}

MainWindow::~MainWindow() = default;

void MainWindow::setupUi() {
    auto* central = new QWidget(this);
    auto* layout = new QVBoxLayout(central);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    m_titleBar = new TitleBar(central);
    layout->addWidget(m_titleBar);

    m_splitter = new QSplitter(Qt::Horizontal, central);
    layout->addWidget(m_splitter, 1);

    setCentralWidget(central);

    // ===== Drzewo plików =====
    m_model = new QFileSystemModel(this);
    m_model->setFilter(QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden);
    m_model->setReadOnly(true);
    m_model->setResolveSymlinks(false);

    m_tree = new QTreeView(m_splitter);
    m_tree->setModel(m_model);
    m_tree->setHeaderHidden(true);
    m_tree->setAnimated(false);
    m_tree->setUniformRowHeights(true);
    m_tree->setSortingEnabled(false);
    m_tree->setContextMenuPolicy(Qt::CustomContextMenu);
    for (int i = 1; i < 4; ++i) m_tree->setColumnHidden(i, true);
    m_splitter->addWidget(m_tree);

    // ===== Zakładki edytora =====
    m_tabs = new QTabWidget(m_splitter);
    m_tabs->setTabsClosable(true);
    m_tabs->setMovable(true);
    m_tabs->setDocumentMode(true);
    m_splitter->addWidget(m_tabs);

    m_splitter->setStretchFactor(0, 0);
    m_splitter->setStretchFactor(1, 1);
    m_splitter->setSizes({280, 1120});

    connect(m_tree, &QTreeView::doubleClicked, this, &MainWindow::onTreeDoubleClicked);
    connect(m_tree, &QTreeView::customContextMenuRequested,
            this, &MainWindow::onTreeContextMenu);
    connect(m_tabs, &QTabWidget::tabCloseRequested, this, &MainWindow::closeTab);
    connect(m_tabs, &QTabWidget::currentChanged, this, &MainWindow::onTabChanged);
}

void MainWindow::setupTerminalDock() {
    m_terminalDock = new QDockWidget("Terminal", this);
    m_terminalDock->setObjectName("terminalDock");
    m_terminalDock->setAllowedAreas(Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea);

    auto* dockWidget = new QWidget();
    auto* layout = new QVBoxLayout(dockWidget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    auto* bar = new QWidget();
    auto* barLayout = new QHBoxLayout(bar);
    barLayout->setContentsMargins(6, 4, 6, 4);
    barLayout->setSpacing(4);

    auto* newTermBtn = new QPushButton("+ New Terminal");
    newTermBtn->setCursor(Qt::PointingHandCursor);
    barLayout->addWidget(newTermBtn);
    barLayout->addStretch();
    layout->addWidget(bar);

    m_terminalTabs = new QTabWidget();
    m_terminalTabs->setTabsClosable(true);
    m_terminalTabs->setDocumentMode(true);
    m_terminalTabs->setMovable(true);
    layout->addWidget(m_terminalTabs, 1);

    m_terminalDock->setWidget(dockWidget);
    m_terminalDock->setMinimumHeight(180);
    addDockWidget(Qt::BottomDockWidgetArea, m_terminalDock);

    connect(newTermBtn, &QPushButton::clicked, this, &MainWindow::addTerminal);
    connect(m_terminalTabs, &QTabWidget::tabCloseRequested, this, [this](int i) {
        if (m_terminalTabs->count() <= 1) return;
        auto* w = m_terminalTabs->widget(i);
        m_terminalTabs->removeTab(i);
        w->deleteLater();
    });
}

void MainWindow::addTerminal() {
    if (!m_terminalTabs) return;
    auto* term = new TerminalWidget();
    QString workDir = m_rootPath.isEmpty() ? QDir::homePath() : m_rootPath;
    term->setWorkingDirectory(workDir);
    int idx = m_terminalTabs->addTab(
        term, QString("Term %1").arg(m_terminalTabs->count() + 1));
    m_terminalTabs->setCurrentIndex(idx);
    term->startShell();
}

void MainWindow::setupMenu() {
    auto* fileMenu = menuBar()->addMenu("&File");

    auto* openFolderAct = fileMenu->addAction("Open Folder...");
    openFolderAct->setShortcut(QKeySequence("Ctrl+Shift+O"));
    connect(openFolderAct, &QAction::triggered, this, &MainWindow::openFolder);

    auto* openFileAct = fileMenu->addAction("Open File...");
    openFileAct->setShortcut(QKeySequence::Open);
    connect(openFileAct, &QAction::triggered, this, &MainWindow::openFile);

    fileMenu->addSeparator();
    auto* saveAct = fileMenu->addAction("Save");
    saveAct->setShortcut(QKeySequence::Save);
    connect(saveAct, &QAction::triggered, this, [this]{
        if (auto* e = currentEditor()) e->saveFile();
    });
        fileMenu->addSeparator();
        auto* quitAct = fileMenu->addAction("Quit");
        quitAct->setShortcut(QKeySequence::Quit);
        connect(quitAct, &QAction::triggered, this, &QWidget::close);

        auto* viewMenu = menuBar()->addMenu("&View");
        auto* toggleTermAct = m_terminalDock->toggleViewAction();
        toggleTermAct->setShortcut(QKeySequence("Ctrl+`"));
        viewMenu->addAction(toggleTermAct);
        auto* newTermAct = viewMenu->addAction("New Terminal");
        newTermAct->setShortcut(QKeySequence("Ctrl+Shift+`"));
        connect(newTermAct, &QAction::triggered, this, &MainWindow::addTerminal);

        auto* appearanceMenu = menuBar()->addMenu("&Appearance");
        auto* setBgAct = appearanceMenu->addAction("Set Background Image...");
        connect(setBgAct, &QAction::triggered, this, &MainWindow::openBackgroundImageDialog);
        auto* clearBgAct = appearanceMenu->addAction("Clear Background Image");
        connect(clearBgAct, &QAction::triggered, this, &MainWindow::clearBackgroundImage);
}

void MainWindow::setupStatusBar() {
    m_posLabel  = new QLabel("Ln 1, Col 1", this);
    m_encLabel  = new QLabel("UTF-8", this);
    m_langLabel = new QLabel("Plain", this);
    statusBar()->addWidget(m_posLabel);
    statusBar()->addPermanentWidget(m_langLabel);
    statusBar()->addPermanentWidget(m_encLabel);
}

void MainWindow::openFolder() {
    QString dir = QFileDialog::getExistingDirectory(this, "Open Folder", m_rootPath);
    if (dir.isEmpty()) return;
    m_rootPath = dir;
    m_tree->setRootIndex(m_model->setRootPath(dir));
    setWindowTitle(QString("TQZTR Editor — %1").arg(dir));
}

void MainWindow::openFile() {
    QString path = QFileDialog::getOpenFileName(this, "Open File", m_rootPath);
    if (path.isEmpty()) return;
    openInTab(path);
}

void MainWindow::onTreeDoubleClicked(const QModelIndex& idx) {
    if (m_model->isDir(idx)) {
        m_tree->setExpanded(idx, !m_tree->isExpanded(idx));
        return;
    }
    openInTab(m_model->filePath(idx));
}

EditorWidget* MainWindow::openInTab(const QString& path) {
    for (int i = 0; i < m_tabs->count(); ++i) {
        auto* ed = qobject_cast<EditorWidget*>(m_tabs->widget(i));
        if (ed && ed->filePath() == path) {
            m_tabs->setCurrentIndex(i);
            return ed;
        }
    }

    auto* ed = new EditorWidget(this);
    if (!ed->loadFile(path)) {
        delete ed;
        return nullptr;
    }

    connect(ed, &EditorWidget::cursorMoved, this, &MainWindow::onCursorMoved);
    connect(ed, &EditorWidget::modificationChanged, this, [this, ed](bool) {
        int idx = m_tabs->indexOf(ed);
        if (idx >= 0) {
            QString name = ed->displayName();
            if (ed->isModified()) name += " ●";
            m_tabs->setTabText(idx, name);
        }
    });

    int idx = m_tabs->addTab(ed, ed->displayName());
    m_tabs->setCurrentIndex(idx);
    ed->setFocus();

    if (!m_backgroundImagePath.isEmpty())
        ed->setBackgroundPixmap(QPixmap(m_backgroundImagePath));

    return ed;
}

void MainWindow::closeTab(int index) {
    auto* ed = qobject_cast<EditorWidget*>(m_tabs->widget(index));
    if (!ed) return;
    if (ed->isModified()) ed->saveFile();
    m_tabs->removeTab(index);
    ed->deleteLater();
}

void MainWindow::onTabChanged(int index) {
    auto* ed = qobject_cast<EditorWidget*>(m_tabs->widget(index));
    if (!ed) return;
    int line, col;
    ed->getCursorPosition(&line, &col);
    onCursorMoved(line + 1, col + 1);
    ed->setFocus();
}

void MainWindow::onCursorMoved(int line, int col) {
    m_posLabel->setText(QString("Ln %1, Col %2").arg(line).arg(col));
}

EditorWidget* MainWindow::currentEditor() const {
    return qobject_cast<EditorWidget*>(m_tabs->currentWidget());
}

// ---------- Background image ----------

void MainWindow::openBackgroundImageDialog() {
    QString path = QFileDialog::getOpenFileName(
        this, "Set Background Image", QString(),
                                                "Images (*.png *.jpg *.jpeg *.webp *.bmp)");
    if (path.isEmpty()) return;
    setBackgroundImage(path);
}

void MainWindow::setBackgroundImage(const QString& path) {
    QPixmap pm(path);
    if (pm.isNull()) {
        QMessageBox::warning(this, "Error", "Cannot load image:\n" + path);
        return;
    }
    m_backgroundImagePath = path;
    for (int i = 0; i < m_tabs->count(); ++i) {
        if (auto* ed = qobject_cast<EditorWidget*>(m_tabs->widget(i)))
            ed->setBackgroundPixmap(pm);
    }
}

void MainWindow::applyBackgroundImage(const QString& path) {
    if (path.isEmpty()) return;
    QPixmap pm(path);
    if (pm.isNull()) return;
    m_backgroundImagePath = path;
    for (int i = 0; i < m_tabs->count(); ++i) {
        if (auto* ed = qobject_cast<EditorWidget*>(m_tabs->widget(i)))
            ed->setBackgroundPixmap(pm);
    }
}

void MainWindow::clearBackgroundImage() {
    m_backgroundImagePath.clear();
    for (int i = 0; i < m_tabs->count(); ++i) {
        if (auto* ed = qobject_cast<EditorWidget*>(m_tabs->widget(i)))
            ed->clearBackgroundPixmap();
    }
}

// ---------- Context menu ----------

void MainWindow::onTreeContextMenu(const QPoint& pos) {
    QModelIndex idx = m_tree->indexAt(pos);
    QMenu menu(this);

    QString targetDir;
    if (idx.isValid() && m_model->isDir(idx))
        targetDir = m_model->filePath(idx);
    else if (idx.isValid())
        targetDir = QFileInfo(m_model->filePath(idx)).absolutePath();
    else
        targetDir = m_rootPath;
    if (targetDir.isEmpty()) targetDir = m_rootPath;

    QAction* actNewFile   = menu.addAction("New File");
    QAction* actNewFolder = menu.addAction("New Folder");
    menu.addSeparator();

    QAction* actOpen   = nullptr;
    QAction* actRename = nullptr;
    QAction* actDelete = nullptr;

    if (idx.isValid() && !m_model->isDir(idx))
        actOpen = menu.addAction("Open");
    if (idx.isValid()) {
        actRename = menu.addAction("Rename");
        actDelete = menu.addAction("Delete");
    }
    if (actOpen || actRename || actDelete) menu.addSeparator();
    QAction* actRefresh = menu.addAction("Refresh");

    QAction* chosen = menu.exec(m_tree->viewport()->mapToGlobal(pos));
    if (!chosen) return;

    if (chosen == actNewFile)         newFile(targetDir);
    else if (chosen == actNewFolder)  newFolder(targetDir);
    else if (chosen == actOpen && idx.isValid())
        openInTab(m_model->filePath(idx));
    else if (chosen == actRename && idx.isValid())
        renameEntry(idx);
    else if (chosen == actDelete && idx.isValid())
        deleteEntry(idx);
    else if (chosen == actRefresh) {
        QString root = m_rootPath;
        m_tree->setRootIndex(m_model->setRootPath(""));
        m_tree->setRootIndex(m_model->setRootPath(root));
    }
}

void MainWindow::newFile(const QString& dir) {
    bool ok = false;
    QString name = QInputDialog::getText(
        this, "New File", "File name:", QLineEdit::Normal, "untitled.txt", &ok);
    if (!ok || name.trimmed().isEmpty()) return;

    QString path = QDir(dir).filePath(name);
    if (QFile::exists(path)) {
        QMessageBox::warning(this, "Error", "File already exists:\n" + path);
        return;
    }
    QFile f(path);
    if (!f.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(this, "Error", "Cannot create file:\n" + path);
        return;
    }
    f.close();
    openInTab(path);
}

void MainWindow::newFolder(const QString& dir) {
    bool ok = false;
    QString name = QInputDialog::getText(
        this, "New Folder", "Folder name:", QLineEdit::Normal, "new_folder", &ok);
    if (!ok || name.trimmed().isEmpty()) return;
    QString path = QDir(dir).filePath(name);
    if (QDir(path).exists()) {
        QMessageBox::warning(this, "Error", "Folder already exists:\n" + path);
        return;
    }
    if (!QDir().mkdir(path))
        QMessageBox::warning(this, "Error", "Cannot create folder:\n" + path);
}

void MainWindow::renameEntry(const QModelIndex& idx) {
    if (!idx.isValid()) return;
    QString oldPath = m_model->filePath(idx);
    QString baseName = QFileInfo(oldPath).fileName();
    bool ok = false;
    QString newName = QInputDialog::getText(
        this, "Rename", "New name:", QLineEdit::Normal, baseName, &ok);
    if (!ok || newName.trimmed().isEmpty() || newName == baseName) return;

    QString dir = QFileInfo(oldPath).absolutePath();
    QString newPath = QDir(dir).filePath(newName);
    if (QFile::exists(newPath)) {
        QMessageBox::warning(this, "Error", "Target already exists:\n" + newPath);
        return;
    }
    if (!QFile::rename(oldPath, newPath)) {
        QMessageBox::warning(this, "Error", "Rename failed:\n" + oldPath);
        return;
    }
    for (int i = 0; i < m_tabs->count(); ++i) {
        auto* ed = qobject_cast<EditorWidget*>(m_tabs->widget(i));
        if (ed && ed->filePath() == oldPath) {
            m_tabs->removeTab(i);
            ed->deleteLater();
            break;
        }
    }
}

void MainWindow::deleteEntry(const QModelIndex& idx) {
    if (!idx.isValid()) return;
    QString path = m_model->filePath(idx);
    bool isDir = m_model->isDir(idx);
    auto reply = QMessageBox::question(
        this, "Delete",
        QString("Delete %1?\n\n%2").arg(isDir ? "folder" : "file", path),
                                       QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (reply != QMessageBox::Yes) return;

    bool ok;
    if (isDir) ok = QDir(path).removeRecursively();
    else       ok = QFile::remove(path);
    if (!ok) {
        QMessageBox::warning(this, "Error", "Delete failed:\n" + path);
        return;
    }
    for (int i = m_tabs->count() - 1; i >= 0; --i) {
        auto* ed = qobject_cast<EditorWidget*>(m_tabs->widget(i));
        if (!ed) continue;
        QString fp = ed->filePath();
        if (fp == path || fp.startsWith(path + "/")) {
            m_tabs->removeTab(i);
            ed->deleteLater();
        }
    }
}

// ---------- Session ----------

void MainWindow::changeEvent(QEvent* e) {
    if (e->type() == QEvent::WindowTitleChange && m_titleBar)
        m_titleBar->setTitle(windowTitle());
    QMainWindow::changeEvent(e);
}

void MainWindow::closeEvent(QCloseEvent* e) {
    SessionManager::save(captureSession());
    QMainWindow::closeEvent(e);
}

SessionState MainWindow::captureSession() const {
    SessionState s;
    s.rootPath            = m_rootPath;
    s.activeTab           = m_tabs->currentIndex();
    s.geometry            = const_cast<MainWindow*>(this)->saveGeometry();
    s.windowState         = const_cast<MainWindow*>(this)->saveState();
    s.splitterState       = const_cast<MainWindow*>(this)->m_splitter->saveState();
    s.backgroundImagePath = m_backgroundImagePath;

    for (int i = 0; i < m_tabs->count(); ++i) {
        if (auto* ed = qobject_cast<EditorWidget*>(m_tabs->widget(i))) {
            TabState ts;
            ts.path = ed->filePath();
            if (ts.path.isEmpty()) continue;
            int line, col;
            ed->getCursorPosition(&line, &col);
            ts.line = line;
            ts.col  = col;
            s.tabs.append(ts);
        }
    }
    return s;
}

void MainWindow::restoreSession(const SessionState& s) {
    if (!s.rootPath.isEmpty() && QDir(s.rootPath).exists()) {
        m_rootPath = s.rootPath;
        m_tree->setRootIndex(m_model->setRootPath(s.rootPath));
        setWindowTitle(QString("TQZTR Editor — %1").arg(s.rootPath));
    }
    if (!s.geometry.isEmpty()) restoreGeometry(s.geometry);
    if (!s.windowState.isEmpty()) restoreState(s.windowState);
    if (!s.splitterState.isEmpty()) m_splitter->restoreState(s.splitterState);

    // Ustaw ścieżkę obrazka PRZED otwieraniem kart, żeby openInTab je od razu aplikował
    m_backgroundImagePath = s.backgroundImagePath;

    for (const auto& ts : s.tabs) {
        if (!QFile::exists(ts.path)) continue;
        if (auto* ed = openInTab(ts.path)) {
            ed->setCursorPosition(ts.line, ts.col);
            ed->ensureLineVisible(ts.line);
        }
    }
    if (s.activeTab >= 0 && s.activeTab < m_tabs->count())
        m_tabs->setCurrentIndex(s.activeTab);
}

// ---------- Edge resize (frameless window) ----------

bool MainWindow::eventFilter(QObject* obj, QEvent* e) {
    auto type = e->type();
    if (type != QEvent::MouseMove && type != QEvent::MouseButtonPress)
        return QMainWindow::eventFilter(obj, e);
    if (isMaximized() || isFullScreen())
        return QMainWindow::eventFilter(obj, e);

    auto* me = static_cast<QMouseEvent*>(e);
    QPoint lp = mapFromGlobal(me->globalPosition().toPoint());
    if (!rect().contains(lp)) {
        unsetCursor();
        return QMainWindow::eventFilter(obj, e);
    }
    const int M = 6;
    bool L = lp.x() < M;
    bool R = lp.x() >= width() - M;
    bool T = lp.y() < M;
    bool B = lp.y() >= height() - M;

    if (type == QEvent::MouseMove) {
        if ((L && T) || (R && B)) setCursor(Qt::SizeFDiagCursor);
        else if ((R && T) || (L && B)) setCursor(Qt::SizeBDiagCursor);
        else if (L || R) setCursor(Qt::SizeHorCursor);
        else if (T || B) setCursor(Qt::SizeVerCursor);
        else unsetCursor();
    } else if (type == QEvent::MouseButtonPress && me->button() == Qt::LeftButton) {
        Qt::Edges edges;
        if (L) edges |= Qt::LeftEdge;
        if (R) edges |= Qt::RightEdge;
        if (T) edges |= Qt::TopEdge;
        if (B) edges |= Qt::BottomEdge;
        if (edges && windowHandle()) {
            windowHandle()->startSystemResize(edges);
            return true;
        }
    }
    return QMainWindow::eventFilter(obj, e);
}
