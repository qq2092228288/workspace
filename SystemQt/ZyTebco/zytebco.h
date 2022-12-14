#ifndef ZYTEBCO_H
#define ZYTEBCO_H

#include <QtCore/qglobal.h>
#include <QObject>
#include <QSerialPort>
#include <QThread>
#include <QDebug>
#include <QSharedPointer>
#include <QVector>
#include <QTimer>
#include <QSerialPortInfo>
#include <QQueue>
#include <QMutexLocker>


#if defined(ZYTEBCO_LIBRARY)
#  define ZYTEBCO_EXPORT Q_DECL_EXPORT
#else
#  define ZYTEBCO_EXPORT Q_DECL_IMPORT
#endif




class ZYTEBCO_EXPORT ZyTebco : public QObject
{
    Q_OBJECT
public:
    explicit ZyTebco(QObject *parent = nullptr);
    virtual ~ZyTebco();
    QSerialPort *getSerial();
public slots:
    void openSerial(const QString &portName);
    void startCheckSlot(bool check);
    void clearSerial();
    bool isWorking();
    void startDemoMode(bool start);
signals:
    void data(uchar, double);
    void ecgValue(uchar);   // ECG
    void diffValue(uchar);  // dZ/dT
    void admitValue(uchar); // dZ
private slots:
    void handleSerialError(QSerialPort::SerialPortError error);
    void reConnect();
    void recvInfoSlot();
    void demoModeSlot();
private:
    QMutex mutex;
    QSerialPort *m_pSerial;
    QTimer *m_pTimer;
    bool working;
    QTimer *m_pDemoDataTimer;
};

#endif // ZYTEBCO_H
