#ifndef ZEYAOTEBCOSDKPRIVATE_H
#define ZEYAOTEBCOSDKPRIVATE_H

#include <QObject>
#include <QJsonObject>
#include <QJsonDocument>
#include <QMutexLocker>
#include <QSerialPort>
#include <QtMqtt/qmqttclient.h>
#include <QtMqtt/qmqttglobal.h>

#include <iostream>
#include "datacalculation.h"

static qreal intercept(qreal value, int digit)
{
    return DatCa::invalid() != value ? (static_cast<int>(value * std::pow(10, digit)))/std::pow(10, digit) : value;
}

struct BaseData
{
    BaseData(int _gender = 0, int _age = 0, int _height = 0, int _weight = 0)
        : gender(_gender),age(_age),height(_height),weight(_weight)
    {
        sbp = 0;
        dbp = 0;
        map = 0;
        bsa = intercept(DatCa::cBsa(height, weight), 2);
        vept = intercept(DatCa::cVept(height, weight, gender), 0);
    }
    void setSbpAndDbp(int _sbp, int _dbp)
    {
        sbp = _sbp;
        dbp = _dbp;
        map = DatCa::cMap(sbp, dbp);
    }
    int gender;
    int age;
    int height;
    int weight;

    int sbp;
    int dbp;

    int map;
    double bsa;
    double vept;

};

class ZeYaoTebcoSDK;
class ZeYaoTebcoSDKPrivate : public QObject
{
    Q_OBJECT
public:
    explicit ZeYaoTebcoSDKPrivate(QObject *q);
    virtual ~ZeYaoTebcoSDKPrivate();
public:
    bool startCheck(int gender, int age, int height, int weight, const std::string &portname);
    void setSbpAndDbp(int sbp, int dbp);
    void endChecked();
    bool isChecking();
    int dataLength();
    int readAll(char *str);
//    std::string error();
private slots:
    void parsingMessages();
    void stateChanged(QMqttClient::ClientState state);
    void messageReceived(const QByteArray &message, const QMqttTopicName &topic);
private:
    QByteArray toUtf8(const QJsonObject &object) const;
private:
//    QMutex mutex;
    QSerialPort *m_pSerialPort;
    QJsonObject m_jsonObject;
    BaseData m_data;
    QMqttClient *m_client;
private:
    ZeYaoTebcoSDK *q_ptr;
};

#endif // ZEYAOTEBCOSDKPRIVATE_H
