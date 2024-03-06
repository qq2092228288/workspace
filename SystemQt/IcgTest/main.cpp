#include <QApplication>
#include "mainwidget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWidget widget;
    widget.showMaximized();

    return a.exec();
}
