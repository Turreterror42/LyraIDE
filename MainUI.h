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
        setStyleSheet(R"(
            /* Main Window */
            QMainWindow {
                background-color: #21252b;
                color: #abb2bf;
                font-family: 'Source Code Pro';
            }

            /* Splitter */
            QSplitter {
                margin: 0;
            }
            QSplitter::handle {
                background-color: #181a1f;
            }
            QSplitter::handle:horizontal {
                width: 1px;
            }
            QSplitter::handle:vertical {
                height: 1px;
            }

            QStatusBar {
                border-top: 1px solid #181a1f;
            }

            /* QMenu item pressed (applies only when the item is pressed) */
            QMenu::item:selected {
                background-color: #0e63bd;
            }
        )");
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
    QStatusBar *statusBar;
    QMap<QString, QString> extensionToLanguageMap;

    void setupUI() {
        QWidget *window = new QWidget;

        QVBoxLayout *layout = new QVBoxLayout(window);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(0);

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

        splitterHoriz->setHandleWidth(1);
        splitterVert->setHandleWidth(1);

        splitterHoriz->setOpaqueResize(true);
        splitterVert->setOpaqueResize(true);

        splitterHoriz->setContentsMargins(0, 0, 0, 0);
        splitterVert->setContentsMargins(0, 0, 0, 0);

        splitterHoriz->setSizes({size().width() / 6, 5 * size().width() / 6});
        splitterVert->setSizes({5 * size().height() / 6, size().height() / 6});

        layout->addWidget(splitterHoriz);
        setCentralWidget(window);

        statusBar = new QStatusBar;
        setStatusBar(statusBar);
        statusBar->showMessage("Ready");

        createMenu();
        connectItems();
        setWindowIcon(QIcon(":/images/icons/icon.ico"));
        initializeExtensionMap();

        if (isDarkMode()) {
            terminal->setColorScheme(":/ColorSchemes/Dark.colorscheme");
            bridgeManager->setTheme("vs-dark");
        } else {
            terminal->setColorScheme(":/ColorSchemes/Light.colorscheme");
            bridgeManager->setTheme("vs-light");
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
        bridgeManager = new BridgeManager(view);

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

    void setLanguageOfFile(const QString &fileNameOfExtension) {
        QFileInfo fileInfo(fileNameOfExtension);
        QString extension = fileInfo.suffix().toLower();
        QString language = extensionToLanguageMap.value(extension, "plaintext");

        if (!language.isEmpty()) bridgeManager->setLanguage(language);
        statusBar->showMessage(QString("Editing: %1 | Language: %2").arg(fileInfo.fileName(), language));

        setWindowTitle(QString("LyraIDE - %1").arg(fileInfo.fileName()));
    }

private slots:
    void openFromPath(const QString &filePath) {
        if (filePath.isEmpty()) return;
        bridgeManager->loadText(filePath);
        setLanguageOfFile(filePath);
    }

    void openFile() {
        QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("All Files (*);;Text Files (*.txt);;C++ Files (*.cpp *.h)"));
        if (fileName.isEmpty()) return;
        bridgeManager->loadText(fileName);
        setLanguageOfFile(fileName);
    }

    void saveFile() {
        QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), "", tr("All Files (*);;Text Files (*.txt);;C++ Files (*.cpp *.h)"));
        if (fileName.isEmpty()) return;

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
        statusBar->showMessage(QString("Saved: %1").arg(fileName));
    }
};

#endif // MAINWINDOW_H
