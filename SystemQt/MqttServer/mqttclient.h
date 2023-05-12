#ifndef MQTTCLIENT_H
#define MQTTCLIENT_H

#include <QObject>
#include <QtMqtt/qmqttclient.h>
#include <QtMqtt/qmqttglobal.h>
#include <QSharedPointer>
#include <QThread>

#include "topicanalysis.h"

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
    void publish(const QMqttTopicName &topic, const QByteArray &message, quint8 qos, bool retain);
private:
    QThread *m_thread;
    QMqttClient *m_client;
    TopicAnalysis_PTR topicAnalysis_PTR;
};
typedef QSharedPointer<MqttClient> MqttClient_PTR;

#endif // MQTTCLIENT_H
