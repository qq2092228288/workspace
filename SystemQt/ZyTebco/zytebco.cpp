#include "zytebco.h"
#include "DemoModeData.h"

struct Value
{
    ushort data : 12;
    ushort type : 4;
};

ZyTebco::ZyTebco(QObject *parent)
    :QObject{parent},working{false}
{
    m_pSerial = new QSerialPort(this);
    //串口配置
    m_pSerial->setParity(QSerialPort::NoParity);
    m_pSerial->setBaudRate(9600);
    m_pSerial->setDataBits(QSerialPort::Data8);
    m_pSerial->setStopBits(QSerialPort::OneStop);

    m_pWriteTimer = new QTimer(this);
    connect(m_pWriteTimer, &QTimer::timeout, this, &ZyTebco::writeF);

    m_pTimer = new QTimer(this);
    m_pDemoDataTimer = new QTimer(this);
    connect(m_pTimer, &QTimer::timeout, this, &ZyTebco::reConnect);
    connect(m_pDemoDataTimer, &QTimer::timeout, this, &ZyTebco::demoModeSlot);

    qRegisterMetaType<QSerialPort::SerialPortError>("SerialThread");
    connect(m_pSerial, &QSerialPort::errorOccurred, this, &ZyTebco::handleSerialError, Qt::UniqueConnection);
}

ZyTebco::~ZyTebco()
{
    if (m_pSerial->isOpen()) {
        m_pSerial->clear();
        m_pSerial->close();
    }
    delete m_pSerial;
}

void ZyTebco::stopTimer()
{
    if (m_pWriteTimer->isActive()) {
        m_pWriteTimer->stop();
    }
    if (m_pDemoDataTimer->isActive()) {
        m_pDemoDataTimer->stop();
    }
    if (m_pTimer->isActive()) {
        m_pTimer->stop();
    }
}

void ZyTebco::startWriteF()
{
    if (!m_pWriteTimer->isActive()) {
        m_pWriteTimer->start(5000);
    }
}

void ZyTebco::openSerial(const QString &portName)
{
    if (m_pSerial->portName() == portName && m_pSerial->isOpen()) {
        return;
    }
    m_pSerial->setPortName(portName);
    if (!m_pSerial->open(QIODevice::ReadWrite))  {
        emit openFailed();
    }
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

void ZyTebco::writeF()
{
    if (m_pSerial->isOpen()) {
        m_pSerial->write("F");
    }
}

void ZyTebco::handleSerialError(QSerialPort::SerialPortError error)
{
    // qDebug()<<error;
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
        emit ecgValue((uchar)array.at(0));
        emit diffValue((uchar)array.at(1));
        emit admitValue((uchar)array.at(2));
        emit waveform(array.mid(0, 4));
        if(array.size() == 6) {
            bool ok;
            auto vdata = array.mid(4, 2).toHex().toUShort(&ok, 16);
            Value value;
            std::memcpy(&value, &vdata, 2);

            auto values = map.value(value.type);
            values.enqueue(value.data);
            map.insert(value.type, values);
            emit data(value.type, values.average());
        }
    }
}


