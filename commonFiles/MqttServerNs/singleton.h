#ifndef SINGLETON_H
#define SINGLETON_H

#include <QObject>
#include <QMetaEnum>
#include <QtMqtt/qmqttclient.h>
#include <QDateTime>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QDebug>

#include "topicns.h"

#define TIME_DEBUG()  qDebug()<<Singleton::currentTime()

using namespace TopicNs;

class Singleton : public QObject
{
    Q_OBJECT
public:
    static QString currentTime();
    static QString serverAddress();
    static QString hostname();
    // mqtt
    static quint16 mqttPort();
    // html
    static quint16 listenPort();
    // database
    static quint16 databasePort();
    static QString databaseName();
    static QString userName();
    static QString password();
    static QString createUniqueId(const QString &macAddress, const QString &deviceId);
    template <class T>
    static QMqttTopicName getTopicName(const T &pTopic, const SecondaryTopic &sTopic, const QString &id)
    {
        return QMqttTopicName(QString("%1/%2/%3").arg(enumValueToKey(pTopic), enumValueToKey(sTopic), id));
    }
    template <class T>
    static QString enumName()
    {
        return QString(QMetaEnum::fromType<T>().enumName()).toLower();
    }
    template <class T>
    static QString enumValueToKey(const T &value)
    {
        return QString(QMetaEnum::fromType<T>().valueToKey(static_cast<int>(value))).toLower();
    }
    template <class T>
    static T enumKeyToValue(const QString &key)
    {
        QMetaEnum meta = QMetaEnum::fromType<T>();
        auto value = meta.keyToValue(key.toStdString().c_str());
        if (-1 == value) {
            for (int index = 0; index < meta.keyCount(); ++index) {
                if (0 == key.compare(QString(meta.key(index)), Qt::CaseInsensitive)) {
                    return T(meta.keyToValue(meta.key(index)));
                }
            }
        }
        return T(value);
    }
    template <class T>
    static QStringList enumKeys()
    {
        QStringList keys;
        auto meta = QMetaEnum::fromType<T>();
        for (int index = 0; index < meta.keyCount(); ++index) {
            keys<<QString(meta.key(index)).toLower();
        }
        return keys;
    }
    template <class T>
    static QString jsonToString(const T &json)
    {
        return QString(QJsonDocument(json).toJson(QJsonDocument::Compact));
    }
    template <class T>
    static QByteArray jsonToUtf8(const T &json)
    {
        return jsonToString(json).toUtf8();
    }
    static QJsonObject utf8ToJsonObject(const QByteArray &json)
    {
        return QJsonDocument::fromJson(json).object();
    }
    static QJsonArray utf8ToJsonArray(const QByteArray &json)
    {
        return QJsonDocument::fromJson(json).array();
    }
    static QJsonObject getJsonObject(const QSqlQuery &sqlQuery);
    void setDatabase(QSqlDatabase *database);
    QSqlDatabase database();
private:
    QSqlDatabase *m_database;
private:
    explicit Singleton(QObject *parent = nullptr);
    virtual ~Singleton();
    Singleton(const Singleton &) = delete;
    Singleton(const Singleton &&) = delete;
    const Singleton &operator=(const Singleton &) = delete;
public:
    static Singleton *getInstance()
    {
        static Singleton instance;
        return &instance;
    }
};

#endif // SINGLETON_H
