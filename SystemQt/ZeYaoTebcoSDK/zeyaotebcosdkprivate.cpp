#include "zeyaotebcosdkprivate.h"
#include <QSerialPortInfo>
#include "databasens.h"
#include "reportdataname.h"
#include "basic.h"

using namespace DatabaseEnumNs;

struct Value
{
    ushort data : 12;
    ushort type : 4;
};

ZeYaoTebcoSDKPrivate::ZeYaoTebcoSDKPrivate(QObject *parent)
    : QObject{parent},
      m_checking{false}
{
    // 类型注册
    qRegisterMetaType<QSerialPort::SerialPortError>("SerialThread");
    qRegisterMetaType<QMqttTopicName>("QMqttTopicName");
    qRegisterMetaType<QMqttClient::ClientState>("ClientState");
    // 定时器
    m_pTimer = new QTimer(this);
    connect(m_pTimer, &QTimer::timeout, this, &ZeYaoTebcoSDKPrivate::reconnect);
    // 串口
    m_pSerialPort = new QSerialPort(this);
    //串口配置
    m_pSerialPort->setParity(QSerialPort::NoParity);
    m_pSerialPort->setBaudRate(9600);
    m_pSerialPort->setDataBits(QSerialPort::Data8);
    m_pSerialPort->setStopBits(QSerialPort::OneStop);
    connect(m_pSerialPort, &QSerialPort::errorOccurred, this, &ZeYaoTebcoSDKPrivate::serialPortError);
    // MQTT
    m_client = new QMqttClient(this);
    m_client->setHostname(Basic::serverAddress());
    m_client->setPort(Basic::mqttPort());
    connect(m_client, &QMqttClient::stateChanged, this, &ZeYaoTebcoSDKPrivate::stateChanged);
    connect(m_client, &QMqttClient::messageReceived, this, &ZeYaoTebcoSDKPrivate::messageReceived);
    m_client->connectToHost();
    qDebug()<<__FUNCTION__<<this->thread();
}

ZeYaoTebcoSDKPrivate::~ZeYaoTebcoSDKPrivate()
{
    qDebug()<<__FUNCTION__<<this->thread();
    if (m_pSerialPort->isOpen())
        m_pSerialPort->close();
    if (m_pTimer->isActive())
        m_pTimer->stop();
}

int ZeYaoTebcoSDKPrivate::login(const QString &deviceId, const QString &password)
{
    qDebug()<<__FUNCTION__<<this->thread();
    // 输入错误
    if (deviceId.isEmpty() || password.isEmpty() || deviceId.length() > 32 || password.length() > 32)
        return Error::InputError;
    // MQTT代理服务器未连接
    if (QMqttClient::Connected != m_client->state())
        return Error::MqttProxyServerNotStarted;
    // 非空则清空
    if (!m_deviceInfo.empty())
        m_deviceInfo = QJsonObject();
    // 订阅此ID
    m_client->subscribe(subTopic(deviceId));
    m_deviceId = deviceId;
    // 推送
    publish(Basic::getTopicName(PrimaryTopic::request, SecondaryTopic::deviceInfo, m_deviceId),
            toString(QJsonObject{ { Basic::valueToLower(Device::password), password } }).toUtf8(), 2, false);
    return Error::NoError;
}

bool ZeYaoTebcoSDKPrivate::open(const QString &portname)
{
    qDebug()<<__FUNCTION__<<this->thread();
    // 串口已打开
    if (m_pSerialPort->isOpen())
        return false;
    // 设置串口名称
    m_pSerialPort->setPortName(portname);
    return m_pSerialPort->open(QIODevice::ReadOnly);
}

int ZeYaoTebcoSDKPrivate::close()
{
    qDebug()<<__FUNCTION__<<this->thread();
    // 未打开串口
    if (!m_pSerialPort->isOpen())
        return Error::SerialPortNotOpened;
    // 正在检查
    if (m_checking)
        return Error::Detecting;
    m_pSerialPort->close();
    return Error::NoError;
}

