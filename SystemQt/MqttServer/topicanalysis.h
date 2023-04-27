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
        Insert,
        Update,
        Delete
    };
    Q_ENUM(DatabaseOperation)
public slots:
    void messageAnalysis(const QByteArray &message, const QMqttTopicName &topic);
signals:
    void messagePublish(const QMqttTopicName &topic, const QByteArray &message = QByteArray(),
                        quint8 qos = 0, bool retain = false);
    void error(const DatabaseEnumNs::MessageError &);
private:
    void request(const QByteArray &message, const SecondaryTopic &sTopic, const QString &id);
    void databaseOperation(const QByteArray &message, const SecondaryTopic &sTopic, const DatabaseOperation &type);
    QJsonObject getJsonObject(const QSqlQuery &sqlQuery, const QStringList &keys);
    void bindValue(QSqlQuery &sqlQuery, const QJsonObject &object);
    template <class T, class T1>
    void dbOperation(const QJsonObject &object, const DatabaseOperation &type, const T1 &column);
private:
    QSqlDatabase m_database;
};
typedef QSharedPointer<TopicAnalysis> TopicAnalysis_PTR;

#endif // TOPICANALYSIS_H
