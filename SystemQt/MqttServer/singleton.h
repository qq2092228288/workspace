#ifndef SINGLETON_H
#define SINGLETON_H

#include "qjsonarray.h"
#include <QObject>
#include <QMetaEnum>
#include <QtMqtt/qmqttclient.h>
#include <QDateTime>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDebug>

#define TIME_DEBUG()  qDebug()<<Singleton::currentTime()

class Singleton : public QObject
{
    Q_OBJECT
public:
    enum class PrimaryTopic
    {
        request,                    // client request to server
        response,                   // server response to client
        append,                     // admin publish append message to server
        update,                     // admin publish update message to server
        remove                      // admin publish remove message to server
    };
    Q_ENUM(PrimaryTopic)
    enum class SecondaryTopic
    {
        deviceInfo,                 // client/server request/response device info
        uploadData,                 // client/server request/response upload data
        software,                   // client/server request/response software data
        signIn,                     // admin/server request/response sign in
        changePassword,             // admin update password
        device,                     // admin append/update/delete device
        computer,                   // admin append/update/delete computer
        combinedDevice,             // admin append/update/delete combined device
        allocatedConsumables,       // admin append allocated consumables
        place,                      // admin append/update/delete place
        agent,                      // admin append/update/delete agent
        reports                     // admin/server request/response reports
    };
    Q_ENUM(SecondaryTopic)
public:
    static QString currentTime();
    static QString hostname();
    static quint16 port();
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
    static QByteArray jsonToUtf8(const QJsonObject &json)
    {
        return QString(QJsonDocument(json).toJson(QJsonDocument::Compact)).toUtf8();
    }
    static QByteArray jsonToUtf8(const QJsonArray &json)
    {
        return QString(QJsonDocument(json).toJson(QJsonDocument::Compact)).toUtf8();
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
