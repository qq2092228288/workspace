#ifndef HTMLCLIENT_H
#define HTMLCLIENT_H

#include <QObject>
#include <QWebChannel>
#include <websockettransport.h>
#include <QJsonObject>


typedef QSharedPointer<QWebChannel> QWebChannel_PTR;

class HtmlClient : public QObject
{
    Q_OBJECT
public:
    explicit HtmlClient(WebSocketTransport *client, QObject *parent = nullptr);
    enum class JsonData
    {
        Type,
        Data,
        UserInfo
    };
    Q_ENUM(JsonData)
    enum class HtmlCallType
    {
        Login = 0,
        Data = 1
    };
    Q_ENUM(HtmlCallType)
    enum class NewDataType
    {
        LoginSucceeded = 0x01,
        LoginFailed = 0x02,
        SendData = 0x04,
        Relist = 0x08,
        ReportData = 0x10
    };
    Q_ENUM(NewDataType)
    enum class UserInfo
    {
        Username,
        Password
    };
    Q_ENUM(UserInfo)
    enum class TablePermission
    {
        InsertOnly = 0x01,
        DeleteOnly = 0x02,
        UpdateOnly = 0x04,
        SelectOnly = 0x08,
        DeleteSelect    = DeleteOnly | SelectOnly,
        UpdateSelect    = UpdateOnly | SelectOnly,
        NotDelete       = InsertOnly | UpdateOnly | SelectOnly,
        AllPermissions  = InsertOnly | DeleteOnly | UpdateOnly | SelectOnly
    };
    enum class TableData
    {
        Columns,
        Data
    };
    Q_ENUM(TableData)
    Q_ENUM(TablePermission)
    enum class Menu
    {
        Id,
        Name,
        Permission,
        Path,
        Icon
    };
    Q_ENUM(Menu)
public Q_SLOTS:
    /*These slots are invoked from the HTML client side and received by the server.*/
    void htmlCall(const QJsonObject &object);
signals:
    /*These signal are emitted from the C++ side and received by the HTML client side.*/
    void newData(const QJsonObject &);
private:
    void sendNewData(const NewDataType &type, const QJsonObject &data);

    bool checkUserInfoStruct(const QJsonObject &userInfo) const;

    bool checkUserInfo(const QJsonObject &userInfo) const;

    QJsonObject getDBData(const int &p);

    template <class T>
    QJsonObject tableData(const QString &columns = "*");

    QJsonArray getTableAllData(const QString &tableName, const QString &columns = "*");

    QJsonObject getMenus() const;

    template <class T>
    QString eicon() const;

    template <class T>
    QJsonObject getMenu(const TablePermission &tp) const;

    template <class T>
    int eint(const T &t) const;

    template <class T>
    QString ekey(const T &t) const;

    template <class T>
    QString ename() const;
private:
    QWebChannel_PTR channel_ptr;
    QJsonObject userInfo;
};

#endif // HTMLCLIENT_H
