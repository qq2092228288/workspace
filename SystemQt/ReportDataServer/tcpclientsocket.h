#ifndef TCPCLIENTSOCKET_H
#define TCPCLIENTSOCKET_H

#include <QObject>
#include <QTcpSocket>
#include "telegramprofile.h"

class TcpClientSocket : public QTcpSocket
{
    Q_OBJECT
public:
    explicit TcpClientSocket(qintptr socketDescriptor, QObject *parent = nullptr);
    virtual ~TcpClientSocket();
signals:
    void breakLink(qintptr);
private slots:
    void disconnectedSlot();
    void dataReceived();
private:
    void handling(TelegramType type, const QJsonObject &data);
    void writeReady(TelegramType type, const QJsonObject &json);
private:
    QByteArray m_data;
};

#endif // TCPCLIENTSOCKET_H
