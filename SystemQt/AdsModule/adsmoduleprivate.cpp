#include "adsmoduleprivate.h"
#include "adsmodule.h"
#include <QDataStream>
#include <QSerialPortInfo>
#include <QDebug>

typedef AdsModule::MessageDataType MessageDataType;

typedef struct MessageDataLength
{
    static const qint8 header = 3;
    static const qint8 length = 1;
    static const qint8 leadwire = 4;
    static const qint8 detsign = 2;
    static const qint8 incTestData = 1;
    static const qint8 checksums = 1;
    static const qint8 addcheck = 1;
}MsgDLen;

QList<MessageDataType> FLWType = QList<MessageDataType>()
        <<MessageDataType::LeadWireI
        <<MessageDataType::LeadWireII
        <<MessageDataType::LeadWireV1
        <<MessageDataType::LeadWireV2
        <<MessageDataType::LeadWireV3
        <<MessageDataType::LeadWireV4
        <<MessageDataType::LeadWireV5
        <<MessageDataType::LeadWireV6;
QList<MessageDataType> SLWType = QList<MessageDataType>()
        <<MessageDataType::LeadWireIII
        <<MessageDataType::LeadWireAVR
        <<MessageDataType::LeadWireAVL
        <<MessageDataType::LeadWireAVF;

const int FLen = MsgDLen::header
        + MsgDLen::length
        + FLWType.length() * MsgDLen::leadwire
        + MsgDLen::checksums
        + MsgDLen::addcheck;

const int SLen = MsgDLen::header
        + MsgDLen::length
        + SLWType.length() * MsgDLen::leadwire
        + MsgDLen::detsign
        + MsgDLen::incTestData
        + MsgDLen::checksums
        + MsgDLen::addcheck;

QByteArray FHeader = QByteArray::number(0xAAFFF120, 16);

QByteArray SHeader = QByteArray::number(0xAAFFF213, 16);

const int MsgLen = FLen + SLen;

AdsModulePrivate::AdsModulePrivate(QObject *q)
    : QObject{q},
      q_ptr{qobject_cast<AdsModule *>(q)}
{
    qRegisterMetaType<AdsModule::MessageDataType>();
    qRegisterMetaType<AdsModule::MessageError>();
    qRegisterMetaType<QSerialPort::SerialPortError>();

    serialPort = new QSerialPort(this);
    serialPort->setParity(QSerialPort::NoParity);
    serialPort->setBaudRate(460800);
    serialPort->setDataBits(QSerialPort::Data8);
    serialPort->setStopBits(QSerialPort::OneStop);

    connect(serialPort, &QSerialPort::readyRead, this, &AdsModulePrivate::receivedData);
    connect(serialPort, &QSerialPort::errorOccurred, this, [=](QSerialPort::SerialPortError error) {
        if (error != QSerialPort::NoError) {
            emit q_ptr->connectFailed();
        }
    });
}

bool AdsModulePrivate::open(const QString &portName)
{
    close();
    serialPort->setPortName(portName);
    return serialPort->open(QIODevice::ReadWrite);
}

void AdsModulePrivate::close()
{
    if (serialPort->isOpen()) {
        serialPort->clear();
        serialPort->close();
    }
}

QStringList AdsModulePrivate::portNames() const
{
    QStringList names;
    foreach (auto info, QSerialPortInfo::availablePorts()) {
        names<<info.portName();
    }
    return names;
}

void AdsModulePrivate::receivedData()
{
    data.append(serialPort->readAll());
    auto index = data.toHex().indexOf(FHeader)/2;
    while (-1 != index) {
        auto bytes = data.mid(index, MsgLen);
        if (bytes.size() == MsgLen) {
            data = data.mid(index + MsgLen);
            index = data.indexOf(FHeader);
            auto frame1 = bytes.mid(0, FLen);
            if (dataCheck(frame1, FHeader)) {
                sendLeadWireData(frame1, FLWType);
            }
            auto frame2 = bytes.mid(FLen);
            if (dataCheck(frame2, SHeader)) {
                sendLeadWireData(frame2, SLWType, true);
            }
        }
        else {
            break;
        }
    }
}

bool AdsModulePrivate::dataCheck(const QByteArray &frame, const QByteArray &header)
{
    if (frame.toHex().indexOf(header) != 0) {
        emit q_ptr->invalid(AdsModule::FrameHeaderError, frame);
        return false;
    }
    qint32 checksums = 0;
    qint32 addcheck = 0;
    const int length = frame.size() - MsgDLen::checksums - MsgDLen::addcheck;
    for (int i = 0; i < length; ++i) {
        checksums += frame.at(i);
        addcheck += checksums;
    }
    if ((checksums & 0xFF) != quint8(frame.at(length))) {
        emit q_ptr->invalid(AdsModule::ChecksumsError, frame);
        return false;
    }
    if ((addcheck & 0xFF) != quint8(frame.at(length + MsgDLen::checksums))) {
        emit q_ptr->invalid(AdsModule::AddCheckError, frame);
        return false;
    }
    return true;
}

template<class T>
void AdsModulePrivate::sendLeadWireData(const QByteArray &data, const QList<T> &types, bool sendOther)
{
    QDataStream out(data);
    out.setByteOrder(QDataStream::LittleEndian);
    out.skipRawData(MsgDLen::header + MsgDLen::length);
    foreach (auto type, types) {
        qint32 temp = 0;
        out>>temp;
        emit q_ptr->newdata(type, temp*0.2861/6000);
    }
    if (sendOther) {
        quint16 sign = 0;
        out>>sign;
        if (sign != 0) {
            emit q_ptr->newdata(MessageDataType::DetSign, sign);
        }
    }
}
