#include "singleton.h"
#include <QCryptographicHash>

QString Singleton::currentTime()
{
    return QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
}

QString Singleton::hostname()
{
    return QString("localhost");
}

quint16 Singleton::mqttPort()
{
    return 1883;
}

quint16 Singleton::listenPort()
{
    return 18088;
}

quint16 Singleton::databasePort()
{
    return 5432;
}

QString Singleton::databaseName()
{
    return QString("zeyao_backend");
}

QString Singleton::userName()
{
    return QString("admin");
}

QString Singleton::password()
{
    return QString("zeyao2022");
}

QString Singleton::createUniqueId(const QString &macAddress, const QString &deviceId)
{
    QString md5 = QString("macAddress=%1;deviceId=%2").arg(macAddress.toLower(), deviceId.toLower());
    return QCryptographicHash::hash(md5.toLatin1(), QCryptographicHash::Md5).toHex().toLower();
}

QJsonObject Singleton::getJsonObject(const QSqlQuery &sqlQuery, const QStringList &keys)
{
    QJsonObject object;
    foreach (auto key, keys) {
        auto value = sqlQuery.value(key);
        switch (value.type()) {
        case QVariant::Type::Int:
            object.insert(key, value.toInt());
            break;
        case QVariant::Type::Double:
            object.insert(key, value.toDouble());
            break;
        case QVariant::Type::Bool:
            object.insert(key, value.toBool());
            break;
        case QVariant::Type::DateTime:
            object.insert(key, value.toDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz"));
            break;
        default:
            object.insert(key, value.toString());
            break;
        }
    }
    return object;
}

void Singleton::setDatabase(QSqlDatabase *database)
{
    m_database = database;
}

QSqlDatabase Singleton::database()
{
    return *m_database;
}

Singleton::Singleton(QObject *parent)
    : QObject{parent},m_database{nullptr}
{

}

Singleton::~Singleton()
{

}
