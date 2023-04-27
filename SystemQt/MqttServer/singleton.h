#ifndef SINGLETON_H
#define SINGLETON_H

#include <QObject>
#include <QMetaEnum>
#include <QtMqtt/qmqttclient.h>
#include <QDateTime>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QDebug>

#include "topicns.h"

#define TIME_DEBUG()  qDebug()<<Singleton::currentTime()

using namespace TopicNs;

class Singleton : public QObject
{
    Q_OBJECT
public:
    static QString currentTime();
    static QString hostname();
    static quint16 port();
    static QString createUniqueId(const QString &macAddress, const QString &deviceId);
    static QMqttTopicName getTopicName(const PrimaryTopic &pTopic, const SecondaryTopic &sTopic, const QString &id)
    {
        return QMqttTopicName(QString("%1/%2/%3").arg(enumValueToKey(pTopic), enumValueToKey(sTopic), id));
    }
    template <class T>
    static QString enumName()
    {
        return QString(QMetaEnum::fromType<T>().enumName());
    }
    template <class T>
    static QString enumValueToKey(const T &value)
    {
        return QString(QMetaEnum::fromType<T>().valueToKey(static_cast<int>(value)));
    }
    template <class T>
    static int enumKeyToValue(const QString &key)
    {
        return QMetaEnum::fromType<T>().keyToValue(key.toStdString().c_str());
    }
    template <class T>
    static QStringList enumKeys()
    {
        QStringList keys;
        auto meta = QMetaEnum::fromType<T>();
        for (int index = 0; index < meta.keyCount(); ++index) {
            keys<<QString(meta.key(index));
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
