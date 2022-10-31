#include "mainwidget.h"
#include <QApplication>
#include "datamanagement.h"


DataManagement DataManagement::instance;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // 防止系统休眠
    SetThreadExecutionState(ES_CONTINUOUS | ES_SYSTEM_REQUIRED | ES_DISPLAY_REQUIRED);

    auto &ins = DataManagement::getInstance();
    ins.startThread();
    QObject::connect(&ins,&DataManagement::clear,&ins,&DataManagement::clearSlot);

    ins.setSize(QApplication::primaryScreen()->availableSize());
    ins.initCurrentPath();
    QDir initDir(ins.getPaths().initDir());
    if (!initDir.exists()) {
        initDir.mkpath(ins.getPaths().initDir());
    }
    QDir tempDir(ins.getPaths().tempDir());
    if (!tempDir.exists()) {
        tempDir.mkpath(ins.getPaths().tempDir());
    }
    QDir reports(ins.getPaths().reports());
    if (!reports.exists()) {
        reports.mkpath(ins.getPaths().reports());
    }
    MainWidget w;
    w.show();
    return a.exec();
}
