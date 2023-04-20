#ifndef MQTTCLIENT_H
#define MQTTCLIENT_H

#include <QObject>
#include <QtMqtt/qmqttclient.h>
#include <QtMqtt/qmqttglobal.h>
#include <QSharedPointer>

class MqttClient : public QObject
{
    Q_OBJECT
public:
    explicit MqttClient(QObject *parent = nullptr);
    void connectToHost();
signals:
public slots:
    void messageReceived(const QByteArray &message, const QMqttTopicName &topic);
private:
    QMqttClient *m_client;
};
typedef QSharedPointer<MqttClient> MqttClient_PTR;

#endif // MQTTCLIENT_H
