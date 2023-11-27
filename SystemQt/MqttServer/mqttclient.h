#ifndef MQTTCLIENT_H
#define MQTTCLIENT_H

#include <QObject>
#include <QtMqtt/qmqttclient.h>
#include <QtMqtt/qmqttglobal.h>
#include <QSharedPointer>
#include <QQueue>

#include "topicanalysis.h"

typedef struct TopicMessage
{
    TopicMessage(const QMqttTopicName &_topic, const QByteArray &_msg)
        : topic(_topic),msg(_msg){}
    QMqttTopicName topic;
    QByteArray msg;
}TMsg;

class MqttClient : public QObject
{
    Q_OBJECT
public:
    explicit MqttClient(QObject *parent = nullptr);
    virtual ~MqttClient();
signals:
public slots:
    void connectToHost();
    void stateChanged(QMqttClient::ClientState state);
    void appendMsg(const QMqttTopicName &topic, const QByteArray &message);
    void messageSent(qint32 id);
private:
    QMqttClient *m_client;
    TopicAnalysis *m_analysis;
    QQueue<TMsg> m_queue;
    std::atomic<bool> publishing;
};
typedef QSharedPointer<MqttClient> MqttClient_PTR;

#endif // MQTTCLIENT_H
