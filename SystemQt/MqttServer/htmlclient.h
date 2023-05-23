#ifndef HTMLCLIENT_H
#define HTMLCLIENT_H

#include <QObject>
#include <QFile>
#include <QSqlRecord>
#include <QWebChannel>
#include <QSharedPointer>
#include <websockettransport.h>

typedef QSharedPointer<QWebChannel> QWebChannel_PTR;

class HtmlClient : public QObject
{
    Q_OBJECT
public:
    explicit HtmlClient(WebSocketTransport *client, QObject *parent = nullptr);
public slots:
    /*These slots are invoked from the HTML client side and received by the server.*/
    void loginUi();
    void login(const QString &adminId, const QString &password);
    void currentTable(const QJsonObject &object, const QString &tableName);
    void execSqlStatement(const QJsonObject &object, const QString &tableName, const QString &statement);
signals:
    /*These signal are emitted from the C++ side and received by the HTML client side.*/
    void sendText(const QString &text);
    void sendUserInfo(const QJsonObject &userInfo);
    void sendReport(const QJsonObject &report);
private:
    bool userInfoCheck(const QJsonObject &userInfo);
    void mainUi(const QJsonObject &object, const QString &tableName);
    static QStringList columnNames(const QSqlRecord &record);
    QString xmlToJsonString(const QString &xmlPath);
    QString htmlJsonString(const QString &data);
private:
    QWebChannel_PTR channel_ptr;
};

#endif // HTMLCLIENT_H
