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
#include <QDir>
#include <KTextEditor/View>
#include <KTextEditor/Document>
#include <KTextEditor/Editor>

#ifndef _WIN32
#include <qtermwidget.h>
#else
#include "CMDWidget.h"
#endif
#include "FileSidebarWidget.h"

class EditorTab : public QWidget {
    Q_OBJECT

public:
    KTextEditor::Document *doc;
    KTextEditor::View *view;

    explicit EditorTab(KTextEditor::Editor*& editor, bool theme, QWidget *parent = nullptr) : QWidget(parent) {
        doc = editor->createDocument(nullptr);
        view = doc->createView(nullptr);

        QVBoxLayout *layout = new QVBoxLayout(this);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->addWidget(view);
        setLayout(layout);
    }
};

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr) : QMainWindow(parent) {
        TextEditor = KTextEditor::Editor::instance();

        setWindowTitle("LyraIDE");
        setWindowIcon(QIcon(":/images/icons/icon.ico"));
        initUI();
        setStyle();
    }

    ~MainWindow() override {
        qDebug() << "Good Bye";
    }

private:
    QMenu *fileMenu, *fileEdit, *fileTools, *fileHelp;
    QAction *seetingsAct, *aboutQt;
    QTabWidget *tabWidget;
    FileSidebarWidget *sidebar;
    QMap<QString, QString> extensionToLanguageMap;
    KTextEditor::Editor *TextEditor;

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
    }

    void createMenu() {
        fileMenu = menuBar()->addMenu("File");
        fileEdit = menuBar()->addMenu("Edit");
        fileTools = menuBar()->addMenu("Tools");
        fileHelp = menuBar()->addMenu("Help");

        seetingsAct = new QAction("Settings", this);
        aboutQt = new QAction("About Qt", this);

        fileTools->addAction(seetingsAct);
        fileHelp->addAction(aboutQt);
    }

    void connectActions() {
        QObject::connect(seetingsAct, &QAction::triggered, this, [this]() { TextEditor->configDialog(nullptr); });
        QObject::connect(aboutQt, &QAction::triggered, qApp, &QApplication::aboutQt);
        QObject::connect(sidebar, &FileSidebarWidget::fileSelected, this, &MainWindow::openFromPath);
        QObject::connect(tabWidget, &QTabWidget::tabCloseRequested, this, &MainWindow::closeTab);
    }

    void setStyle() {
        // For now, force dark mode style; extend to light later
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
            if (tab && tab->doc->url().toLocalFile() == filePath) {
                return i;
            }
        }
        return -1;
    }

    EditorTab* createEditorTab(const QString &filePath = QString()) {
        EditorTab *tab = new EditorTab(TextEditor, isDarkMode());

        if (!filePath.isEmpty()) {
            bool opened = tab->doc->openUrl(QUrl::fromLocalFile(filePath));
            if (!opened) {
                QMessageBox::warning(this, tr("Error"), tr("Cannot open file: %1").arg(filePath));
                delete tab;
                return nullptr;
            }
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

private slots:
    void createNewTab() {
        EditorTab *tab = new EditorTab(TextEditor, isDarkMode());
        tabWidget->addTab(tab, "Untitled");
        tabWidget->setCurrentWidget(tab);
    }

    void openFile() {
        QString filePath = QFileDialog::getOpenFileName(this, tr("Open File"));
        if (!filePath.isEmpty()) openFromPath(filePath);
    }

    void openFromPath(const QString &filePath) {
        int existingIndex = findOpenTabByPath(filePath);
        if (existingIndex >= 0) {
            tabWidget->setCurrentIndex(existingIndex);
            return;
        }

        EditorTab *tab = createEditorTab(filePath);
        if (!tab) return;
    }

    void saveFile() {
        EditorTab *tab = currentEditor();
        if (!tab) return;

        if (tab->doc->url().isEmpty()) {
            saveFileAs();
        } else {
            bool success = tab->doc->save();
            if (!success) QMessageBox::warning(this, tr("Save Error"), tr("Failed to save the file."));
        }
    }

    void saveFileAs() {
        EditorTab *tab = currentEditor();
        if (!tab) return;

        QString fileName = QFileDialog::getSaveFileName(this, tr("Save File As"));
        if (!fileName.isEmpty()) {
            bool success = tab->doc->saveAs(QUrl::fromLocalFile(fileName));
            if (!success) QMessageBox::warning(this, tr("Save Error"), tr("Failed to save the file."));
        }
    }

    void closeTab(int index) {
        QWidget *widget = tabWidget->widget(index);
        if (widget) {
            // Ask user if unsaved changes exist before closing - optional enhancement
            tabWidget->removeTab(index);
            widget->deleteLater();
        }
    }

    void updateStatusBarForCurrentTab(int index) {
        EditorTab *tab = qobject_cast<EditorTab*>(tabWidget->widget(index));
        if (!tab) return;

        QString fileName = tab->doc->url().toLocalFile().isEmpty() ? "Untitled" : QFileInfo(tab->doc->url().toLocalFile()).fileName();
        QString ext = QFileInfo(fileName).suffix().toLower();
        QString lang = extensionToLanguageMap.value(ext, "plaintext");
    }
};

#endif // MAINWINDOW_H
