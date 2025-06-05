#include "MainUI.h"
#include <QApplication>
#include <QFontDatabase>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QFontDatabase::addApplicationFont(":/fonts/JetBrainsMono-Regular.ttf");
    MainWindow MainUI;
    MainUI.showMaximized();
    return app.exec();
}
