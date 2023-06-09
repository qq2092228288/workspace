#include "mainwidget.h"
#include <QApplication>
#include <QTextCodec>
#include <QObject>
#include "datamanagement.h"
#include "threadserivce.h"
//#include "singleapplication.h"
#include <qtsingleapplication.h>
#include <QTimer>
#include "mqttclient.h"
#include "logindialog.h"

DataManagement DataManagement::instance;
ThreadSerivce ThreadSerivce::instance;

int main(int argc, char *argv[])
{
//    qRegisterMetaType<QAbstractSocket::SocketError>("QAbstractSocket::SocketError");
    QtSingleApplication a(QApplication::applicationName(),argc, argv);
    if (!a.isRunning()) {
        // prevent system hibernation
        SetThreadExecutionState(ES_CONTINUOUS | ES_SYSTEM_REQUIRED | ES_DISPLAY_REQUIRED);
        // data management instance
        auto &ins = DataManagement::getInstance();
        // tebco serialport class move to thread
        ThreadSerivce::getInstance().objectMoveToThread(ins.getTebco());
        // clear old data
        QObject::connect(&ins, &DataManagement::clear, &ins, &DataManagement::clearSlot);
        // open serial port
        QObject::connect(&ins, &DataManagement::sendSerialName, ins.getTebco(), &ZyTebco::openSerial);
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

        auto client = ins.mqttClient();
        client->openDatabase();
        LoginDialog_PTR dialog(new LoginDialog);
        QObject::connect(&a, &QtSingleApplication::messageReceived, dialog.get(), &LoginDialog::sltMessageReceived);
        QObject::connect(dialog.get(), &LoginDialog::deviceIdAndPassword, client, &MqttClient::login);
        QObject::connect(client, &MqttClient::deviceInfoReceived, dialog.get(), &LoginDialog::loginSucceeded);
        QObject::connect(client, &MqttClient::connected, dialog.get(), &LoginDialog::serverConnected);
        QObject::connect(client, &MqttClient::messageFromServer, dialog.get(), &LoginDialog::serverMessage);
        dialog->exec();
        if (dialog->exitApplication()) {
            return 0;
        }
        else {
            QObject::disconnect(dialog.get(), &LoginDialog::deviceIdAndPassword, client, &MqttClient::login);
            QObject::disconnect(client, &MqttClient::deviceInfoReceived, dialog.get(), &LoginDialog::loginSucceeded);
            QObject::disconnect(client, &MqttClient::connected, dialog.get(), &LoginDialog::serverConnected);
            QObject::disconnect(client, &MqttClient::messageFromServer, dialog.get(), &LoginDialog::serverMessage);
        }

        MainWidget_PTR widget(new MainWidget);
        QObject::connect(client, &MqttClient::newVerion, widget.get(), &MainWidget::newVersion);

        widget->showMaximized();

        client->getSoftwareInfo();
        client->uploadReport();

        return a.exec();
    }
    else {
        a.sendMessage(QString("程序正在运行中！"));
    }
    return 0;
}
