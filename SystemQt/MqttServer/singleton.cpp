#include "singleton.h"
#include <QCryptographicHash>

QString Singleton::currentTime()
{
    return QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
}

QString Singleton::hostname()
{
    return QString("nicas.zeyaotebco.com");
}

quint16 Singleton::port()
{
    return 1883;
}

QString Singleton::createUniqueId(const QString &macAddress, const QString &deviceId)
{
    QString md5 = QString("macAddress=%1;deviceId=%2").arg(macAddress.toLower(), deviceId.toLower());
    return QCryptographicHash::hash(md5.toLatin1(), QCryptographicHash::Md5).toHex().toLower();
}

Singleton::Singleton(QObject *parent)
    : QObject{parent}
{

}

Singleton::~Singleton()
{

}
