#ifndef MQTTCLIENT_H
#define MQTTCLIENT_H

#include <QObject>
#include <QtMqtt/qmqttclient.h>
#include <QtMqtt/qmqttglobal.h>
#include <QSharedPointer>
#include <QTimer>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>

enum ReportState
{
    NotUploaded,
    UploadedToOldServer,    /* 旧服务器已被弃用，数据重新上传 */
    Uploaded
};

class MqttClient : public QObject
{
    Q_OBJECT
public:
    explicit MqttClient(QObject *parent = nullptr);
    enum ReportTable
    {
        time,
        upload,
        data
    };
    Q_ENUM(ReportTable)
    enum CountType
    {
        totalCount,
        usedCount
    };
    Q_ENUM(CountType)
    bool openDatabase();
public slots:
    void login(const QString &deviceId, const QString &password);
    void insert(qint64 time, int upload, QString dataString);
    int surplus();
    void uploadReport();
    void getSoftwareInfo();
signals:
    void setError();    // Device information has been saved
    void connected();
    void deviceInfoReceived(bool succeeded);
    void surplusCount(int count);
    void messageFromServer(QString);
    void newVerion(const QJsonObject &);
private slots:
    void connectToHost();
    void stateChanged(QMqttClient::ClientState state);
    void publish(const QMqttTopicName &topic, const QByteArray &message, quint8 qos, bool retain);
    void messageReceived(const QByteArray &message, const QMqttTopicName &topic);
    void getDeviceInfo();

private:
    QMqttTopicFilter subTopic(const QString &deviceId);
    bool compareVersion(QString ver1, QString ver2);
    void bindValue(QSqlQuery &sqlQuery, const QJsonObject &object);
private:
    QString m_deviceId;
    QString m_password;
    QMqttClient *m_client;
    const QString m_appId;
    const QString m_databaseName;
    const QString m_reportsTable;
    QSqlDatabase m_database;
};
typedef QSharedPointer<MqttClient> MqttClient_PTR;

#endif // MQTTCLIENT_H
