#include <QCoreApplication>
#include "mqttclient.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    MqttClient_PTR client(new MqttClient);
    client->connectToHost();

    return a.exec();
}
