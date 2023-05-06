#include <QCoreApplication>
#include <qtsinglecoreapplication.h>
#include "mqttclient.h"


int main(int argc, char *argv[])
{
    QtSingleCoreApplication a("my QtSingleCoreApplication", argc, argv);
    if (a.isRunning()) {
        qDebug("The program is currently running.");
        return EXIT_SUCCESS;
    }
    MqttClient_PTR client(new MqttClient);
    client->connectToHost();
    return a.exec();
}
