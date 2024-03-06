#include "icgcontrolmoduleprivate.h"
#include "icgcontrolmodule.h"

#include <QDebug>

IcgControlModulePrivate::IcgControlModulePrivate(QObject *parent)
    : QObject{parent}
    , m_startByte{0xff}
    , q_ptr{qobject_cast<IcgControlModule *>(parent)}
{
    m_pSerial = new QSerialPort(this);
    m_pSerial->setBaudRate(38400);
    m_pSerial->setDataBits(QSerialPort::Data8);
    m_pSerial->setParity(QSerialPort::NoParity);
    m_pSerial->setStopBits(QSerialPort::OneStop);
    connect(m_pSerial, &QSerialPort::readyRead, this, &IcgControlModulePrivate::received);
}

QStringList IcgControlModulePrivate::availablePorts() const
{
    QStringList names;
    foreach (auto info, QSerialPortInfo::availablePorts()) {
        names<<info.portName();
    }
    return names;
}

bool IcgControlModulePrivate::isOpen() const
{
    return m_pSerial->isOpen();
}

bool IcgControlModulePrivate::open(const QString &portName)
{
    close();
    m_pSerial->setPortName(portName);
    if (m_pSerial->open(QIODevice::ReadWrite)) {
        stopMeasuring();
    }
    return isOpen();
}

void IcgControlModulePrivate::close()
{
    if (isOpen()) {
        m_pSerial->clear();
        m_pSerial->close();
    }
}

void IcgControlModulePrivate::startMeasuring(const PatientData &data)
{
    writeCommand(SendType::SetPatientData, data);
}

void IcgControlModulePrivate::stopMeasuring()
{
    writeCommand(SendType::SetPatientData, PatientData(1, 1, 1, 1, 1));
}

void IcgControlModulePrivate::retrieve(const char &prop)
{
    writeCommand(SendType::RetrieveProp, prop);
}

void IcgControlModulePrivate::retrieveBsa()
{
    writeCommand(SendType::RetrieveProp, PatientProp::Bsa);
}

void IcgControlModulePrivate::retrieveVept()
{
    writeCommand(SendType::RetrieveProp, PatientProp::Vept);
}

void IcgControlModulePrivate::setBp(const BloodPressure &bp)
{
    writeCommand(SendType::SetBp, bp);
}

void IcgControlModulePrivate::setExtVal(const ExternalValues &extVal)
{
    writeCommand(SendType::SetExtMeasureVal, extVal);
}

void IcgControlModulePrivate::normalMode()
{
    writeCommand(SendType::OperatingMode, OpMode::Normal);
}

void IcgControlModulePrivate::powerDownMode()
{
    writeCommand(SendType::OperatingMode, OpMode::PowerDown);
}

void IcgControlModulePrivate::userCommand(const QByteArray &control_data)
{
    if (control_data.length() == 0 || !isOpen()) return;
    QByteArray comm;
    comm.append(static_cast<char>(m_startByte));
    comm.append(static_cast<char>(control_data.length() + 1));
    comm.append(control_data);
    comm.append(getChecksum(control_data.at(0), control_data.mid(1)));
    m_pSerial->write(comm);
}

void IcgControlModulePrivate::received()
{
    m_data.append(m_pSerial->readAll());
    auto index = m_data.indexOf(m_startByte);
    while (-1 != index) {  // 标记位存在
        m_data = m_data.mid(index); // 删除标记位前数据
        if (m_data.length() >= 4 && m_data.at(1) <= m_data.length() - 2) {  // 可能携带报文的数据
            uchar count = m_data.at(1); // 有效数据长度
            auto control = m_data.at(2);   // 控制字节
            auto bytes = m_data.mid(3, count - 2);  // 有效数据
            auto checksum = m_data.at(count + 1);   // 校验和
            if (checksum == getChecksum(control, bytes)) {
                parsingMsg(control, bytes);
            }
            m_data = m_data.mid(count + 2);
            index = m_data.indexOf(m_startByte);
        }
        else {
            break;
        }
    }
    if (m_data.length() > 127) m_data.clear();
}

void IcgControlModulePrivate::parsingMsg(const char &control, const QByteArray &data)
{
    switch (control) {
    case PacketType::SetPatientDataRet:
        if (data.length() == 1 && data.at(0) == 0x02) {   // ICG处于断电模式
            normalMode();
        }
    case PacketType::SetBpRet:
    case PacketType::AlertsMsg:
    case PacketType::SetExtMeasureValRet:
        if (data.length() == 1) {   // 错误代码
            emit q_ptr->error(control, data.at(0));
        }
        else {  // 未知错误代码
            emit q_ptr->error(control, 0xfd);
        }
        break;
    case PacketType::Waveform:
        if (data.length() == 4) {   // ecg icg
            emit q_ptr->icg(MsgType::hiLo(data.at(0), data.at(1)) - 8192);
            emit q_ptr->ecg(MsgType::hiLo(data.at(2), data.at(3)) - 8192);
        }
        else if (data.length() == 2) {  // icg
            emit q_ptr->icg(MsgType::hiLo(data.at(0), data.at(1)) - 8192);
        }
        else {  // 未知报文
            emit q_ptr->error(control, 0xfd);
        }
        break;
    case PacketType::Parameter:
        emit q_ptr->data(IcgControlModule::Standard, MsgType::standard(data));
        break;
    case PacketType::ExtParameter:
        emit q_ptr->data(IcgControlModule::Extended, MsgType::extended(data));
        break;
    case PacketType::RetrievePropRet:
        if (data.length() == 2 && data.at(1) != invalid_value) {
            if (data.at(0) == PatientProp::Bsa) {
                emit q_ptr->bsa(data.at(1) * 0.02);
            }
            else if (data.at(0) == PatientProp::Vept) {
                emit q_ptr->vept(data.at(1) * 0.1);
            }
            else {  // 未知报文
                emit q_ptr->error(control, 0xfd);
            }
        }
        else {  // 未知错误代码
            emit q_ptr->error(control, 0xfd);
        }
        break;
    default:    // 未知报文
        break;
    }
}

char IcgControlModulePrivate::getChecksum(const char &control, const QByteArray &data) const
{
    return ((control + std::accumulate(data.constBegin(), data.constEnd(), 0)) ^ 0xff) + 1;
}

template <class DataBytes>
void IcgControlModulePrivate::writeCommand(const char &control, const DataBytes &data) const
{
    if (!isOpen()) return;
    QByteArray dataBytes((char*)&data, sizeof(data));
    QByteArray comm;
    comm.append(static_cast<char>(m_startByte));
    comm.append(static_cast<char>(dataBytes.length() + 2));
    comm.append(control);
    comm.append(dataBytes);
    comm.append(getChecksum(control, dataBytes));
    m_pSerial->write(comm);
}
