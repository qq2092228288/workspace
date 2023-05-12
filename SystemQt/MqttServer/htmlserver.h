#ifndef HTMLSERVER_H
#define HTMLSERVER_H

#include <QObject>
#include <QWebSocketServer>
#include <QWebChannel>
#include <QSharedPointer>

#include <websocketclientwrapper.h>
#include <websockettransport.h>

typedef QSharedPointer<QWebSocketServer> QWebSocketServer_PTR;
typedef QSharedPointer<WebSocketClientWrapper> WebSocketClientWrapper_PTR;
typedef QSharedPointer<QWebChannel> QWebChannel_PTR;

class HtmlServer : public QObject
{
    Q_OBJECT
public:
    explicit HtmlServer(QObject *parent = nullptr);
public slots:
    void startListening();
public slots:
    /*These slots are invoked from the HTML client side and received by the server.*/
    void loginUi();
    void login(const QString &adminId, const QString &password);
signals:
    /*These signal are emitted from the C++ side and received by the HTML client side.*/
    void sendText(const QString &text);
private:
    QWebSocketServer_PTR server_ptr;
    WebSocketClientWrapper_PTR clientWrapper_ptr;
    QWebChannel_PTR channel_ptr;
};
typedef QSharedPointer<HtmlServer> HtmlServer_PTR;

#endif // HTMLSERVER_H
