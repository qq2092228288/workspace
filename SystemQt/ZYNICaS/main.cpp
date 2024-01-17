#include "mainwidget.h"
#include <QApplication>
#include "datamanagement.h"
#include "singleapplication.h"

DataManagement DataManagement::instance;

int main(int argc, char *argv[])
{
    SingleApplication a(argc, argv);
    if (!a.isRunning()) {
        // 防止系统休眠
        SetThreadExecutionState(ES_CONTINUOUS | ES_SYSTEM_REQUIRED | ES_DISPLAY_REQUIRED);

        auto &ins = DataManagement::getInstance();
        ins.startThread();
        QObject::connect(&ins,&DataManagement::clear,&ins,&DataManagement::clearSlot);
        QObject::connect(&ins,&DataManagement::startCheck,ins.getTebco(),&ZyTebco::startCheckSlot);
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
        a.mainWidget = &w;
        w.showMaximized();
        return a.exec();
    }
    return 0;
}
