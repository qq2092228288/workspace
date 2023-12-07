#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QObject>
#include <QTcpServer>

class TcpClientSocket;

class TcpServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit TcpServer(uint16_t port, int numConnection, QObject *parent = nullptr);
    virtual ~TcpServer();
private:
    void incomingConnection(qintptr socketDescriptor) override;
private slots:
    void acceptErrorSlot(QAbstractSocket::SocketError socketError);
    void breakLinkSlot(qintptr socketDescriptor);
private:
    QList<TcpClientSocket *> m_clientSocketList;
};

#endif // TCPSERVER_H
