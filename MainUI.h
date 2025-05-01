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

#include <qtermwidget.h>
#include "BridgeManager.h"
#include "FileSidebarWidget.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr) : QMainWindow(parent) {
        initMonaco();
    }
    ~MainWindow() {
        qDebug() << "Good Bye";
    }

private:
    QMenu *fileMenu;
    QAction *openAction;
    QAction *saveAction;
    QWebEngineView *view;
    BridgeManager *bridgeManager;
    FileSidebarWidget *sidebar;
    QMap<QString, QString> extensionToLanguageMap;

    void setupUI() {
        QWidget *window = new QWidget;
        QVBoxLayout *layout = new QVBoxLayout(window);

        QTermWidget *terminal = new QTermWidget;
        terminal->setShellProgram("/bin/bash");
        terminal->setScrollBarPosition(QTermWidget::ScrollBarRight);
        terminal->setMinimumHeight(size().height() / 8);

        QSplitter *splitterHoriz = new QSplitter(Qt::Horizontal);
        QSplitter *splitterVert = new QSplitter(Qt::Vertical);

        sidebar = new FileSidebarWidget;

        splitterVert->addWidget(view);
        splitterVert->addWidget(terminal);

        splitterHoriz->addWidget(sidebar);
        splitterHoriz->addWidget(splitterVert);

        splitterHoriz->setOpaqueResize(true);
        splitterVert->setOpaqueResize(true);

        splitterHoriz->setSizes({size().width() / 6, 5 * size().width() / 6});
        splitterVert->setSizes({5 * size().height() / 6, size().height() / 6});

        layout->addWidget(splitterHoriz);
        setCentralWidget(window);

        createMenu();
        connectItems();
        initializeExtensionMap();

        if (isDarkMode()) {
            terminal->setColorScheme(":/ColorSchemes/DarkPastels.colorscheme");
            bridgeManager->setTheme("vs-dark");
        } else {
            terminal->setColorScheme(":/ColorSchemes/Linux.colorscheme");
            bridgeManager->setTheme("vs");
        }
    }

    void createMenu() {
        fileMenu = menuBar()->addMenu("&File");

        openAction = new QAction("&Open", this);
        saveAction = new QAction("&Save", this);

        fileMenu->addAction(openAction);
        fileMenu->addAction(saveAction);
    }

    void connectItems() {
        QObject::connect(openAction, &QAction::triggered, this, &MainWindow::openFile);
        QObject::connect(saveAction, &QAction::triggered, this, &MainWindow::saveFile);
        QObject::connect(sidebar, &FileSidebarWidget::fileSelected, this, &MainWindow::openFromPath);
    }

    bool isDarkMode() {
        const auto scheme = QGuiApplication::styleHints()->colorScheme();
        return scheme == Qt::ColorScheme::Dark;
    }

    void initMonaco() {
        view = new QWebEngineView;
        view->setUrl(QUrl("qrc:///web/monaco.html"));
        bridgeManager = new BridgeManager(view);  // parented to view

        connect(view, &QWebEngineView::loadFinished, this, [this]() {
            qDebug() << "Monaco loaded. Sending initialization message.";
            setupUI();
        });
    }

    void initializeExtensionMap() {
        extensionToLanguageMap = {
            {"abap", "abap"}, {"apex", "apex"}, {"azcli", "azcli"},
            {"bat", "bat"}, {"bicep", "bicep"},
            {"c", "cpp"}, {"cpp", "cpp"}, {"cxx", "cpp"}, {"cc", "cpp"},
            {"cs", "csharp"}, {"css", "css"}, {"coffee", "coffeescript"},
            {"cmake", "cmake"}, {"clj", "clojure"}, {"cljs", "clojure"},
            {"cljc", "clojure"}, {"csp", "csp"}, {"cypher", "cypher"},
            {"dart", "dart"}, {"dockerfile", "dockerfile"},
            {"ex", "elixir"}, {"exs", "elixir"}, {"ecl", "ecl"},
            {"fs", "fsharp"}, {"fsi", "fsharp"}, {"fsx", "fsharp"},
            {"fsscript", "fsharp"}, {"ftl", "freemarker2"}, {"flow", "flow9"},
            {"go", "go"}, {"graphql", "graphql"}, {"gql", "graphql"},
            {"h", "cpp"}, {"hpp", "cpp"}, {"hxx", "cpp"},
            {"html", "html"}, {"htm", "html"}, {"hcl", "hcl"},
            {"hbs", "handlebars"}, {"ini", "ini"},
            {"java", "java"}, {"js", "javascript"}, {"jsx", "javascript"},
            {"json", "json"}, {"jl", "julia"},
            {"kt", "kotlin"}, {"kts", "kotlin"},
            {"less", "less"}, {"lua", "lua"}, {"l", "lexon"},
            {"ligo", "pascaligo"}, {"liquid", "liquid"},
            {"md", "markdown"}, {"mdx", "mdx"}, {"mips", "mips"},
            {"m3", "m3"}, {"cm3", "m3"}, {"m3i", "m3"},
            {"msdax", "msdax"}, {"m", "objective-c"}, {"mligo", "cameligo"},
            {"mysql", "mysql"},
            {"pas", "pascal"}, {"pp", "pascal"}, {"php", "php"},
            {"pl", "perl"}, {"pm", "perl"}, {"ps1", "powershell"},
            {"psm1", "powershell"}, {"py", "python"}, {"pyc", "python"},
            {"pyw", "python"}, {"pq", "powerquery"}, {"proto", "protobuf"},
            {"pla", "pla"}, {"pgsql", "pgsql"}, {"pug", "pug"},
            {"pats", "postiats"}, {"qs", "qsharp"},
            {"r", "r"}, {"rb", "ruby"}, {"rs", "rust"},
            {"rst", "restructuredtext"}, {"cshtml", "razor"},
            {"redis", "redis"}, {"redshift", "redshift"},
            {"scss", "scss"}, {"sh", "shell"}, {"bash", "shell"},
            {"sql", "sql"}, {"swift", "swift"}, {"sol", "solidity"},
            {"st", "st"}, {"sv", "systemverilog"}, {"svh", "systemverilog"},
            {"scala", "scala"}, {"sc", "scala"}, {"scm", "scheme"},
            {"ss", "scheme"}, {"sb", "sb"}, {"sparql", "sparql"},
            {"sophia", "sophia"},
            {"ts", "typescript"}, {"tsx", "typescript"}, {"tcl", "tcl"},
            {"twig", "twig"}, {"tsp", "typespec"},
            {"vb", "vb"}, {"v", "systemverilog"}, {"wgsl", "wgsl"},
            {"xml", "xml"}, {"yaml", "yaml"}, {"yml", "yaml"}
        };
    }

