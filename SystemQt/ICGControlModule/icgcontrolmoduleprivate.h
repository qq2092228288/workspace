#ifndef ICGCONTROLMODULEPRIVATE_H
#define ICGCONTROLMODULEPRIVATE_H

#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>

#include "msgtype.h"

class IcgControlModule;

class IcgControlModulePrivate : public QObject
{
    Q_OBJECT
public:
    explicit IcgControlModulePrivate(QObject *parent = nullptr);
    enum PatientProp : char
    {
        Bsa = 0x00,
        Vept = 0x01
    };
    Q_ENUM(PatientProp)
    enum OpMode : char
    {
        Normal = 0x00,
        PowerDown = 0x01
    };
    Q_ENUM(OpMode)
    enum SendType : uchar
    {
        SetBp = 0x04,
        RetrieveDeviceInfo = 0x08,
        SetPatientData = 0x10,
        RetrieveProp = 0x14,
        SetExtMeasureVal = 0x0b,
        OperatingMode = 0x0d,
        ModuleInit = 0x0e
    };
    Q_ENUM(SendType)
    enum PacketType : uchar
    {
        SetPatientDataRet = 0x02,
        Parameter = 0x03,
        SetBpRet = 0x05,
        AlertsMsg = 0x06,
        Waveform = 0x07,
        SetExtMeasureValRet = 0x0c,
        ExtParameter = 0x13,
        RetrievePropRet = 0x15
    };
    Q_ENUM(PacketType)
public:
    QStringList availablePorts() const;
    bool isOpen() const;
    bool open(const QString &portName);
    void close();
    void startMeasuring(const PatientData &data);
    void stopMeasuring();
    void retrieve(const char &prop);
    void retrieveBsa();
    void retrieveVept();
    void setBp(const BloodPressure &bp);
    void setExtVal(const ExternalValues &extVal);
    void normalMode();
    void powerDownMode();
    void userCommand(const QByteArray &control_data);
private slots:
    void received();
private:
    void parsingMsg(const char &control, const QByteArray &data);
    char getChecksum(const char &control, const QByteArray &data) const;
    template <class DataBytes>
    void writeCommand(const char &control, const DataBytes &data) const;
private:
    QSerialPort *m_pSerial;
    QByteArray m_data;
    const uchar m_startByte;
private:
    IcgControlModule *q_ptr;
};

#endif // ICGCONTROLMODULEPRIVATE_H
