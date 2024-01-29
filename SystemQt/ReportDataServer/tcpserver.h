#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QObject>
#include <QTcpServer>

#include "subtcpserver.h"

class TcpServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit TcpServer(QObject *parent = nullptr);
    virtual ~TcpServer();
signals:
    void sendSocketDescriptor(qintptr socketDescriptor, int index);
private:
    void incomingConnection(qintptr socketDescriptor) override;
private slots:
    void acceptErrorSlot(QAbstractSocket::SocketError socketError);
private:
    QList<SubTcpServer *> subList;
};

#endif // TCPSERVER_H
