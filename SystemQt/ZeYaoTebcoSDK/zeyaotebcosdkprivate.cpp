#include "zeyaotebcosdkprivate.h"
#include "zeyaotebcosdk.h"


struct Value
{
    ushort data : 12;
    ushort type : 4;
};

ZeYaoTebcoSDKPrivate::ZeYaoTebcoSDKPrivate(QObject *q)
    : QObject{q},
      q_ptr{qobject_cast<ZeYaoTebcoSDK *>(q)}
{
    // 类型注册
    qRegisterMetaType<QMqttTopicName>("QMqttTopicName");
    qRegisterMetaType<QMqttClient::ClientState>("ClientState");
    // 串口
    m_pSerialPort = new QSerialPort(this);
    //串口配置
    m_pSerialPort->setParity(QSerialPort::NoParity);
    m_pSerialPort->setBaudRate(9600);
    m_pSerialPort->setDataBits(QSerialPort::Data8);
    m_pSerialPort->setStopBits(QSerialPort::OneStop);
    // 读取到串口数据
    connect(m_pSerialPort, &QSerialPort::readyRead, this, &ZeYaoTebcoSDKPrivate::parsingMessages);
    // MQTT
    m_client = new QMqttClient(this);
    m_client->setHostname(QString("120.78.134.255"));
    m_client->setPort(1883);
    connect(m_client, &QMqttClient::stateChanged, this, &ZeYaoTebcoSDKPrivate::stateChanged);
    connect(m_client, &QMqttClient::messageReceived, this, &ZeYaoTebcoSDKPrivate::messageReceived);
    m_client->connectToHost();
}

ZeYaoTebcoSDKPrivate::~ZeYaoTebcoSDKPrivate()
{
    endChecked();
//    m_buffer.close();
    if (m_pSerialPort->isOpen()) {
        m_pSerialPort->close();
    }
    m_pSerialPort->deleteLater();
}

bool ZeYaoTebcoSDKPrivate::startCheck(int gender, int age, int height, int weight, const char *portname)
{
    // 设置基本数据
    m_data = BaseData(gender, age, height, weight);
    // 串口已打开或名称为空
    if (m_pSerialPort->isOpen()) {
        return false;
    }
    // 设置串口名称
    m_pSerialPort->setPortName(QString(portname));
    return m_pSerialPort->open(QIODevice::ReadOnly);
}

void ZeYaoTebcoSDKPrivate::setSbpAndDbp(int sbp, int dbp)
{
    m_data.setSbpAndDbp(sbp, dbp);
}

void ZeYaoTebcoSDKPrivate::endChecked()
{
    if (m_pSerialPort->isOpen()) {
        m_pSerialPort->close();
    }
}

bool ZeYaoTebcoSDKPrivate::isChecking()
{
    return m_pSerialPort->isOpen();
}

int ZeYaoTebcoSDKPrivate::dataLength()
{
    if (6 != m_jsonObject.count()) {
        return 0;
    }
    return toUtf8(m_jsonObject).length();
}

int ZeYaoTebcoSDKPrivate::readAll(char *str)
{
    if (6 != m_jsonObject.count()) {
        return 0;
    }
    auto data = toUtf8(m_jsonObject);
    auto len = data.length();
    if (std::strlen(str) != len) {
        return 0;
    }
    m_jsonObject = QJsonObject();
    std::memcpy(str, data.constData(), len);
    return len;
}

string ZeYaoTebcoSDKPrivate::readAll()
{
    if (6 != m_jsonObject.count()) return string();
    auto data = QJsonDocument(m_jsonObject).toJson(QJsonDocument::Compact).toStdString();
    m_jsonObject = QJsonObject();
    return data;
}

void ZeYaoTebcoSDKPrivate::parsingMessages()
{
    auto data = m_pSerialPort->readAll();
    // 携带数据
    if (6 != data.size()) {
        return;
    }
    bool ok;
    auto vdata = data.mid(4, 2).toHex().toUShort(&ok, 16);
    Value value;
    std::memcpy(&value, &vdata, sizeof(Value));
    switch (value.type) {
    case 0:
        m_jsonObject.insert("hr", intercept(DatCa::cHr(value.data), 0));
        break;
    case 7:
    {
        auto si = intercept(DatCa::cSi(value.data, m_data.bsa, m_data.vept), 0);
        m_jsonObject.insert("si", si);
        m_jsonObject.insert("sv", intercept(DatCa::cSv(si, m_data.bsa), 1));
    }
        break;
    case 8:
    {
        auto ci = intercept(DatCa::cCi(value.data, m_data.bsa, m_data.vept), 1);
        m_jsonObject.insert("ci", ci);
        m_jsonObject.insert("co", intercept(DatCa::cCo(ci, m_data.bsa), 1));
        m_jsonObject.insert("hrv", intercept(DatCa::cHrv(ci), 0));
    }
        break;
    default:
        break;
    }
}

void ZeYaoTebcoSDKPrivate::stateChanged(QMqttClient::ClientState state)
{
    qDebug()<<state;
}

void ZeYaoTebcoSDKPrivate::messageReceived(const QByteArray &message, const QMqttTopicName &topic)
{
    qDebug()<<message<<topic;
}

QByteArray ZeYaoTebcoSDKPrivate::toUtf8(const QJsonObject &object) const
{
    return QString(QJsonDocument(object).toJson(QJsonDocument::Compact)).toUtf8();
}
