#ifndef BASIC_H
#define BASIC_H

#include <QObject>
#include <QtMqtt/qmqttclient.h>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QMetaEnum>

#include "topicns.h"

using namespace TopicNs;

class Basic
{
public:
    static QString serverAddress();
    static quint16 mqttPort();
    template<class T>
    static QMqttTopicName getTopicName(const T &pTopic, const SecondaryTopic &sTopic, const QString &id)
    {
        return QMqttTopicName(QString("%1/%2/%3").arg(valueToLower(pTopic), valueToLower(sTopic), id));
    }
    template<class T>
    static QString valueToLower(const T &value)
    {
        return QString(QMetaEnum::fromType<T>().valueToKey(static_cast<int>(value))).toLower();
    }

    template<class T>
    static int keyToValue(const QString &key)
    {
        QMetaEnum meta = QMetaEnum::fromType<T>();
        auto value = meta.keyToValue(key.toStdString().c_str());
        if (-1 == value) {
            for (int index = 0; index < meta.keyCount(); ++index) {
                if (0 == key.compare(QString(meta.key(index)), Qt::CaseInsensitive)) {
                    return meta.keyToValue(meta.key(index));
                }
            }
        }
        return value;
    }
};

#endif // BASIC_H
