#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QMenuBar>
#include <QFileDialog>
#include <QWidget>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGuiApplication>
#include <QDebug>
#include <QScreen>
#include <QSize>
#include <QPalette>
#include <QStyleHints>
#include <QSplitter>
#include <QTabWidget>
#include <QMap>
#include <QTimer>
#include <QCloseEvent>
#include <QStatusBar>
#include <QLabel>
#include <QFileInfo>
#include <QWebEngineView>
#include <QWebChannel>
#include <QLabel>
#include <QWebEngineProfile>
#include <QWebEngineSettings>
#include <QHttpServer>
#include <QTcpServer>
#include <QDir>

#include <qtermwidget.h>
#include "BridgeManager.h"
#include "FileSidebarWidget.h"

class EditorTab : public QWidget {
    Q_OBJECT

public:
    QWebEngineView *view;
    BridgeManager *bridgeManager;
    QString filePath;

    EditorTab(bool theme, QTcpServer* server, QWidget *parent = nullptr) : QWidget(parent) {
        view = new QWebEngineView;

        QWebEngineProfile *profile = new QWebEngineProfile(this);
        profile->setHttpCacheMaximumSize(5 * 1024 * 1024);
        QWebEnginePage* page = new QWebEnginePage(profile, this);
        view->setPage(page);

        QWebEngineSettings *settings = view->settings();
        settings->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
        settings->setAttribute(QWebEngineSettings::PluginsEnabled, false);
        settings->setAttribute(QWebEngineSettings::LocalStorageEnabled, false);
        settings->setAttribute(QWebEngineSettings::WebGLEnabled, false);
        settings->setAttribute(QWebEngineSettings::Accelerated2dCanvasEnabled, false);
        settings->setAttribute(QWebEngineSettings::FullScreenSupportEnabled, false);

        if(theme) view->page()->setBackgroundColor(QColor(33, 37, 43, 255));

        QString urlString = QString("http://127.0.0.1:%1").arg(server->serverPort());
        view->setUrl(QUrl(urlString));

        bridgeManager = new BridgeManager(view);

        QObject::connect(view, &QWebEngineView::loadFinished, this, [this](bool ok) {
            if (ok) bridgeManager->setTheme("vs-dark");
        });

        QVBoxLayout *layout = new QVBoxLayout(this);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->addWidget(view);
        setLayout(layout);
    }
};



class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr) : QMainWindow(parent) {
        setWindowTitle("LyraIDE");
        setWindowIcon(QIcon(":/images/icons/icon.ico"));
        launchHttpServer();
        initUI();
        setStyle();
    }

    ~MainWindow() {
        qDebug() << "Good Bye";
    }

