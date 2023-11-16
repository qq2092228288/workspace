#include "zeyaotebcosdk.h"
#include "zeyaotebcosdkprivate.h"
#include <thread>

ZeYaoTebcoSDK::ZeYaoTebcoSDK()
    : d_ptr{new ZeYaoTebcoSDKPrivate}
{

}

ZeYaoTebcoSDK::~ZeYaoTebcoSDK()
{

}

int ZeYaoTebcoSDK::login(string deviceId, string password)
{
    return d_ptr->login(QString::fromStdString(deviceId), QString::fromStdString(password));
}

bool ZeYaoTebcoSDK::open(string portname)
{
    return d_ptr->open(QString::fromStdString(portname));
}

int ZeYaoTebcoSDK::close()
{
    return d_ptr->close();
}

int ZeYaoTebcoSDK::start(string name, string id, int gender, int age, int height, int weight)
{
    return d_ptr->start(QString::fromStdString(name), QString::fromStdString(id), gender, age, height, weight);
}

int ZeYaoTebcoSDK::appendBpAndPostion(int sbp, int dbp, int postion)
{
    return d_ptr->appendBpAndPostion(sbp, dbp, postion);
}

int ZeYaoTebcoSDK::end()
{
    return d_ptr->end();
}

bool ZeYaoTebcoSDK::isOpen()
{
    return d_ptr->isOpen();
}

bool ZeYaoTebcoSDK::isDetecting()
{
    return d_ptr->isDetecting();
}

string ZeYaoTebcoSDK::availablePorts()
{
    return d_ptr->availablePorts();
}

string ZeYaoTebcoSDK::readAll()
{
    return d_ptr->readAll();
}

string ZeYaoTebcoSDK::deviceInfo()
{
    return d_ptr->deviceInfo();
}

