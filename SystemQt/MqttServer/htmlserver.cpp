#include "htmlserver.h"
#include "singleton.h"
#include "htmlclient.h"

HtmlServer::HtmlServer(QObject *parent)
    : QObject{parent},
      server_ptr{new QWebSocketServer(QStringLiteral("Html Server"), QWebSocketServer::NonSecureMode)},
      clientWrapper_ptr{new WebSocketClientWrapper(server_ptr.get())}
{
    connect(clientWrapper_ptr.get(), &WebSocketClientWrapper::clientConnected, this, &HtmlServer::newClientConnected);
}

void HtmlServer::startListening()
{
    if (!server_ptr->listen(QHostAddress::Any, Singleton::listenPort())) {
        qFatal("Failed to open web socket server.");
        exit(1) ;
    }
    else {
        TIME_DEBUG()<<"The server is listening on port: "<<Singleton::listenPort();
    }
}

void HtmlServer::newClientConnected(WebSocketTransport *client)
{
    HtmlClient *newClient = new HtmlClient(client, this);
    connect(client, &WebSocketTransport::destroyed, newClient, &HtmlClient::deleteLater);
}
