#ifndef HTMLSERVER_H
#define HTMLSERVER_H

#include <QObject>
#include <QWebSocketServer>
#include <QSharedPointer>
#include <websocketclientwrapper.h>

typedef QSharedPointer<QWebSocketServer> QWebSocketServer_PTR;
typedef QSharedPointer<WebSocketClientWrapper> WebSocketClientWrapper_PTR;

class HtmlServer : public QObject
{
    Q_OBJECT
public:
    explicit HtmlServer(QObject *parent = nullptr);
public slots:
    void startListening();
private slots:
    void newClientConnected(WebSocketTransport *client);
private:
    QWebSocketServer_PTR server_ptr;
    WebSocketClientWrapper_PTR clientWrapper_ptr;
};
typedef QSharedPointer<HtmlServer> HtmlServer_PTR;

#endif // HTMLSERVER_H
