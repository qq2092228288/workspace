#include "mqttclient.h"

MqttClient::MqttClient(QObject *parent)
    : QObject{parent}
{
    m_client = new QMqttClient(this);
    m_client->setHostname("nicas.zeyaotebco.com");
    m_client->setPort(1883);

    connect(m_client, &QMqttClient::messageReceived, this, &MqttClient::messageReceived);
    connect(m_client, &QMqttClient::stateChanged, this, [=](QMqttClient::ClientState state){
        qDebug()<<"connect state: "<<state;
    });
}

void MqttClient::connectToHost()
{
    m_client->connectToHost();
}

void MqttClient::messageReceived(const QByteArray &message, const QMqttTopicName &topic)
{
    qDebug()<<topic.name()<<message;
}
