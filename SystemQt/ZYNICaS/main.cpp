#include "mainwidget.h"
#include <QApplication>
#include <QTextCodec>
#include <QObject>
#include "datamanagement.h"
#include "threadserivce.h"
#include "singleapplication.h"


DataManagement DataManagement::instance;
ThreadSerivce ThreadSerivce::instance;

int main(int argc, char *argv[])
{
    SingleApplication a(argc, argv);
    if (!a.isRunning()) {
        // prevent system hibernation
        SetThreadExecutionState(ES_CONTINUOUS | ES_SYSTEM_REQUIRED | ES_DISPLAY_REQUIRED);

        auto &ins = DataManagement::getInstance();
        ThreadSerivce::getInstance().objectMoveToThread(ins.getTebco());
        ThreadSerivce::getInstance().objectMoveToThread(ins.httpPost());
        qRegisterMetaType<DataList>("DataList");
        QObject::connect(&ins, &DataManagement::clear, &ins, &DataManagement::clearSlot);
        QObject::connect(&ins, &DataManagement::sendSerialName, ins.getTebco(), &ZyTebco::openSerial);
        QObject::connect(&ins, &DataManagement::onlineConsumableList, ins.httpPost(), &HttpPost::getConsumableList);
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
        ReportDataBase reportDatabase;
        DeviceDatabase deviceDatabase;
        ins.setReportDataBase(&reportDatabase);
        ins.setDeviceDatabase(&deviceDatabase);
        QObject::connect(&reportDatabase, &ReportDataBase::upload, ins.httpPost(), &HttpPost::reportUpload);
        QObject::connect(ins.httpPost(), &HttpPost::deviceInfo, &deviceDatabase, &DeviceDatabase::updateDeviceInfo);
        QObject::connect(ins.httpPost(), &HttpPost::quantitReceived, &deviceDatabase, &DeviceDatabase::updateConsumableList);
        QObject::connect(ins.httpPost(), &HttpPost::used, &deviceDatabase, &DeviceDatabase::uploaded);
        QObject::connect(&deviceDatabase, &DeviceDatabase::requestUseConsumable, ins.httpPost(), &HttpPost::useConsumable);

        // set deviceInfo for httpPost
        ins.httpPost()->setDeviceInfo(deviceDatabase.deviceInfo());
        // get device info
        ins.httpPost()->activeDevice(ins.getMac());
        // update device info when starting software
        if (ins.httpPost()->deviceOnlineNotice(deviceDatabase.getDeviceInfo("deviceId"))) { // device online
            // upload reports
            reportDatabase.dataUpload();
            // upload offline used consumable
            deviceDatabase.tryToUpload();
        }
        MainWidget w;
        a.mainWidget = &w;
        w.show();
        // update consumable list
        ins.requestConsumableList();
        return a.exec();
    }
    return 0;
}
