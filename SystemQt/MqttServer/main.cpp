#include <QCoreApplication>
#include <qtsinglecoreapplication.h>
#include <threadservice.h>
#include <singleton.h>
#include "mqttclient.h"
#include "htmlserver.h"

int main(int argc, char *argv[])
{
    QtSingleCoreApplication a("zeyao server", argc, argv);
    if (a.isRunning()) {
        TIME_DEBUG()<<"The program is currently running.";
        return EXIT_SUCCESS;
    }
    // mqtt
    MqttClient_PTR mqttClient(new MqttClient);
    mqttClient->connectToHost();

    // html
    HtmlServer_PTR htmlServer(new HtmlServer);
    htmlServer->startListening();
    ThreadService::getInstance()->objectMoveToThread(htmlServer.get());

    return a.exec();
}
