#include "mqttclient.h"
#include <singleton.h>
#include <threadservice.h>

MqttClient::MqttClient(QObject *parent)
    : QObject{parent},
      topicAnalysis_PTR{new TopicAnalysis}
{
    qRegisterMetaType<QMqttTopicName>("QMqttTopicName");
    qRegisterMetaType<MessageError>("MessageError");
    qRegisterMetaType<QMqttClient::ClientState>("ClientState");

    m_client = new QMqttClient(this);
    m_client->setHostname(Singleton::hostname());
    m_client->setPort(Singleton::mqttPort());

    auto ptr = topicAnalysis_PTR.get();
    ptr->createTables();
    ThreadService::getInstance()->objectMoveToThread(ptr);

    connect(m_client, &QMqttClient::messageReceived, ptr, &TopicAnalysis::messageAnalysis);
    connect(ptr, &TopicAnalysis::messagePublish, this, &MqttClient::publish);
    connect(m_client, &QMqttClient::stateChanged, this, &MqttClient::stateChanged);
    connect(ptr, &TopicAnalysis::error, this, [=](const MessageError &error)
    {
        TIME_DEBUG()<<Singleton::enumValueToKey(error);
    });
}

MqttClient::~MqttClient()
{
    delete m_client;
}

void MqttClient::connectToHost()
{
    m_client->connectToHost();
}

void MqttClient::stateChanged(QMqttClient::ClientState state)
{
    TIME_DEBUG()<<"connect state: "<<state;
    if (QMqttClient::ClientState::Connected == state) {
        // subscribe topics
        foreach (auto key, Singleton::enumKeys<PrimaryTopic>()) {
            m_client->subscribe(QMqttTopicFilter(key + "/#"));
        }
    }
}

void MqttClient::publish(const QMqttTopicName &topic, const QByteArray &message, quint8 qos, bool retain)
{
    m_client->publish(topic, message, qos, retain);
}
