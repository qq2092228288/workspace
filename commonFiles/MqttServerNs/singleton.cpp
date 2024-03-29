#include "singleton.h"
#include <QCryptographicHash>

QString Singleton::currentTime()
{
    return QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
}

QString Singleton::serverAddress()
{
    return QString("120.78.134.255");
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

QString Singleton::databaseType()
{
    return QString("QPSQL");
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

bool Singleton::compareVersion(QString ver1, QString ver2)
{
    QStringList list1 = ver1.replace(QRegExp("[a-zA-Z() ]"), "").split(".");
    QStringList list2 = ver2.replace(QRegExp("[a-zA-Z() ]"), "").split(".");
    if (list1.count() == list2.count()) {
        for (int index = 0; index < list1.count(); ++index) {
            if (list1.at(index).toInt() > list2.at(index).toInt()) {
                return true;
            }
            else if (list1.at(index).toInt() < list2.at(index).toInt()) {
                return false;
            }
        }
    }
    else {
        return true;
    }
    return false;
}

QJsonObject Singleton::getJsonObject(const QSqlQuery &sqlQuery)
{
    QJsonObject object;
    auto record = sqlQuery.record();
    for (int index = 0; index < record.count(); ++index) {
        auto value = sqlQuery.value(index);
        auto key = record.fieldName(index);
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
