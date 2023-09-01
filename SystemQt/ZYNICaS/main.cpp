#include "mainwidget.h"
#include <QApplication>
#include <QTextCodec>
#include <QObject>
#include "datamanagement.h"
#include <threadservice.h>
//#include "singleapplication.h"
#include <qtsingleapplication.h>
#include <QTimer>
#include "mqttclient.h"
#include "logindialog.h"
#include <QPrintPreviewDialog>
#include "reportpainter.h"
#include <QPageSize>

DataManagement DataManagement::instance;

int main(int argc, char *argv[])
{
//    qRegisterMetaType<QAbstractSocket::SocketError>("QAbstractSocket::SocketError");
    QtSingleApplication a(QApplication::applicationName(),argc, argv);
    QPrinter printer(QPrinter::ScreenResolution);
    printer.setPageSize(QPageSize(QSizeF(210, 297), QPageSize::Millimeter));
    QPrintPreviewDialog dialog(&printer);
    dialog.setWindowTitle("报告预览");
    dialog.setWindowFlags(Qt::Dialog | Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);
    dialog.resize(QApplication::primaryScreen()->size());
    QObject::connect(&dialog, &QPrintPreviewDialog::paintRequested, [=](QPrinter *printer) {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("Reports.db");
        db.open();
        QSqlQuery query(db);
        query.exec(QString("SELECT time, data from reports"));
        while(query.next()) {
            if (query.value(0).toLongLong() == 1686643211404) {
                break;
            }
        }
        ReportPainter painter(ReportInfo(Printer_Type::General,
                                         Check_Mode::IntensiveCareUnit,
                                         true,
                                         QJsonDocument::fromJson(query.value(1).toString().toUtf8()).object()),
                              printer);
    });
    dialog.exec();
    return 0;
    if (!a.isRunning()) {
        // prevent system hibernation
        SetThreadExecutionState(ES_CONTINUOUS | ES_SYSTEM_REQUIRED | ES_DISPLAY_REQUIRED);
        // data management instance
        auto &ins = DataManagement::getInstance();
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

        widget->showMaximized();

        return a.exec();
    }
    else {
        a.sendMessage(QString("程序正在运行中！"));
    }
    return 0;
}
