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
public slots:
    void writeReady(qintptr socketDescriptor, const QByteArray &data);
signals:
    void send(qintptr socketDescriptor, TelegramType type, const QJsonObject &data);
private slots:
    void dataReceived();
private:
    qintptr m_socketDescriptor;
    QByteArray m_data;
};

#endif // TCPCLIENTSOCKET_H
