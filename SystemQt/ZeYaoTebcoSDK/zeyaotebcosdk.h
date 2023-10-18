#ifndef ZEYAOTEBCOSDK_H
#define ZEYAOTEBCOSDK_H

#include <QtCore/qglobal.h>

#if defined(ZEYAOTEBCOSDK_LIBRARY)
#  define ZEYAOTEBCOSDK_EXPORT Q_DECL_EXPORT
#else
#  define ZEYAOTEBCOSDK_EXPORT Q_DECL_IMPORT
#endif

#include <QObject>
#include <iostream>
#include <cstring>

using namespace std;

class ZeYaoTebcoSDKPrivate;
class ZEYAOTEBCOSDK_EXPORT ZeYaoTebcoSDK : public QObject
{
    Q_OBJECT
public:
    explicit ZeYaoTebcoSDK(QObject *parent = nullptr);
public:
    bool startCheck(int gender, int age, int height, int weight, const char *portname);
    void setSbpAndDbp(int sbp, int dbp);
    void endChecked();
    bool isChecking();
    int dataLength();
    void readAll(char *str);
    string readAll();
//    std::string error();
private:
    ZeYaoTebcoSDKPrivate *d_ptr;
};

#endif // ZEYAOTEBCOSDK_H
