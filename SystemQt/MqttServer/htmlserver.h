#ifndef HTMLSERVER_H
#define HTMLSERVER_H

#include <QObject>
#include <QWebSocketServer>
#include <QSharedPointer>
#include <websocketclientwrapper.h>

class HtmlClient;

class HtmlServer : public QObject
{
    Q_OBJECT
public:
    explicit HtmlServer(QObject *parent = nullptr);
public slots:
    void startListening();
private slots:
    void newClientConnected(WebSocketTransport *client);
    void clientDestroyed(QObject *obj);
private:
    QWebSocketServer *server;
    WebSocketClientWrapper *clientWrapper;
    QMap<QObject *, HtmlClient *> map;
};
typedef QSharedPointer<HtmlServer> HtmlServer_PTR;

#endif // HTMLSERVER_H
