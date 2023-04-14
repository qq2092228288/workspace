#include "serverbackend.h"
#include <QDateTime>

ServerBackend::ServerBackend(QObject *parent)
    : QObject{parent}
{
    m_client = new QMqttClient(this);
    m_client->setUsername("server");
    m_client->setPassword("zeyao2022");
    connect(m_client, &QMqttClient::stateChanged, this, &ServerBackend::stateChanged);
    connect(m_client, &QMqttClient::messageReceived, this, &ServerBackend::messageReviced);
}

ServerBackend::~ServerBackend()
{

}

void ServerBackend::subscribe(const QString &topic)
{
    if (!m_client->subscribe(topic)) {
        qDebug()<<QString("Could not subscribe. Is there a valid connection?");
    }
}

void ServerBackend::connectToHost(const QString &hostname, const quint16 &port)
{
    m_client->setHostname(hostname);
    m_client->setPort(port);
    m_client->connectToHost();
}

void ServerBackend::publish(const QMqttTopicName &topic, const QByteArray &message, quint8 qos, bool retain)
{
    m_client->publish(topic, message, qos, retain);
}

void ServerBackend::stateChanged(ClientState state)
{
    qDebug()<<ctime()<<state;
    if (ClientState::Connected == state) {
        subscribe("testtopic");
    }
}

void ServerBackend::messageReviced(const QByteArray &message, const QMqttTopicName &topic)
{
    qDebug()<<ctime()<<QString("topic: ")<<topic.name()<<QString(" message:")<<message;
}

QString ServerBackend::ctime()
{
    return QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
}