int ZeYaoTebcoSDKPrivate::start(const QString &name, const QString &id, int gender, int age, int height, int weight)
{
    qDebug()<<__FUNCTION__<<this->thread();
    // 串口未打开
    if (!m_pSerialPort->isOpen())
        return Error::SerialPortNotOpened;
    // 未获取设备信息
    if (m_deviceInfo.empty())
        return Error::NoDeviceInformation;
    // 有效验证码不足
    auto totalCount = m_deviceInfo.value(Basic::valueToLower(CountType::totalCount)).toString().toInt();
    auto usedCount = m_deviceInfo.value(Basic::valueToLower(CountType::usedCount)).toString().toInt();
    if (totalCount - usedCount <= 0)
        return Error::InsufficientQuantity;
    // 正在检测
    if (m_checking)
        return Error::Detecting;
    // 信息错误
    if (name.isEmpty() || name.length() > 100 || id.isEmpty() || id.length() > 100 || gender < 0 || gender > 1 ||
        age < 0 || age > 150 || height < 0 || height > 300 || weight < 0 || weight > 300) return Error::InputError;
    // 设置基本数据
    m_data = BaseData(name, id, gender, age, height, weight);
    // 报告
    m_reportData.setPatientInfo(name, id, gender, age, height, weight);
    // 读取串口数据
    connect(m_pSerialPort, &QSerialPort::readyRead, this, &ZeYaoTebcoSDKPrivate::parsingMessages, Qt::UniqueConnection);
    // 设置为正在检测
    m_checking = true;
    return Error::NoError;
}

int ZeYaoTebcoSDKPrivate::appendBpAndPostion(int sbp, int dbp, int postion)
{
    qDebug()<<__FUNCTION__<<this->thread();
    // 未进行检测
    if (!m_checking) return Error::NotDetected;
    // 输入错误
    if (sbp <= dbp || sbp < 0 || sbp > 300 || dbp < 0 || dbp > 300 || postion < 1 || postion > 3) return false;
    // 未检测到数据
    if (!dataIntegrity()) return Error::NoDataDetected;
    // 报告
    m_reportData.appendPosition(sbp, dbp, 4, 9, postion);
    return Error::NoError;
}

int ZeYaoTebcoSDKPrivate::end()
{
    qDebug()<<__FUNCTION__<<this->thread();
    // 未进行检测
    if (!m_checking) return Error::NotDetected;
    // 信息不完整
    auto reportData = m_reportData.getJson();
    auto position = reportData.value(Basic::valueToLower(ReportDataName::position)).toArray();
    if (0 == position.size()) return Error::IncompleteData;
    // 报告时间
    auto rtime = QDateTime::fromString(m_reportData.getReportTime(), "yyyyMMddhhmmsszzz");
    // 推送的数据
    QJsonObject object {
        { Basic::valueToLower(ReportInfo::reportTime), rtime.toString("yyyy-MM-dd hh:mm:ss.zzz") },
        { Basic::valueToLower(ReportInfo::deviceId), m_deviceId },
        { Basic::valueToLower(ReportInfo::name), m_data.id + "-" + m_data.name },
        { Basic::valueToLower(ReportInfo::modify), 0 },
        { Basic::valueToLower(ReportInfo::reportData), toString(reportData) }
    };
    // 上传数据
    publish(Basic::getTopicName(PrimaryTopic::request, SecondaryTopic::uploadData, m_deviceId),
            toString(object).toUtf8(), 2, false);
    // 清空
    m_reportData.clear();
    m_jsonObject = QJsonObject();
    // 使用数量+1
    auto usedCount = m_deviceInfo.value(Basic::valueToLower(CountType::usedCount)).toString().toInt();
    m_deviceInfo.insert(Basic::valueToLower(CountType::usedCount), QString::number(usedCount + 1));
    // 断开串口数据接收
    disconnect(m_pSerialPort, &QSerialPort::readyRead, this, &ZeYaoTebcoSDKPrivate::parsingMessages);
    // 设为未检测状态
    m_checking = false;
    return Error::NoError;
}

