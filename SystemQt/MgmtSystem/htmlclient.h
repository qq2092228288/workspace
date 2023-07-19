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
        Login = 0x01,
        InsertData = 0x10,
        DeleteData = 0x20,
        UpdateData = 0x40,
        SelectData = 0x80
    };
    Q_ENUM(HtmlCallType)
    enum class NewDataType
    {
        LoginSucceeded = 0x01,
        LoginFailed = 0x02,
        SendData = 0x04,
        Relist = 0x08,
        ReportData = 0x10,
        OperationSuccessful = 0x20,
        OperationFailed = 0x40,
        InsufficientPermissions = 0x80
    };
    Q_ENUM(NewDataType)
    enum class UserInfo
    {
        Username,
        Password
    };
    Q_ENUM(UserInfo)
    enum TablePermission
    {
        Insert = 0x01,
        Delete = 0x02,
        Update = 0x04,
        View = 0x08,
        InsertUpdate = Insert | Update,
        DeleteView = Delete | View,
        InsertDeleteUpdate = Insert | Delete | Update
    };
    Q_ENUM(TablePermission)
    enum class TableData
    {
        Columns,
        Data,
        En,
        Cn
    };
    Q_ENUM(TableData)
    enum class Menu
    {
        Id,
        Name,
        Permission,
        Path,
        Icon
    };
    Q_ENUM(Menu)
    enum class NewColumns
    {
        AgentName,
        AdminName,
        PlaceName,
        TotalCount,
        UsedCount
    };
    Q_ENUM(NewColumns)
    enum class CommStruct
    {
        TableName,
        Criteria,
        Set,
        ColumnName,
        ColumnValue
    };
    Q_ENUM(CommStruct)
public Q_SLOTS:
    /*These slots are invoked from the HTML client side and received by the server.*/
    void htmlCall(const QJsonObject &object);
    void getReport(const QJsonObject &object);
    void consultation(const QJsonObject &object);
signals:
    /*These signal are emitted from the C++ side and received by the HTML client side.*/
    void newData(const QJsonObject &);
    void reportData(const QJsonObject &);
private:
    void sendNewData(const NewDataType &type, const QJsonObject &data = QJsonObject());
    /*Check the structure of logged in user information*/
    bool checkUserInfoStruct(const QJsonObject &userInfo) const;
    /*Check logged in user information*/
    int checkUserInfo(const QJsonObject &userInfo) const;

    bool insertData(const QJsonObject &json) const;

    bool deleteData(const QJsonObject &json) const;

    bool updateData(const QJsonObject &json) const;

    /*Get database data*/
    QJsonObject getDBData(const int &p);

    QJsonObject tableData(const QStringList &cnNames, const QString &sqlStatement);

    QJsonArray getTableAllData(const QString &tableName, const QString &columns = "*");

    QJsonObject getMenus() const;

    QString cn_NewColumns(const NewColumns &newColumns) const;

    template <class T>
    QStringList getCnColumns() const;

    template <class T>
    QString eicon() const;

    template <class T>
    QJsonObject getMenu(const int &tp) const;

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
