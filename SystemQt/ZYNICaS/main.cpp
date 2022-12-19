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
        //set app info
        QCoreApplication::setApplicationName("无创血流动力学检测系统");
        QCoreApplication::setApplicationVersion("1.0.0.1");
        QCoreApplication::setOrganizationName("广东泽耀医疗技术有限公司");
        QCoreApplication::setOrganizationDomain("https://zeyaotebco.com/");
        // prevent system hibernation
        SetThreadExecutionState(ES_CONTINUOUS | ES_SYSTEM_REQUIRED | ES_DISPLAY_REQUIRED);

        auto &ins = DataManagement::getInstance();
        // tebco serialport class move to thread
        ThreadSerivce::getInstance().objectMoveToThread(ins.getTebco());
        // http request class
        ThreadSerivce::getInstance().objectMoveToThread(ins.httpPost());
        // register DataList struct
        qRegisterMetaType<DataList>("DataList");
        // singleton class needs to init QCoreApplication before connecting signals and slots.
        // clear old data
        QObject::connect(&ins, &DataManagement::clear, &ins, &DataManagement::clearSlot);
        // open serial port
        QObject::connect(&ins, &DataManagement::sendSerialName, ins.getTebco(), &ZyTebco::openSerial);
        // online get consumable list
        QObject::connect(&ins, &DataManagement::onlineConsumableList, ins.httpPost(), &HttpPost::getConsumableList);
        // set global ui size
        ins.setSize(QApplication::primaryScreen()->availableSize());
        // init app path
        ins.initCurrentPath();
        // mkdir init dir, temp dir, reports dir
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
        // create database
        ReportDataBase reportDatabase;
        DeviceDatabase deviceDatabase;
        ins.setReportDataBase(&reportDatabase);
        ins.setDeviceDatabase(&deviceDatabase);
        // rpeorts upload
        QObject::connect(&reportDatabase, &ReportDataBase::upload, ins.httpPost(), &HttpPost::reportUpload);
        // update device info
        QObject::connect(ins.httpPost(), &HttpPost::deviceInfo, &deviceDatabase, &DeviceDatabase::updateDeviceInfo);
        // update consumable list
        QObject::connect(ins.httpPost(), &HttpPost::quantitReceived, &deviceDatabase, &DeviceDatabase::updateConsumableList);
        // server return used data
        QObject::connect(ins.httpPost(), &HttpPost::used, &deviceDatabase, &DeviceDatabase::uploaded);
        // upload used consumable data
        QObject::connect(&deviceDatabase, &DeviceDatabase::requestUseConsumable, ins.httpPost(), &HttpPost::useConsumable);

        // update device info when starting software
        // set deviceInfo for httpPost
        ins.httpPost()->setDeviceInfo(deviceDatabase.deviceInfo());
        // get device info
        ins.httpPost()->activeDevice(ins.getMac());
        // upload reports
        reportDatabase.dataUpload();
        // upload offline used consumable
        deviceDatabase.tryToUpload();

        MainWidget w;
        a.mainWidget = &w;
        w.show();
        // update consumable list, exec message box for getiddialog  when get new consumable list.
        ins.requestConsumableList();
        return a.exec();
    }
    return 0;
}
