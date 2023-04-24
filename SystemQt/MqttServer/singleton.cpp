#include "singleton.h"


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

Singleton::Singleton(QObject *parent)
    : QObject{parent}
{

}

Singleton::~Singleton()
{

}
