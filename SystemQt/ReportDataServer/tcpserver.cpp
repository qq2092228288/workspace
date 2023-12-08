#include "tcpserver.h"
#include "singleton.h"
#include "threadservice.h"
#include "config.h"

TcpServer::TcpServer(QObject *parent)
    : QTcpServer{parent}
{
    setMaxPendingConnections(MAX_NUM_CONNECTION);
    connect(this, &TcpServer::acceptError, this, &TcpServer::acceptErrorSlot);
    if (listen(QHostAddress::Any, LISTEN_PORT)) {
        TIME_DEBUG()<<QString("listen to port %1 ...").arg(LISTEN_PORT);
        for (int i = 0; i < MAX_NUM_CONNECTION / SUB_SERVER_MAX + 1; ++i) {
            auto sub = new SubTcpServer;
            ThreadService::getInstance()->objectMoveToThread(sub);
            subList.append(sub);
        }
    }
    else {
        TIME_DEBUG()<<QString("failed to listen to port %1").arg(LISTEN_PORT);
    }
}

TcpServer::~TcpServer()
{
    qDeleteAll(subList.begin(), subList.end());
}

void TcpServer::incomingConnection(qintptr socketDescriptor)
{
    foreach (auto sub, subList) {
        disconnect(this, &TcpServer::sendSocketDescriptor, sub, &SubTcpServer::newSocketDescriptor);
    }
    foreach (auto sub, subList) {
        if (sub->count() < SUB_SERVER_MAX) {
            connect(this, &TcpServer::sendSocketDescriptor, sub, &SubTcpServer::newSocketDescriptor);
            break;
        }
    }
    emit sendSocketDescriptor(socketDescriptor);
}

void TcpServer::acceptErrorSlot(QAbstractSocket::SocketError socketError)
{
    TIME_DEBUG()<<" socket error: "<<socketError;
}
