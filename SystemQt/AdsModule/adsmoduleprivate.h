#ifndef ADSMODULEPRIVATE_H
#define ADSMODULEPRIVATE_H

#include <QObject>
#include <QSerialPort>

class AdsModule;
class AdsModulePrivate : public QObject
{
    Q_OBJECT
public:
    explicit AdsModulePrivate(QObject *q);
public:
    bool open(const QString &portName);
    void close();
    QStringList portNames() const;
private Q_SLOTS:
    void receivedData();
private:
    bool dataCheck(const QByteArray &frame, const QByteArray &header);
    template <class T>
    void sendLeadWireData(const QByteArray &data, const QList<T> &types, bool sendOther = false);
private:
    QSerialPort *serialPort;
    QByteArray data;
private:
    AdsModule *q_ptr;
};

#endif // ADSMODULEPRIVATE_H
