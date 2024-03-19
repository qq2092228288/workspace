#include "mainwidget.h"
#include <QApplication>
#include <QTextCodec>
#include <QObject>
#include "datamanagement.h"
#include <threadservice.h>
#include <qtsingleapplication.h>
#include <QTimer>
#include "mqttclient.h"
#include "logindialog.h"
#include "version.h"

#include "homewidget.h"

DataManagement DataManagement::instance;

int main(int argc, char *argv[])
{
    QtSingleApplication a(QApplication::applicationName(),argc, argv);
    if (!a.isRunning()) {
// Windows system prevents hibernation
#ifdef Q_OS_WIN
        SetThreadExecutionState(ES_CONTINUOUS | ES_SYSTEM_REQUIRED | ES_DISPLAY_REQUIRED);
#endif
        QApplication::setApplicationVersion(FILE_VERSION_STR);
        QApplication::setApplicationName(INTERNAL_NAME);
        QApplication::setOrganizationName(ORGANIZATION_NAME);
        QApplication::setOrganizationDomain(ORGANIZATION_DOMAIN);
#if 1
        // data management instance
        auto &ins = DataManagement::getInstance();
        ins.getArgs().init();
        // tebco serialport class move to thread
        ThreadService::getInstance()->objectMoveToThread(ins.getTebco());
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
        QDir reports(ins.getPaths().reports());
        if (!reports.exists()) {
            reports.mkpath(ins.getPaths().reports());
        }

        auto client = ins.mqttClient();
        client->openDatabase();
        if (client->deviceInfoIsEmpty()) {
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
        }


        MainWidget_PTR widget(new MainWidget);
        QObject::connect(client, &MqttClient::newVerion, widget.get(), &MainWidget::newVersion);
        QObject::connect(widget.get(), &MainWidget::destroyed, ins.getTebco(), &ZyTebco::stopTimer);

        widget->showMaximized();
#else
        HomeWidget widget;
        widget.showMaximized();

#endif
        return a.exec();
    }
    else {
        a.sendMessage(QString("程序正在运行中！"));
    }
    return 0;
}
