#ifndef HTMLSERVER_H
#define HTMLSERVER_H

#include <QObject>
#include <QWebSocketServer>
#include <QWebChannel>
#include <QSharedPointer>
#include <QXmlStreamReader>
#include <QFile>
#include <QSqlRecord>
#include <QSqlQuery>

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
    enum class DBOperationError
    {
        NoError,
        SelectError,
        InsertError,
        UpdateError,
        RemoveEroor
    };
    Q_ENUM(DBOperationError)
public slots:
    void startListening();
public slots:
    /*These slots are invoked from the HTML client side and received by the server.*/
    void loginUi();
    void login(const QString &adminId, const QString &password);
    void currentTable(const QJsonObject &object, const QString &table);
    void execSqlStatement(const QJsonObject &object, const QString &tableName, const QString &statement);
signals:
    /*These signal are emitted from the C++ side and received by the HTML client side.*/
    void sendText(const QString &text);
    void sendUserInfo(const QJsonObject &userInfo);
private:
    bool userInfoCheck(const QJsonObject &userInfo);
    void mainUi(const QJsonObject &object, QString tableName);
    static QStringList columnNames(const QSqlRecord &record);
    QString tableDivs(const QStringList &tableNames);
    QString dataList(QSqlQuery &sqlQuery, const int &permission, bool show);
    QString xmlToJsonString(const QString &xmlPath);
    QString htmlJsonString(const QString &data);
private:
    QWebSocketServer_PTR server_ptr;
    WebSocketClientWrapper_PTR clientWrapper_ptr;
    QWebChannel_PTR channel_ptr;
};
typedef QSharedPointer<HtmlServer> HtmlServer_PTR;

#endif // HTMLSERVER_H
