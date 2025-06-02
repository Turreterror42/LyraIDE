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
#include <QLabel>
#include <QDir>
#include "QCodeEditor.h"

#ifndef _WIN32
#include <qtermwidget.h>
#else
#include "CMDWidget.h"
#endif
#include "FileSidebarWidget.h"

class EditorTab : public QWidget {
    Q_OBJECT

public:
    QString filePath;
    QMonacoEditor *editor;

    explicit EditorTab(bool theme, QWidget *parent = nullptr) : QWidget(parent) {
        editor = new QMonacoEditor(this);
        QVBoxLayout *layout = new QVBoxLayout(this);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->addWidget(editor);
        setLayout(layout);
    }
};

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr) : QMainWindow(parent) {
        setWindowTitle("LyraIDE");
        setWindowIcon(QIcon(":/images/icons/icon.ico"));
        initUI();
        setStyle();
    }

    ~MainWindow() {
        qDebug() << "Good Bye";
    }

private:
    QMenu *fileMenu, *fileEdit, *fileHelp;
    QAction *newFileAction, *openAction, *saveAction, *saveAsAction, *quitAction;
    QAction *undoAction, *redoAction, *selectAllAction, *aboutQt;
    QTabWidget *tabWidget;
    FileSidebarWidget *sidebar;
    QStatusBar *statusBar;
    QMap<QString, QString> extensionToLanguageMap;

#ifndef _WIN32
    QTermWidget *terminal;
#else
    CMDWidget *terminal;
#endif

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

#ifndef _WIN32
        terminal = new QTermWidget;
        terminal->setShellProgram("/bin/bash");
        terminal->setScrollBarPosition(QTermWidget::ScrollBarRight);
#else
        terminal = new CMDWidget;
#endif
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

    void connectActions() {
        connect(newFileAction, &QAction::triggered, this, &MainWindow::createNewTab);
        connect(openAction, &QAction::triggered, this, &MainWindow::openFile);
        connect(saveAction, &QAction::triggered, this, &MainWindow::saveFile);
        connect(saveAsAction, &QAction::triggered, this, &MainWindow::saveFileAs);
        connect(quitAction, &QAction::triggered, qApp, &QApplication::quit);

        connect(aboutQt, &QAction::triggered, qApp, &QApplication::aboutQt);
        connect(sidebar, &FileSidebarWidget::fileSelected, this, &MainWindow::openFromPath);
        connect(tabWidget, &QTabWidget::tabCloseRequested, this, &MainWindow::closeTab);
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

        tab->filePath = filePath;
        tabWidget->setTabText(tabWidget->indexOf(tab), info.fileName());
        statusBar->showMessage(QString("Editing: %1 | Language: %2").arg(info.fileName(), lang));
    }

    void setStyle() {
        // I will add a light theme later
        if (isDarkMode() || !isDarkMode()) {
#ifndef _WIN32
            terminal->setColorScheme(":/ColorSchemes/Dark.colorscheme");
#else
            terminal->setDarkTheme(true);
#endif
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

    int findOpenTabByPath(const QString &filePath) {
        for (int i = 0; i < tabWidget->count(); ++i) {
            EditorTab *tab = qobject_cast<EditorTab *>(tabWidget->widget(i));
            if (tab && tab->filePath == filePath) {
                return i;
            }
        }
        return -1;
    }

    QString loadFileContent(const QString &filePath) {
        QFile file(filePath);
        if (!file.exists() || !file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QMessageBox::warning(this, tr("Error"), tr("Cannot open file"));
            return QString();
        }

        QTextStream in(&file);
        QString content = in.readAll();
        file.close();
        return content;
    }

    EditorTab* createEditorTab(const QString &filePath = QString()) {
        EditorTab *tab = new EditorTab(isDarkMode());

        if (!filePath.isEmpty()) {
            tab->editor->setPlainText(loadFileContent(filePath));
            setLanguage(tab, filePath);
        }

        if (tabWidget->indexOf(tab) < 0) {
            tabWidget->addTab(tab, filePath.isEmpty() ? "Untitled" : QFileInfo(filePath).fileName());
        }

        tabWidget->setCurrentWidget(tab);
        return tab;
    }

    EditorTab* currentEditor() const {
        return qobject_cast<EditorTab*>(tabWidget->currentWidget());
    }

    void writeToFile(EditorTab *tab, const QString &filePath) {
        QFile file(filePath);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QMessageBox::warning(this, tr("Error"), tr("Cannot save file"));
            return;
        }

        QTextStream out(&file);
        out << tab->editor->toPlainText();
        file.close();

        tab->filePath = filePath;
        setLanguage(tab, filePath);
        statusBar->showMessage("Saved: " + filePath);
    }

private slots:
    void createNewTab() {
        EditorTab *tab = new EditorTab(isDarkMode());
        tabWidget->addTab(tab, "Untitled");
        tabWidget->setCurrentWidget(tab);
    }

    void openFile() {
        QString filePath = QFileDialog::getOpenFileName(this, tr("Open File"));
        if (!filePath.isEmpty()) {
            openFromPath(filePath);
        }
    }

    void openFromPath(const QString &filePath) {
        int existingIndex = findOpenTabByPath(filePath);
        if (existingIndex >= 0) {
            tabWidget->setCurrentIndex(existingIndex);
            return;
        }

        EditorTab *tab = new EditorTab(isDarkMode());
        tab->editor->setPlainText(loadFileContent(filePath));
        setLanguage(tab, filePath);
        tabWidget->addTab(tab, QFileInfo(filePath).fileName());
        tabWidget->setCurrentWidget(tab);
    }

    void saveFile() {
        EditorTab *tab = currentEditor();
        if (!tab) return;

        if (tab->filePath.isEmpty()) {
            saveFileAs();
        } else {
            writeToFile(tab, tab->filePath);
        }
    }

    void saveFileAs() {
        EditorTab *tab = currentEditor();
        if (!tab) return;

        QString fileName = QFileDialog::getSaveFileName(this, tr("Save File As"));
        if (!fileName.isEmpty()) {
            writeToFile(tab, fileName);
        }
    }

    void closeTab(int index) {
        QWidget *widget = tabWidget->widget(index);
        if (widget) {
            widget->deleteLater();
        }
        tabWidget->removeTab(index);
    }
};

#endif // MAINWINDOW_H
