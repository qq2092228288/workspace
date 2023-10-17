#include "zeyaotebcosdk.h"
#include "zeyaotebcosdkprivate.h"

ZeYaoTebcoSDK::ZeYaoTebcoSDK(QObject *parent)
    : QObject{parent},
      d_ptr{new ZeYaoTebcoSDKPrivate(this)}
{

}

bool ZeYaoTebcoSDK::startCheck(int gender, int age, int height, int weight, const char *portname)
{
    return d_ptr->startCheck(gender, age, height, weight, portname);
}

void ZeYaoTebcoSDK::setSbpAndDbp(int sbp, int dbp)
{
    d_ptr->setSbpAndDbp(sbp, dbp);
}

void ZeYaoTebcoSDK::endChecked()
{
    d_ptr->endChecked();
}

bool ZeYaoTebcoSDK::isChecking()
{
    return d_ptr->isChecking();
}

int ZeYaoTebcoSDK::dataLength()
{
    return d_ptr->dataLength();
}

void ZeYaoTebcoSDK::readAll(char *str)
{
    d_ptr->readAll(str);
}