bool ZeYaoTebcoSDKPrivate::isOpen()
{
    return m_pSerialPort->isOpen();
}

bool ZeYaoTebcoSDKPrivate::isDetecting()
{
    return m_checking;
}


string ZeYaoTebcoSDKPrivate::availablePorts()
{
    qDebug()<<__FUNCTION__<<this->thread();
    QJsonArray array;
    foreach (auto info, QSerialPortInfo::availablePorts()) {
        array.append(info.portName());
    }
    return toString(array).toStdString();
}

string ZeYaoTebcoSDKPrivate::readAll()
{
    qDebug()<<__FUNCTION__<<this->thread();
    if (!dataIntegrity()) return string();
    auto data = QJsonDocument(m_jsonObject).toJson(QJsonDocument::Compact).toStdString();
    return data;
}

string ZeYaoTebcoSDKPrivate::deviceInfo()
{
    qDebug()<<__FUNCTION__<<this->thread();
    if (m_deviceInfo.empty()) return string();
    return toString(m_deviceInfo).toStdString();
}

void ZeYaoTebcoSDKPrivate::reconnect()
{
    qDebug()<<__FUNCTION__<<this->thread();
    if (m_pSerialPort->open(QIODevice::ReadOnly)) m_pTimer->stop();
}

void ZeYaoTebcoSDKPrivate::serialPortError(QSerialPort::SerialPortError error)
{
    qDebug()<<__FUNCTION__<<this->thread();
    if(error != QSerialPort::ResourceError && error != QSerialPort::PermissionError) return;
    if (m_pSerialPort->isOpen()) m_pSerialPort->close();
    if (!m_pTimer->isActive()) m_pTimer->start(1000);
}

void ZeYaoTebcoSDKPrivate::parsingMessages()
{
    qDebug()<<__FUNCTION__<<this->thread();
    auto data = m_pSerialPort->readAll();
    // 携带波形数据
    if (4 == data.size()) m_reportData.appendWaveformData(data);
    else m_reportData.appendWaveformData(data.mid(0, 4));
    // 携带数据
    if (6 != data.size()) return;
    bool ok;
    auto vdata = data.mid(4, 2).toHex().toUShort(&ok, 16);
    Value value;
    std::memcpy(&value, &vdata, sizeof(Value));
    m_reportData.appendData(value.type, value.data);
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
    qDebug()<<__FUNCTION__<<this->thread();
    if (QMqttClient::Connected != state) m_deviceInfo = QJsonObject();
}

void ZeYaoTebcoSDKPrivate::messageReceived(const QByteArray &message, const QMqttTopicName &topic)
{
    qDebug()<<__FUNCTION__<<this->thread();
    if (topic.levelCount() != 3 || message.isNull()) return;
    auto object = QJsonDocument::fromJson(message).object();
    auto level2 = SecondaryTopic(Basic::keyToValue<SecondaryTopic>(topic.levels().at(1)));
    if (SecondaryTopic::deviceInfo == level2) {
        m_client->unsubscribe(subTopic(m_deviceId));
        m_deviceInfo = object;
    }
}

void ZeYaoTebcoSDKPrivate::publish(const QMqttTopicName &topic, const QByteArray &message, quint8 qos, bool retain)
{
    qDebug()<<__FUNCTION__<<this->thread();
    m_client->publish(topic, message, qos, retain);
}

bool ZeYaoTebcoSDKPrivate::dataIntegrity() const
{
    return (m_jsonObject.count() >= 6);
}

QMqttTopicFilter ZeYaoTebcoSDKPrivate::subTopic(const QString &deviceId) const
{
    return QMqttTopicFilter(Basic::valueToLower(ResponseTopic::response) + "/+/" + deviceId);
}

template <class T>
QString ZeYaoTebcoSDKPrivate::toString(const T &qjson) const
{
    return QString(QJsonDocument(qjson).toJson(QJsonDocument::Compact));
}
