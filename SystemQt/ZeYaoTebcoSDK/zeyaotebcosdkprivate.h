#ifndef ZEYAOTEBCOSDKPRIVATE_H
#define ZEYAOTEBCOSDKPRIVATE_H

#include <QObject>
#include <QJsonObject>
#include <QJsonDocument>
#include <QSerialPort>
#include <QtMqtt/qmqttclient.h>
#include <QtMqtt/qmqttglobal.h>
#include <QTimer>

#include <iostream>
#include <cmath>
#include <cstring>
#include "datacalculation.h"
#include "reportdatajson.h"

using namespace std;

static qreal intercept(qreal value, int digit)
{
    return DatCa::invalid() != value ? (static_cast<int>(value * pow(10, digit))) / pow(10, digit) : value;
}

struct BaseData
{
    BaseData(QString _name = QString(), QString _id = QString(),
             int _gender = 0, int _age = 0, int _height = 0, int _weight = 0)
        : name(_name),id(_id),gender(_gender),age(_age),height(_height),weight(_weight)
    {
        bsa = intercept(DatCa::cBsa(height, weight), 2);
        vept = intercept(DatCa::cVept(height, weight, gender), 0);
    }
    QString name;
    QString id;
    int gender;
    int age;
    int height;
    int weight;

    double bsa;
    double vept;

};

class ZeYaoTebcoSDKPrivate : public QObject
{
    Q_OBJECT
public:
    explicit ZeYaoTebcoSDKPrivate(QObject *parent = nullptr);
    virtual ~ZeYaoTebcoSDKPrivate();
    enum CountType
    {
        totalCount,
        usedCount
    };
    Q_ENUM(CountType)
    enum Error
    {
        NoError = 0,
        InputError = 1,
        MqttProxyServerNotStarted = 2,
        SerialPortNotOpened = 3,
        Detecting = 4,
        NoDeviceInformation = 5,
        InsufficientQuantity = 6,
        NotDetected = 7,
        NoDataDetected = 8,
        IncompleteData = 9
    };
    Q_ENUM(Error)
public:
    int login(const QString &deviceId, const QString &password);
    bool open(const QString &portname);
    int close();
    int start(const QString &name, const QString &id, int gender, int age, int height, int weight);
    int appendBpAndPostion(int sbp, int dbp, int postion);
    int end();
    bool isOpen();
    bool isDetecting();
    string availablePorts();
    string readAll();
    string deviceInfo();
private slots:
    void reconnect();
    void serialPortError(QSerialPort::SerialPortError error);
    void parsingMessages();
    void stateChanged(QMqttClient::ClientState state);
    void messageReceived(const QByteArray &message, const QMqttTopicName &topic);
    void publish(const QMqttTopicName &topic, const QByteArray &message, quint8 qos, bool retain);
private:
    bool dataIntegrity() const;
    QMqttTopicFilter subTopic(const QString &deviceId) const;
    template <class T>
    string toString(const T &qjson) const;
private:
    bool m_checking;            // 正在检测中
    QTimer *m_pTimer;           // 串口重连
    QSerialPort *m_pSerialPort; // 串口
    QJsonObject m_jsonObject;   // 存储串口处理后的串口数据
    BaseData m_data;            // 输入的基本信息
    QMqttClient *m_client;      // MQTT
    QString m_deviceId;         // 设备编号
    QJsonObject m_deviceInfo;   // 设备信息
    ReportDataJson m_reportData;// 需要上传的报告数据
};

#endif // ZEYAOTEBCOSDKPRIVATE_H
