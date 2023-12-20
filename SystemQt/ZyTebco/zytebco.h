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

// 记录值队列
class VQueue : public QQueue<short>
{
public:
    explicit VQueue(int maxcount = 80) : maxcount(maxcount) {}
    void enqueue(const short &d)
    {
        if (length() >= maxcount) QQueue::dequeue();
        QQueue<short>::enqueue(d);
    }
    void append(const short &d)
    {
        enqueue(d);
    }
    short average() const
    {
        return sum()/length();
    }
    short sum() const
    {
        return std::accumulate(QQueue<short>::constBegin(), QQueue<short>::constEnd(), 0);
    }
private:
    int maxcount;
};

class ZYTEBCO_EXPORT ZyTebco : public QObject
{
    Q_OBJECT
public:
    explicit ZyTebco(QObject *parent = nullptr);
    virtual ~ZyTebco();
public slots:
    void stopTimer();
    void startWriteF();
    void openSerial(const QString &portName);
    void startCheckSlot(bool check);
    void clearSerial();
    bool isWorking();
    void startDemoMode(bool start);
    void clearMap();
signals:
    void data(uchar, short);
    void ecgValue(uchar);   // ECG
    void diffValue(uchar);  // dZ/dT
    void admitValue(uchar); // dZ
    void waveform(QByteArray);// 波形数据
    void openFailed();
private slots:
    void writeF();
    void handleSerialError(QSerialPort::SerialPortError error);
    void reConnect();
    void recvInfoSlot();
    void demoModeSlot();
private:
    void processingData(const QByteArray &array);
private:
    QMutex mutex;
    QSerialPort *m_pSerial;
    QTimer *m_pWriteTimer;
    QTimer *m_pTimer;
    bool working;
    QTimer *m_pDemoDataTimer;
    QMap<uchar, VQueue> map;
};

#endif // ZYTEBCO_H
