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
    NotUploaded,        /** 未上传 */
    UploadedToOldServer,/** 旧服务器已被弃用，数据重新上传 */
    Uploaded,           /** 已上传 */
    Modified            /** 已会诊 */
};

enum PullState
{
    Pulled,             /** 已拉取 */
    NotPulling,         /** 不需要拉取 */
    NoData              /** 已拉取，但无会诊 */
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
    bool deviceInfoIsEmpty();
public slots:
    void login(const QString &deviceId, const QString &password);
    void logout();
    void insert(qint64 time, QString dataString);
    int surplus();
    void uploadReport();
    void getSoftwareInfo();
    void publish(const QMqttTopicName &topic, const QByteArray &message, quint8 qos, bool retain);
    void getDeviceInfo();
    void pullingConclusion(qint64 time);
signals:
    void setError();    // Device information has been saved
    void connected();
    void deviceInfoReceived(bool succeeded);
    void surplusCount(int count);
    void messageFromServer(QString);
    void newVerion(const QJsonObject &);
    void loggedOut();
    void pulled(PullState);
private slots:
    void connectToHost();
    void stateChanged(QMqttClient::ClientState state);
    void messageReceived(const QByteArray &message, const QMqttTopicName &topic);
private:
    QMqttTopicFilter subTopic(const QString &deviceId);
    bool compareVersion(QString ver1, QString ver2);
    void bindValue(QSqlQuery &sqlQuery, const QJsonObject &object);
    void setLocalUserInfo();
private:
    QString m_deviceId;
    QString m_password;
    QMqttClient *m_client;
    const QString m_databaseName;
    const QString m_reportsTable;
    QSqlDatabase m_database;
};
typedef QSharedPointer<MqttClient> MqttClient_PTR;

#endif // MQTTCLIENT_H
