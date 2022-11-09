#include "zytebco.h"
#include "DemoModeData.h"


ZyTebco::ZyTebco(QObject *parent)
    :QObject{parent},working{false}
{
    m_pSerial = new QSerialPort(this);
    //串口配置
    m_pSerial->setParity(QSerialPort::NoParity);
    m_pSerial->setBaudRate(9600);
    m_pSerial->setDataBits(QSerialPort::Data8);
    m_pSerial->setStopBits(QSerialPort::OneStop);

    m_pTimer = new QTimer(this);
    m_pDemoDataTimer = new QTimer(this);
    connect(m_pTimer, &QTimer::timeout, this, &ZyTebco::reConnect);
    connect(m_pDemoDataTimer, &QTimer::timeout, this, &ZyTebco::demoModeSlot);
}

ZyTebco::~ZyTebco()
{
    if (m_pDemoDataTimer->isActive()) {
        m_pDemoDataTimer->stop();
    }
    if (m_pTimer->isActive()) {
        m_pTimer->stop();
    }
    if (m_pSerial->isOpen()) {
        m_pSerial->clear();
        m_pSerial->close();
    }
    delete m_pDemoDataTimer;
    delete m_pTimer;
    delete m_pSerial;
//    qDebug()<<"~ZyTebco()";
}

QSerialPort *ZyTebco::getSerial()
{
    return m_pSerial;
}

void ZyTebco::openSerial(const QString &portName)
{
    m_pSerial->setPortName(portName);
    //若串口已打开，则关闭
    m_pSerial->open(QIODevice::ReadWrite);
    qRegisterMetaType<QSerialPort::SerialPortError>("SerialThread");
    connect(m_pSerial,&QSerialPort::errorOccurred,this,&ZyTebco::handleSerialError);
}

void ZyTebco::startCheckSlot(bool check)
{
    if(check) {
        connect(m_pSerial,&QSerialPort::readyRead,this,&ZyTebco::recvInfoSlot,Qt::UniqueConnection);
        working = true;
    }
    else {
        disconnect(m_pSerial,&QSerialPort::readyRead,this,&ZyTebco::recvInfoSlot);
        working = false;
    }
}

void ZyTebco::clearSerial()
{
    if (m_pSerial->isOpen()) {
        m_pSerial->clear();
    }
}

bool ZyTebco::isWorking()
{
    return working;
}

void ZyTebco::startDemoMode(bool start)
{
    if (start) {
        if (!m_pDemoDataTimer->isActive()) {
            m_pDemoDataTimer->start(20);
        }
    }
    else {
        if (m_pDemoDataTimer->isActive()) {
            m_pDemoDataTimer->stop();
        }
    }
}

void ZyTebco::handleSerialError(QSerialPort::SerialPortError error)
{
    qDebug()<<error;
    if(error == QSerialPort::ResourceError || error == QSerialPort::PermissionError) {
        m_pSerial->close();
        m_pTimer->start(1000);
    }
}

void ZyTebco::reConnect()
{
    foreach (auto info, QSerialPortInfo::availablePorts()) {
        if(info.portName() == m_pSerial->portName()) {
            m_pTimer->stop();
            m_pSerial->open(QIODevice::ReadWrite);
        }
    }
}
//#include <QDateTime>
void ZyTebco::recvInfoSlot()
{
    QByteArray array = m_pSerial->readAll();
    if (array.size() == 4 || array.size() == 6) {
        emit ecgValue((uchar)array.at(0));
        emit diffValue((uchar)array.at(1));
        emit admitValue((uchar)array.at(2));
        if(array.size() == 6) {
            bool ok;
            ushort argVal = array.mid(4,2).toHex().toUShort(&ok,16);
            emit data(argVal>>12,argVal&0xFFF);
        }
    }
//    qDebug()<<array.toHex();
}

void ZyTebco::demoModeSlot()
{
    QMutexLocker locker(&mutex);
    if (INDEX >= SIZE) {
        INDEX = 0;
    }
    qint64 value = demoData[INDEX];
    if (value < 0xFFFFFFFF) {
        emit ecgValue(uchar(value>>24));
        emit diffValue(uchar((value&0x00FF0000)>>16));
        emit admitValue(uchar((value&0x0000FF00)>>8));
    }
    else {
        emit data((value&0xF000)>>12,value&0xFFF);
    }
    ++INDEX;
}

