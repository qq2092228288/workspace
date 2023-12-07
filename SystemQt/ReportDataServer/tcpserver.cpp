#include "tcpserver.h"
#include "tcpclientsocket.h"
#include "singleton.h"
#include "databasesingleton.h"
#include <QThread>

TcpServer::TcpServer(uint16_t port, int numConnection, QObject *parent)
    : QTcpServer{parent}
{
    setMaxPendingConnections(numConnection);
    connect(this, &TcpServer::acceptError, this, &TcpServer::acceptErrorSlot);
    if (listen(QHostAddress::Any, port)) {
        TIME_DEBUG()<<QString("listen to port %1 ...").arg(port);
        DatabaseSingleton::getInstance()->openDatabase();
    }
    else {
        TIME_DEBUG()<<QString("failed to listen to port %1").arg(port);
    }
}

TcpServer::~TcpServer()
{

}

void TcpServer::incomingConnection(qintptr socketDescriptor)
{
    auto client = new TcpClientSocket(socketDescriptor);
    connect(client, &TcpClientSocket::breakLink, this, &TcpServer::breakLinkSlot);
    m_clientSocketList.append(client);
}

void TcpServer::acceptErrorSlot(QAbstractSocket::SocketError socketError)
{
    TIME_DEBUG()<<" socket error: "<<socketError;
}

void TcpServer::breakLinkSlot(qintptr socketDescriptor)
{
    for (int i = 0; i < m_clientSocketList.count(); ++i) {
        auto client = m_clientSocketList.at(i);
        if (client->socketDescriptor() == socketDescriptor) {
            m_clientSocketList.removeAt(i);
            return;
        }
    }
}