private slots:
    void openFromPath(const QString &filePath) {
        if (filePath.isEmpty()) return;

        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QMessageBox::warning(this, tr("Error"), tr("Cannot open file: ") + file.errorString());
            return;
        }

        QTextStream in(&file);
        QString content = in.readAll();
        file.close();

        bridgeManager->loadText(content);

        QFileInfo fileInfo(filePath);
        QString extension = fileInfo.suffix().toLower();
        QString language = extensionToLanguageMap.value(extension, "plaintext");

        if (!language.isEmpty()) bridgeManager->setLanguage(language);
    }

    void openFile() {
        QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("All Files (*);;Text Files (*.txt);;C++ Files (*.cpp *.h)"));
        if (fileName.isEmpty()) return;

        QFile file(fileName);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QMessageBox::warning(this, tr("Error"), tr("Cannot open file: ") + file.errorString());
            return;
        }

        QTextStream in(&file);
        QString content = in.readAll();
        file.close();

        bridgeManager->loadText(content);

        QFileInfo fileInfo(fileName);
        QString extension = fileInfo.suffix().toLower();
        QString language = extensionToLanguageMap.value(extension, "plaintext");

        if (!language.isEmpty()) bridgeManager->setLanguage(language);
    }

    void saveFile() {
        QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), "", tr("All Files (*);;Text Files (*.txt);;C++ Files (*.cpp *.h)"));
        if (fileName.isEmpty()) return;

        // Retrieve the current content from Monaco Editor
        QString content;
        QEventLoop loop;
        connect(bridgeManager, &BridgeManager::textRetrieved, this, [&content, &loop](const QString &text) {
            content = text;
            loop.quit();
        });

        bridgeManager->retrieveText();
        loop.exec();

        QFile file(fileName);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QMessageBox::warning(this, tr("Error"), tr("Cannot save file: ") + file.errorString());
            return;
        }

        QTextStream out(&file);
        out << content;
        file.close();
    }
};

#endif // MAINWINDOW_H