private:
    QMenu *fileMenu;
    QMenu *fileEdit;
    QMenu *fileHelp;
    QAction *newFileAction;
    QAction *openAction;
    QAction *saveAction;
    QAction *saveAsAction;
    QAction *quitAction;
    QAction *undoAction;
    QAction *redoAction;
    QAction *selectAllAction;
    QAction *aboutQt;
    QHttpServer server;
    QTcpServer *tcpServer;
    QTabWidget *tabWidget;
    FileSidebarWidget *sidebar;
    QStatusBar *statusBar;
    QMap<QString, QString> extensionToLanguageMap;
    QTermWidget *terminal;

    void initUI() {
        QWidget *central = new QWidget;
        QVBoxLayout *mainLayout = new QVBoxLayout(central);
        mainLayout->setContentsMargins(0, 0, 0, 0);

        QSplitter *horizSplitter = new QSplitter(Qt::Horizontal);
        QSplitter *vertSplitter = new QSplitter(Qt::Vertical);

        sidebar = new FileSidebarWidget;
        QVBoxLayout *sideLayout = new QVBoxLayout;
        QLabel *nameEdit = new QLabel("LyraIDE");
        nameEdit->setAlignment(Qt::AlignCenter);
        nameEdit->setFixedHeight(size().height() / 40);
        sideLayout->addWidget(nameEdit);
        sideLayout->addWidget(sidebar);
        QWidget *sidePanel = new QWidget;
        sidePanel->setLayout(sideLayout);

        tabWidget = new QTabWidget;
        tabWidget->setTabsClosable(true);

        terminal = new QTermWidget;
        terminal->setShellProgram("/bin/bash");
        terminal->setScrollBarPosition(QTermWidget::ScrollBarRight);
        terminal->setMinimumHeight(size().height() / 8);

        vertSplitter->addWidget(tabWidget);
        vertSplitter->addWidget(terminal);
        horizSplitter->addWidget(sidePanel);
        horizSplitter->addWidget(vertSplitter);

        horizSplitter->setSizes({size().width() / 6, 5 * size().width() / 6});
        vertSplitter->setSizes({5 * size().height() / 6, size().height() / 6});

        mainLayout->addWidget(horizSplitter);
        setCentralWidget(central);

        createMenu();
        connectActions();

        statusBar = new QStatusBar;
        setStatusBar(statusBar);
        statusBar->showMessage("Ready");

        initializeExtensionMap();
    }

    void createMenu() {
        fileMenu = menuBar()->addMenu("&File");
        fileEdit = menuBar()->addMenu("&Edit");
        fileHelp = menuBar()->addMenu("&Help");

        newFileAction = new QAction("New File", this);
        openAction = new QAction("Open", this);
        saveAction = new QAction("Save", this);
        saveAsAction = new QAction("Save As...", this);
        quitAction = new QAction("Quit", this);

        undoAction = new QAction("Undo", this);
        redoAction = new QAction("Redo", this);
        selectAllAction = new QAction("Select All", this);

        aboutQt = new QAction("About Qt", this);

        newFileAction->setShortcut(QKeySequence("Ctrl+N"));
        openAction->setShortcut(QKeySequence("Ctrl+O"));
        saveAction->setShortcut(QKeySequence("Ctrl+S"));
        saveAsAction->setShortcut(QKeySequence("Ctrl+Shift+S"));
        quitAction->setShortcut(QKeySequence("Ctrl+Q"));

        undoAction->setShortcut(QKeySequence("Ctrl+Z"));
        redoAction->setShortcut(QKeySequence("Ctrl+Shift+Z"));
        selectAllAction->setShortcut(QKeySequence("Ctrl+A"));

        fileMenu->addAction(newFileAction);
        fileMenu->addAction(openAction);
        fileMenu->addAction(saveAction);
        fileMenu->addAction(saveAsAction);
        fileMenu->addAction(quitAction);

        fileEdit->addAction(undoAction);
        fileEdit->addAction(redoAction);
        fileEdit->addAction(selectAllAction);

        fileHelp->addAction(aboutQt);
    }

    void launchHttpServer() {
        server.route("/", []() {
            const QString filePath = QDir::current().filePath(":/web/monaco.html");
            QFile file(filePath);

            if (!file.exists() || !file.open(QIODevice::ReadOnly | QIODevice::Text)) return QHttpServerResponse("text/plain", "Error: index.html not found");

            QByteArray content = file.readAll();
            return QHttpServerResponse("text/html", content);
        });

        tcpServer = new QTcpServer();
        if (!tcpServer->listen() || !server.bind(tcpServer)) {
            qCritical() << "Failed to bind server";
            delete tcpServer;
            return;
        }

        qDebug() << "Listening on port" << tcpServer->serverPort();

    }

    void connectActions() {
        QObject::connect(newFileAction, &QAction::triggered, this, [this]() {
            EditorTab *tab = new EditorTab(isDarkMode(), tcpServer);

            QObject::connect(tab->view, &QWebEngineView::loadFinished, this, [this, tab](bool ok) {
                if (ok) {
                    tabWidget->addTab(tab, "Untitled");
                    tabWidget->setCurrentWidget(tab);
                } else {
                    QMessageBox::warning(this, "Load Error", "Failed to load Monaco editor.");
                    tab->deleteLater();
                }
            });
        });

        QObject::connect(openAction, &QAction::triggered, this, &MainWindow::openFile);
        QObject::connect(saveAction, &QAction::triggered, this, &MainWindow::saveFile);
        QObject::connect(saveAsAction, &QAction::triggered, this, &MainWindow::saveFileAs);
        QObject::connect(quitAction, &QAction::triggered, qApp, &QApplication::quit);

        QObject::connect(undoAction, &QAction::triggered, this, [this]() { if (EditorTab *tab = currentEditor()) tab->bridgeManager->executeCommand("undo"); });
        QObject::connect(redoAction, &QAction::triggered, this, [this]() { if (EditorTab *tab = currentEditor()) tab->bridgeManager->executeCommand("redo"); });
        QObject::connect(selectAllAction, &QAction::triggered, this, [this]() { if (EditorTab *tab = currentEditor()) tab->bridgeManager->executeCommand("editor.action.selectAll"); });

        QObject::connect(aboutQt, &QAction::triggered, qApp, &QApplication::aboutQt);

        QObject::connect(sidebar, &FileSidebarWidget::fileSelected, this, &MainWindow::openFromPath);
        QObject::connect(tabWidget, &QTabWidget::tabCloseRequested, this, &MainWindow::closeTab);
    }

    void initializeExtensionMap() {
        extensionToLanguageMap = {
            {"cpp", "cpp"}, {"h", "cpp"}, {"py", "python"},
            {"js", "javascript"}, {"ts", "typescript"}, {"html", "html"},
            {"css", "css"}, {"json", "json"}, {"java", "java"},
            {"cs", "csharp"}, {"xml", "xml"}, {"sql", "sql"},
            {"sh", "shell"}, {"rb", "ruby"}, {"php", "php"},
            {"go", "go"}, {"rs", "rust"}, {"swift", "swift"},
            {"md", "markdown"}, {"yml", "yaml"}, {"yaml", "yaml"}
        };
    }

    void setLanguage(EditorTab *tab, const QString &filePath) {
        QFileInfo info(filePath);
        QString ext = info.suffix().toLower();
        QString lang = extensionToLanguageMap.value(ext, "plaintext");

        tab->bridgeManager->setLanguage(lang);
        tab->filePath = filePath;
        tabWidget->setTabText(tabWidget->indexOf(tab), info.fileName());
        statusBar->showMessage(QString("Editing: %1 | Language: %2").arg(info.fileName(), lang));
    }

    void setStyle() {
        // I will add a light theme later
        if (isDarkMode() || !isDarkMode()) {
            terminal->setColorScheme(":/ColorSchemes/Dark.colorscheme");
            setStyleSheet(R"(
                QMainWindow { background-color: #21252b; color: #abb2bf; font-family: 'Source Code Pro'; }
                QSplitter::handle { background-color: #181a1f; }
                QSplitter::handle:horizontal { width: 1px; }
                QSplitter::handle:vertical { height: 1px; }
                QStatusBar { border-top: 1px solid #181a1f; }
                QMenu::item:selected { background-color: #0e63bd; }
                QMenuBar::item { padding: 4px 7px; margin: 0px; min-height: 15px; border: none; }
                QMenuBar::item:hover, QMenuBar::item:pressed, QMenuBar::item:selected { padding: 4px 7px; margin: 0px; min-height: 15px; border: none; background-color: #4d4f50; border-radius: 5px; color: white; }
                QMenuBar { background-color: #3b3e3f; padding: 0px; margin: 0px; text-align: left; spacing: 0px; }
                QMenuBar::item:hover { background-color: #4d4f50; color: white; }
                QTabWidget::pane { border: 1px solid #2e2e2e; top: -1px; }
                QTabBar::tab { background: #21252b; color: #66717c; padding: 7px; border: 1px solid #2e2e2e; border-bottom: none; }
                QTabBar::tab:selected { background: #282c34; color: white; border-left: 2px solid #568af2; padding-left: 6px; background-clip: padding-box; }
                QScrollBar:vertical { background: transparent; width: 12px; margin: 0px; border-radius: 4px; }
                QScrollBar::handle:vertical { background: #4f75d6; min-height: 20px; border-radius: 4px; }
                QScrollBar::handle:vertical:hover { background: #6b8ae6; }
                QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { background: none; height: 0px; }
                QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical { background: none; }
                QStatusBar {color: white;}
            )");
            sidebar->setStyleSheet(R"(
                QTreeView { background-color: #21252b; border: none; color: #d3d3d3; }
                QTreeView::item { padding: 2px; color: #d3d3d3; border: none; outline: none; }
                QTreeView::item:selected { background-color: #4d78cc; color: white; }
            )");
        }
    }

    bool isDarkMode() {
        return QGuiApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark;
    }

    EditorTab* createEditorTab(const QString &filePath = QString()) {
        EditorTab *tab = new EditorTab(isDarkMode(), tcpServer);

        QObject::connect(tab->view, &QWebEngineView::loadFinished, this, [=](bool ok) {
            if (!ok) {
                QMessageBox::warning(this, "Load Error", "Failed to load the Monaco Editor.");
                delete tab;
                return;
            }

            if (!filePath.isEmpty()) {
                tab->bridgeManager->loadText(filePath);
                setLanguage(tab, filePath);
            }

            if (tabWidget->indexOf(tab) >= 0) {
                QString tabName = filePath.isEmpty() ? "Untitled" : QFileInfo(filePath).fileName();
                tabWidget->setTabText(tabWidget->indexOf(tab), tabName);
            }

            tabWidget->setCurrentWidget(tab);
        });

        return tab;
    }


    EditorTab* currentEditor() const {
        return qobject_cast<EditorTab*>(tabWidget->currentWidget());
    }

    void writeToFile(EditorTab *tab, const QString &filePath) {
        connect(tab->bridgeManager, &BridgeManager::textRetrieved, this,
                [=](const QString &content) {
                    QFile file(filePath);
                    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                        QMessageBox::warning(this, tr("Error"), tr("Cannot save file: ") + file.errorString());
                        return;
                    }

                    QTextStream out(&file);
                    out << content;
                    file.close();

                    tab->filePath = filePath;
                    setLanguage(tab, filePath);
                    statusBar->showMessage("Saved: " + filePath);
                }, Qt::SingleShotConnection);

        tab->bridgeManager->retrieveText();
    }

    int findOpenTabByPath(const QString &filePath) const {
        for (int i = 0; i < tabWidget->count(); ++i) {
            EditorTab *tab = qobject_cast<EditorTab*>(tabWidget->widget(i));
            if (tab && QFileInfo(tab->filePath) == QFileInfo(filePath)) {
                return i;
            }
        }
        return -1;
    }

private slots:
    void openFile() {
        QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"));
        if (!fileName.isEmpty()) openFromPath(fileName);
    }

    void openFromPath(const QString &filePath) {
        int existingIndex = findOpenTabByPath(filePath);
        if (existingIndex >= 0) {
            tabWidget->setCurrentIndex(existingIndex);
            return;
        }

        EditorTab *tab = new EditorTab(isDarkMode(), tcpServer);

        QObject::connect(tab->view, &QWebEngineView::loadFinished, this, [this, tab, filePath](bool ok) {
            if (!ok) {
                QMessageBox::warning(this, "Load Error", "Failed to load the Monaco Editor.");
                tab->deleteLater();
                return;
            }

            tab->bridgeManager->loadText(filePath);
            setLanguage(tab, filePath);

            tabWidget->addTab(tab, QFileInfo(filePath).fileName());
            tabWidget->setCurrentWidget(tab);
        });
    }
    void saveFile() {
        EditorTab *tab = currentEditor();
        if (!tab) return;

        if (tab->filePath.isEmpty()) saveFileAs();
        else writeToFile(tab, tab->filePath);
    }

    void saveFileAs() {
        EditorTab *tab = currentEditor();
        if (!tab) return;

        QString fileName = QFileDialog::getSaveFileName(this, tr("Save File As"));
        if (!fileName.isEmpty()) writeToFile(tab, fileName);
    }

    void closeTab(int index) {
        QWidget *widget = tabWidget->widget(index);
        if (widget) widget->deleteLater();
        tabWidget->removeTab(index);
    }
};

#endif // MAINWINDOW_H
