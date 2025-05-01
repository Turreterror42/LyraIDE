#include "MainUI.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow MainUI;
    MainUI.showMaximized();
    return app.exec();
}
