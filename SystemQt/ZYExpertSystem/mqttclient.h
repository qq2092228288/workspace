#ifndef MQTTCLIENT_H
#define MQTTCLIENT_H

#include <QObject>
#include <QtMqtt/qmqttclient.h>
#include <QtMqtt/qmqttglobal.h>
#include <QJsonObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDebug>

class MqttClient : public QObject
{
    Q_OBJECT
public:
    QSqlDatabase &db();
public slots:
    void connectToHost();
    void login(const QString &username, const QString &password);
    void getReports();
private slots:
    void serverConnected();
    void messageReceived(const QByteArray &message, const QMqttTopicName &topic);
private:
    bool compareVersion(QString ver1, QString ver2);
    void getReport(const QString &deviceId);
signals:
    void stateChanged(QMqttClient::ClientState);
    void newVerion(const QJsonObject &);
    void loginStatus(const QJsonObject &);
    void reportPulled();
private:
    QString m_appId;
    QString m_mac;
    QSqlDatabase m_db;
    QMqttClient *m_client;
    QString m_username;
    QString m_deviceIds;
private:
    explicit MqttClient(QObject *parent = nullptr);
    virtual ~MqttClient();
    MqttClient(const MqttClient &) = delete;
    MqttClient(const MqttClient &&) = delete;
    MqttClient &operator=(const MqttClient &) = delete;
public:
    static MqttClient *getInstance();
};

#endif // MQTTCLIENT_H
