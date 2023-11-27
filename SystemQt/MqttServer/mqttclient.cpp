#include "mqttclient.h"
#include <singleton.h>
#include <threadservice.h>
#include <QTimer>

MqttClient::MqttClient(QObject *parent)
    : QObject{parent},
      publishing{false}
{
    qRegisterMetaType<QMqttTopicName>("QMqttTopicName");
    qRegisterMetaType<MessageError>("MessageError");
    qRegisterMetaType<QMqttClient::ClientState>("ClientState");

    m_client = new QMqttClient;
    m_client->setHostname(Singleton::hostname());
    m_client->setPort(Singleton::mqttPort());
    ThreadService::getInstance()->objectMoveToThread(m_client);

    m_analysis = new TopicAnalysis;
    m_analysis->createTables();
    ThreadService::getInstance()->objectMoveToThread(m_analysis);

    connect(m_client, &QMqttClient::messageReceived, m_analysis, &TopicAnalysis::messageAnalysis);
    connect(m_analysis, &TopicAnalysis::publish, this, &MqttClient::appendMsg);
    connect(m_client, &QMqttClient::stateChanged, this, &MqttClient::stateChanged);
    connect(m_analysis, &TopicAnalysis::error, this, [=](const MessageError &error)
    {
        TIME_DEBUG()<<Singleton::enumValueToKey(error);
    });
    connect(m_client, &QMqttClient::messageSent, this, &MqttClient::messageSent);
}

MqttClient::~MqttClient()
{
    delete m_client;
    delete m_analysis;
}

void MqttClient::connectToHost()
{
    m_client->connectToHost();
}

void MqttClient::stateChanged(QMqttClient::ClientState state)
{
    TIME_DEBUG()<<"connect state: "<<state;
    if (QMqttClient::Connected == state) {
        // subscribe topics
        foreach (auto key, Singleton::enumKeys<PrimaryTopic>()) {
            m_client->subscribe(QMqttTopicFilter(key + "/#"));
        }
    }
    else if (QMqttClient::Disconnected == state) {
        QTimer::singleShot(10000, this, &MqttClient::connectToHost);
    }
}

void MqttClient::appendMsg(const QMqttTopicName &topic, const QByteArray &message)
{
    if (publishing) {
        m_queue.enqueue(TMsg(topic, message));
    }
    else {
        publishing = true;
        m_client->publish(topic, message, 2, false);
    }
}

void MqttClient::messageSent(qint32 id)
{
    if (!m_queue.empty()) {
        publishing = true;
        auto tmsg = m_queue.dequeue();
        m_client->publish(tmsg.topic, tmsg.msg, 2, false);
    }
    else {
        publishing = false;
    }
}
