#include "htmlserver.h"
#include <singleton.h>
#include <threadservice.h>
#include "htmlclient.h"

HtmlServer::HtmlServer(QObject *parent)
    : QObject{parent}
{
    server = new QWebSocketServer(QStringLiteral("Html Server"), QWebSocketServer::NonSecureMode, this);
    clientWrapper = new WebSocketClientWrapper(server, this);
    connect(clientWrapper, &WebSocketClientWrapper::clientConnected, this, &HtmlServer::newClientConnected);
}

void HtmlServer::startListening()
{
    if (!server->listen(QHostAddress::Any, Singleton::listenPort())) {
        qFatal("Failed to open web socket server.");
        exit(1);
    }
    else {
        TIME_DEBUG()<<"The server is listening on port: "<<Singleton::listenPort();
    }
}

void HtmlServer::newClientConnected(WebSocketTransport *client)
{
    HtmlClient *newClient = new HtmlClient(client);
    connect(client, &WebSocketTransport::destroyed, this, &HtmlServer::clientDisconnected);
    ThreadService::getInstance()->objectMoveToThread(newClient);
    map.insert(client, newClient);
}

void HtmlServer::clientDisconnected(QObject *obj)
{
    auto client = map.value(obj);
    if (client != nullptr) {
        map.remove(obj);
        delete client;
        client = nullptr;
    }
}
