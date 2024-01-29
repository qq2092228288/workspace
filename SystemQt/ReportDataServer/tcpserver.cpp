#include "tcpserver.h"
#include "singleton.h"
#include "threadservice.h"
#include "config.h"

#include <algorithm>

auto cmp(SubTcpServer *a, SubTcpServer *b)
{
    return a->count() < b->count();
}

TcpServer::TcpServer(QObject *parent)
    : QTcpServer{parent}
{
    setMaxPendingConnections(MAX_NUM_CONNECTION);
    connect(this, &TcpServer::acceptError, this, &TcpServer::acceptErrorSlot);
    if (listen(QHostAddress::Any, LISTEN_PORT)) {
        TIME_DEBUG()<<QString("listen to port %1 ...").arg(LISTEN_PORT);
        for (int index = 0; index < MAX_NUM_CONNECTION / SUB_SERVER_MAX; ++index) {
            auto sub = new SubTcpServer(index);
            ThreadService::getInstance()->objectMoveToThread(sub);
            connect(this, &TcpServer::sendSocketDescriptor, sub, &SubTcpServer::newSocketDescriptor);
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
    auto sub = *std::min_element(subList.constBegin(), subList.constEnd(), cmp);
    if (sub->count() < SUB_SERVER_MAX) {
        emit sendSocketDescriptor(socketDescriptor, sub->index());
    }
    else {
        // server full
        auto client = new QTcpSocket(this);
        client->setSocketDescriptor(socketDescriptor);
        connect(client, &TcpClientSocket::bytesWritten, client, &TcpClientSocket::deleteLater);
        client->write(TelegramProfile(TelegramType::ServerFull, QByteArray()).toByteArray());
    }
}

void TcpServer::acceptErrorSlot(QAbstractSocket::SocketError socketError)
{
    TIME_DEBUG()<<" socket error: "<<socketError;
}
