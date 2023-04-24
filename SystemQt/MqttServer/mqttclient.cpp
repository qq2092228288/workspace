#include "mqttclient.h"
#include "singleton.h"

MqttClient::MqttClient(QObject *parent)
    : QObject{parent},
      topicAnalysis_PTR{new TopicAnalysis}
{
    qRegisterMetaType<QMqttTopicName>("QMqttTopicName");

    m_thread = new QThread;

    m_client = new QMqttClient(this);
    m_client->setHostname(Singleton::hostname());
    m_client->setPort(Singleton::port());

    auto ptr = topicAnalysis_PTR.data();
    ptr->createTables();
    ptr->moveToThread(m_thread);

    connect(m_client, &QMqttClient::messageReceived, ptr, &TopicAnalysis::messageAnalysis);
    connect(ptr, &TopicAnalysis::messagePublish, this, &MqttClient::publish);
    connect(m_client, &QMqttClient::stateChanged, this, &MqttClient::stateChanged);

    m_thread->start();
}

void MqttClient::connectToHost()
{
    m_client->connectToHost();
}

MqttClient::~MqttClient()
{
    m_thread->quit();
    m_thread->wait();
    delete m_thread;
    delete m_client;
}

void MqttClient::stateChanged(QMqttClient::ClientState state)
{
    qDebug()<<"connect state: "<<state;
    if (QMqttClient::ClientState::Connected == state) {
        // subscribe topics
        m_client->subscribe(QMqttTopicFilter(Singleton::enumValueToKey(Singleton::PrimaryTopic::request) + "/#"));
        m_client->subscribe(QMqttTopicFilter(Singleton::enumValueToKey(Singleton::PrimaryTopic::append) + "/#"));
        m_client->subscribe(QMqttTopicFilter(Singleton::enumValueToKey(Singleton::PrimaryTopic::update) + "/#"));
        m_client->subscribe(QMqttTopicFilter(Singleton::enumValueToKey(Singleton::PrimaryTopic::remove) + "/#"));
    }
}

void MqttClient::publish(const QMqttTopicName &topic, const QByteArray &message, quint8 qos, bool retain)
{
    m_client->publish(topic, message, qos, retain);
}
