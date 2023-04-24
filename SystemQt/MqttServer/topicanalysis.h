#ifndef TOPICANALYSIS_H
#define TOPICANALYSIS_H

#include <QObject>
#include <QtMqtt/qmqttclient.h>
#include <QSharedPointer>
#include <QSqlDatabase>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QSqlQuery>
#include <QMutex>

#include "singleton.h"

typedef Singleton::PrimaryTopic PrimaryTopic;
typedef Singleton::SecondaryTopic SecondaryTopic;

class DatabaseManager
{
public:
    DatabaseManager(QSqlDatabase &database) : m_database{database}
    {
        m_mutex.lock();
        if (!m_database.isOpen()) m_database.open();
    }
    ~DatabaseManager()
    {
        if (m_database.isOpen()) m_database.close();
        m_mutex.unlock();
    }
private:
    DatabaseManager(const DatabaseManager &) = delete;
    DatabaseManager(const DatabaseManager &&) = delete;
    const DatabaseManager &operator=(const DatabaseManager &) = delete;
    void *operator new(size_t) = delete;
    void operator delete(void *) = delete;
private:
    QMutex m_mutex;
    QSqlDatabase &m_database;
};

class TopicAnalysis : public QObject
{
    Q_OBJECT
public:
    explicit TopicAnalysis(QObject *parent = nullptr);
    void createTables();
    enum class MessageError
    {
        NoError,
        TopicInvalid,
        IsEmpty,
        IncorrectFormat,
        NoAnalysis
    };
    Q_ENUM(MessageError)
    enum class AgentInfo
    {
        agentId,
        name,
        contact,
        address,
        remarks
    };
    Q_ENUM(AgentInfo)
    enum class AdministratorInfo
    {
        amdinId,
        password,
        name,
        permission,
        remarks
    };
    Q_ENUM(AdministratorInfo)
    enum class AllocatedConsumables
    {
        consumablesId,
        type,
        uniqueId,
        agentId,
        adminId,
        count,
        status,
        time,
        receiveTime
    };
    Q_ENUM(AllocatedConsumables)
    enum class CombinedDevice
    {
        uniqueId,
        name,
        placeId,
        agentId,
        status,
        totalCount,
        usedCount
    };
    Q_ENUM(CombinedDevice)
    enum class Device
    {
        deviceId,
        type,
        batch,
        status,
        remarks
    };
    Q_ENUM(Device)
    enum class Computer
    {
        macAddress,
        type,
        systemVersion,
        batch,
        status,
        remarks
    };
    Q_ENUM(Computer)
    enum class ReportInfo
    {
        uniqueId,
        reportTime,
        data
    };
    Q_ENUM(ReportInfo)
    enum class SoftwareManagement
    {
        appId,
        name,
        version,
        path,
        content,
        time
    };
    Q_ENUM(SoftwareManagement)
    enum class PlaceInfo
    {
        placeId,
        hostName,
        secondaryName
    };
    Q_ENUM(PlaceInfo)
public slots:
    void messageAnalysis(const QByteArray &message, const QMqttTopicName &topic);
signals:
    void messagePublish(const QMqttTopicName &topic, const QByteArray &message = QByteArray(),
                        quint8 qos = 0, bool retain = false);
    void error(const TopicAnalysis::MessageError &);
private:
    void request(const QByteArray &message, const SecondaryTopic &sTopic, const QString &uniqueId);
    QJsonObject getJsonObject(const QSqlQuery &sqlQuery, const QStringList &keys);
private:
    QSqlDatabase m_database;
};
typedef QSharedPointer<TopicAnalysis> TopicAnalysis_PTR;

#endif // TOPICANALYSIS_H
