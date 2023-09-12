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
    /***************************/
//    m_pExtSerial = new QextSerialPort(QextSerialPort::QueryMode::EventDriven, this);
//    m_pExtSerial->setParity(ParityType::PAR_NONE);
//    m_pExtSerial->setBaudRate(BaudRateType::BAUD9600);
//    m_pExtSerial->setDataBits(DataBitsType::DATA_8);
//    m_pExtSerial->setStopBits(StopBitsType::STOP_1);
//    m_pExtSerial->setPortName("COM0");
    /**************************/


    m_pTimer = new QTimer(this);
    m_pDemoDataTimer = new QTimer(this);
    connect(m_pTimer, &QTimer::timeout, this, &ZyTebco::reConnect);
    connect(m_pDemoDataTimer, &QTimer::timeout, this, &ZyTebco::demoModeSlot);

    qRegisterMetaType<QSerialPort::SerialPortError>("SerialThread");
    connect(m_pSerial, &QSerialPort::errorOccurred, this, &ZyTebco::handleSerialError, Qt::UniqueConnection);
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
//    if (m_pExtSerial->isOpen()) {
//        m_pExtSerial->close();
//        qDebug()<<__LINE__;
//    }
    delete m_pDemoDataTimer;
    delete m_pTimer;
    delete m_pSerial;
//    delete m_pExtSerial;
//    qDebug()<<__FUNCTION__;
}

//QSerialPort *ZyTebco::getSerial()
//{
//    return m_pSerial;
//}

void ZyTebco::openSerial(const QString &portName)
{
    if (m_pSerial->portName() == portName && m_pSerial->isOpen()) {
        return;
    }
    m_pSerial->setPortName(portName);
    if (!m_pSerial->open(QIODevice::ReadWrite))  {
        emit openFailed();
    }
//    if (m_pExtSerial->portName() == portName && m_pExtSerial->isOpen()) {
//        return ;
//    }
//    m_pExtSerial->setPortName(portName);
//    if (!m_pExtSerial->open(QIODevice::ReadWrite)) {
//        emit openFailed();
//    }
//    qDebug()<<m_pExtSerial->isOpen();
}

void ZyTebco::startCheckSlot(bool check)
{
    if(check) {
        connect(m_pSerial, &QSerialPort::readyRead, this, &ZyTebco::recvInfoSlot, Qt::UniqueConnection);
        working = true;
    }
    else {
        disconnect(m_pSerial, &QSerialPort::readyRead, this, &ZyTebco::recvInfoSlot);
        working = false;
    }
//    if(check) {
//        connect(m_pExtSerial, &QSerialPort::readyRead, this, &ZyTebco::recvInfoSlot, Qt::UniqueConnection);
//        working = true;
//    }
//    else {
//        disconnect(m_pExtSerial, &QSerialPort::readyRead, this, &ZyTebco::recvInfoSlot);
//        working = false;
//    }
}

void ZyTebco::clearSerial()
{
    if (m_pSerial->isOpen()) {
        m_pSerial->clear();
    }
    map.clear();
}

bool ZyTebco::isWorking()
{
    return working;
}

//void ZyTebco::hotPlug(bool insert)
//{
//    qDebug()<<m_pSerial->portName()<<insert;
//    bool removal = true;
//    foreach (auto info, QSerialPortInfo::availablePorts()) {
//        if(info.portName() == m_pSerial->portName()) {
//            if (insert) {
//                if (!m_pSerial->isOpen()) {
//                    m_pSerial->open(QIODevice::ReadWrite);
//                }
//                return;
//            }
//            else {
//                removal = false;    // 非指定设备移除
//            }
//            break;
//        }
//    }
//    if (m_pSerial->isOpen() && removal) {
//        m_pSerial->close();
//    }
//    qDebug()<<m_pExtSerial->portName()<<insert;
//    bool removal = true;
//    foreach (auto info, QSerialPortInfo::availablePorts()) {
//        if(info.portName() == m_pExtSerial->portName()) {
//            if (insert) {
//                if (!m_pExtSerial->isOpen()) {
//                    m_pExtSerial->open(QIODevice::ReadWrite);
//                }
//                return;
//            }
//            else {
//                removal = false;    // 非指定设备移除
//            }
//            break;
//        }
//    }
//    if (m_pExtSerial->isOpen() && removal) {
//        m_pExtSerial->close();
//    }
//}

void ZyTebco::startDemoMode(bool start)
{
    if (start) {
        if (!m_pDemoDataTimer->isActive()) {
            m_pDemoDataTimer->start(20);
        }
        working = true;
    }
    else {
        if (m_pDemoDataTimer->isActive()) {
            m_pDemoDataTimer->stop();
        }
        working = false;
    }
}

void ZyTebco::clearMap()
{
    map.clear();
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

void ZyTebco::recvInfoSlot()
{
    processingData(m_pSerial->readAll());
}

void ZyTebco::demoModeSlot()
{
    QMutexLocker locker(&mutex);
    if (INDEX >= SIZE) {
        INDEX = 0;
    }
    qint64 value = demoData[INDEX];
//    if (value < 0xFFFFFFFF) {
//        emit ecgValue(uchar(value>>24));
//        emit diffValue(uchar((value&0x00FF0000)>>16));
//        emit admitValue(uchar((value&0x0000FF00)>>8));
//    }
//    else {
//        emit data((value&0xF000)>>12,value&0xFFF);
//    }
    QByteArray array;
    if (value < 0xFFFFFFFF) {
        array.resize(4);
        memcpy(array.data(), &value, 4);
        array.reserve(4);
    }
    else {
        array.resize(6);
        memcpy(array.data(), &value, 6);
    }
    std::reverse(array.begin(), array.end());
    processingData(array);
    ++INDEX;
}

void ZyTebco::processingData(const QByteArray &array)
{
    if (array.size() == 4 || array.size() == 6) {
        bool ok;
        emit ecgValue((uchar)array.at(0));
        emit diffValue((uchar)array.at(1));
        emit admitValue((uchar)array.at(2));
        emit waveform(array.mid(0, 4));
        if(array.size() == 6) {
            ushort argVal = array.mid(4,2).toHex().toUShort(&ok, 16);
            uchar type = argVal>>12;
            auto values = map.value(type);
            values.enqueue(argVal&0xFFF);
            map.insert(type, values);
            emit data(type, values.average());
        }
    }
}

