#ifndef SERVERBACKEND_H
#define SERVERBACKEND_H

#include <QObject>
#include <QtMqtt/qmqttclient.h>
#include <QtMqtt/qmqttglobal.h>
#include <QScopedPointer>
#include <QDebug>


typedef QMqttClient::ClientState ClientState;

class ServerBackend : public QObject
{
    Q_OBJECT
public:
    explicit ServerBackend(QObject *parent = nullptr);
    virtual ~ServerBackend();

    void subscribe(const QString &topic);
public slots:
    void connectToHost(const QString &hostname, const quint16 &port);
    void publish(const QMqttTopicName &topic, const QByteArray &message, quint8 qos = 0, bool retain = false);
private slots:
    void stateChanged(ClientState state);
    void messageReviced(const QByteArray &message, const QMqttTopicName &topic);
private:

    static QString ctime();
signals:
    void quit();
private:
    QMqttClient *m_client;
};
typedef QScopedPointer<ServerBackend> ServerBackend_PTR;

#endif // SERVERBACKEND_H
