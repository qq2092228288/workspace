#ifndef TOPICANALYSIS_H
#define TOPICANALYSIS_H

#include <QObject>
#include <QtMqtt/qmqttclient.h>
#include <QSharedPointer>
#include <QSqlDatabase>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QMutex>

#include "databasens.h"
#include "topicns.h"

using namespace DatabaseEnumNs;
using namespace TopicNs;

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
    enum class DatabaseOperation
    {
        Select,
        Insert,
        Update,
        Delete
    };
    Q_ENUM(DatabaseOperation)
public slots:
    void messageAnalysis(const QByteArray &message, const QMqttTopicName &topic);
signals:
    void messagePublish(const QMqttTopicName &topic, const QByteArray &message = QByteArray(),
                        quint8 qos = 1, bool retain = false);
    void error(const DatabaseEnumNs::MessageError &);
private:
    bool legalAdminId(const QString &id);
    bool legalDeviceId(const QString &id);
    void response(const QByteArray &message, const QMqttTopicName &topic);
    QByteArray databaseOperation(const QByteArray &message, const QMqttTopicName &topic, const DatabaseOperation &type);
    void bindValue(QSqlQuery &sqlQuery, const QJsonObject &object);
    SecondaryTopic getSTopic(const QMqttTopicName &topic) const;
    template <class T>
    QByteArray dbOperation(const QJsonObject &object, const DatabaseOperation &type,
                           const QString &primaryKey, const QMqttTopicName &topic);
    template <class T>
    QByteArray dbOperation(const QJsonObject &object, const DatabaseOperation &type,
                           const T &column, const QMqttTopicName &topic);
private:
    QSqlDatabase m_database;
};
typedef QSharedPointer<TopicAnalysis> TopicAnalysis_PTR;

#endif // TOPICANALYSIS_H
