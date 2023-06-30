#include "htmlclient.h"
#include <singleton.h>
#include <QSqlQuery>
#include <QUuid>
#include <QDebug>
#include "databasens.h"
using namespace DatabaseEnumNs;

HtmlClient::HtmlClient(WebSocketTransport *client, QObject *parent)
    : QObject{parent},
      channel_ptr{new QWebChannel}
{
    channel_ptr->connectTo(client);
    channel_ptr->registerObject(QStringLiteral("core"), this);
}

void HtmlClient::htmlCall(const QJsonObject &object)
{
    auto type = object.value(ekey(JsonData::Type));
    if (!type.isDouble()) {
        TIME_DEBUG()<<"Incorrect JSON data structure.";
        return;
    }
    auto data = object.value(ekey(JsonData::Data)).toObject();
    switch (HtmlCallType(type.toInt(-1))) {
    case HtmlCallType::Login:
        if (checkUserInfoStruct(data)) {
            QSqlQuery query(Singleton::getInstance()->database());
            query.prepare(QString("SELECT * FROM %1 WHERE %2= ? AND %3 = ?")
                          .arg(ename<AdministratorInfo>(),
                               ekey(AdministratorInfo::adminId),
                               ekey(AdministratorInfo::password)));
            query.addBindValue(data.value(ekey(UserInfo::Username)).toString());
            query.addBindValue(data.value(ekey(UserInfo::Password)).toString());
            query.exec();
            if (query.next()) {
                userInfo = Singleton::getJsonObject(query);
                sendNewData(NewDataType::LoginSucceeded, getMenus());
            }
            else {
                sendNewData(NewDataType::LoginFailed, QJsonObject());
            }
        }
        break;
    case HtmlCallType::Data:
        if (checkUserInfo(data)) {
            sendNewData(NewDataType::SendData, getDBData(data.value(ekey(AdministratorInfo::permission)).toInt(-1)));
        }
        else {
            TIME_DEBUG()<<"false";
        }
        break;
    default:
        TIME_DEBUG()<<"The value of 'type' is an undefined type.";
        break;
    }
}

void HtmlClient::sendNewData(const NewDataType &type, const QJsonObject &data)
{
    QJsonObject object;
    object.insert(ekey(JsonData::Type), eint(type));
    object.insert(ekey(JsonData::Data), data);
    object.insert(ekey(JsonData::UserInfo), userInfo);
    emit newData(object);
}

bool HtmlClient::checkUserInfoStruct(const QJsonObject &userInfo) const
{
    if (userInfo.count() == 2 &&
            userInfo.value(ekey(UserInfo::Username)).isString() &&
            userInfo.value(ekey(UserInfo::Password)).isString()) {
        return true;
    }
    return false;
}

bool HtmlClient::checkUserInfo(const QJsonObject &userInfo) const
{
    QStringList filter;
    foreach (auto key, userInfo.keys()) {
        auto value = userInfo.value(key);
        if (value.isDouble()) {
            filter<<QString("%1 = '%2'").arg(key).arg(value.toDouble());
        }
        else {
            filter<<QString("%1 = '%2'").arg(key, value.toString());
        }
    }
    QSqlQuery query(Singleton::getInstance()->database());
    query.exec(QString("SELECT * FROM %1 WHERE %2").arg(ename<AdministratorInfo>(), filter.join(" AND ")));
    if (query.next()) {
        return true;
    }
    return false;
}

QJsonObject HtmlClient::getDBData(const int &p)
{
    QJsonObject object;
    switch (UserPermissions(p)) {
    case UserPermissions::SuperAdministrator:
        object.insert(ename<SoftwareManagement>(), tableData<SoftwareManagement>());
        object.insert(ename<AdministratorInfo>(), tableData<AdministratorInfo>());
    case UserPermissions::SecondaryAdministrator:
        object.insert(ename<AgentInfo>(), tableData<AgentInfo>());
        object.insert(ename<AllocatedConsumables>(), tableData<AllocatedConsumables>());
        object.insert(ename<Device>(), tableData<Device>());
        object.insert(ename<PlaceInfo>(), tableData<PlaceInfo>());
    case UserPermissions::ReportSelectAndModify:
    case UserPermissions::ReportSelect:
    {
        auto columns = Singleton::enumKeys<ReportInfo>();
        columns.removeOne(ekey(ReportInfo::reportData));
        object.insert(ename<ReportInfo>(), tableData<ReportInfo>(columns.join(",")));
    }
        break;
    }
    return object;
}

template <class T>
QJsonObject HtmlClient::tableData(const QString &columns)
{
    QJsonObject object;
    auto tableName = ename<T>();

    QSqlQuery query(Singleton::getInstance()->database());
    query.exec(QString("SELECT %1 FROM %2").arg(columns, tableName));

    QJsonArray columnArray;
    for (int i = 0; i < query.record().count(); ++i) {
        auto key = query.record().fieldName(i);
        QJsonObject obj;
        obj.insert(key, EnumTextCN::cn_EnumValue(tableName, key));
        columnArray.append(obj);
    }
    object.insert(ekey(TableData::Columns), columns);

    QJsonArray data;
    while (query.next()) {
        data.append(Singleton::getJsonObject(query));
    }
    object.insert(ekey(TableData::Data), data);
    return object;
}

QJsonArray HtmlClient::getTableAllData(const QString &tableName, const QString &columns)
{
    QJsonArray array;
    QSqlQuery query(Singleton::getInstance()->database());
    query.exec(QString("SELECT %1 FROM %2").arg(columns, tableName));
    while (query.next()) {
        array.append(Singleton::getJsonObject(query));
    }
    return array;
}

QJsonObject HtmlClient::getMenus() const
{
    QJsonObject object;
    auto p = userInfo.value(ekey(AdministratorInfo::permission)).toInt(-1);
    if (-1 == p) {
        return object;
    }
    QJsonArray menu;
    switch (UserPermissions(p)) {
    case UserPermissions::SuperAdministrator:
        menu.append(getMenu<SoftwareManagement>(TablePermission::AllPermissions));
        menu.append(getMenu<AdministratorInfo>(TablePermission::AllPermissions));
        menu.append(getMenu<AgentInfo>(TablePermission::AllPermissions));
        menu.append(getMenu<AllocatedConsumables>(TablePermission::AllPermissions));
        menu.append(getMenu<Device>(TablePermission::AllPermissions));
        menu.append(getMenu<PlaceInfo>(TablePermission::AllPermissions));
        menu.append(getMenu<ReportInfo>(TablePermission::DeleteSelect));
        break;
    case UserPermissions::SecondaryAdministrator:
        menu.append(getMenu<AgentInfo>(TablePermission::NotDelete));
        menu.append(getMenu<AllocatedConsumables>(TablePermission::NotDelete));
        menu.append(getMenu<Device>(TablePermission::NotDelete));
        menu.append(getMenu<PlaceInfo>(TablePermission::NotDelete));
        menu.append(getMenu<ReportInfo>(TablePermission::SelectOnly));
        break;
    case UserPermissions::ReportSelectAndModify:
        menu.append(getMenu<ReportInfo>(TablePermission::UpdateSelect));
        break;
    case UserPermissions::ReportSelect:
        menu.append(getMenu<ReportInfo>(TablePermission::SelectOnly));
        break;
    }
    object.insert(ename<Menu>(), menu);
    return object;
}

template<class T>
QString HtmlClient::eicon() const
{
    QString str;
    if (typeid(T) == typeid(SoftwareManagement)) {
        str = "el-icon-files";
    }
    else if (typeid(T) == typeid(AdministratorInfo)) {
        str = "el-icon-user";
    }
    else if (typeid(T) == typeid(AgentInfo)) {
        str = "el-icon-phone";
    }
    else if (typeid(T) == typeid(AllocatedConsumables)) {
        str = "el-icon-goods";
    }
    else if (typeid(T) == typeid(Device)) {
        str = "el-icon-monitor";
    }
    else if (typeid(T) == typeid(PlaceInfo)) {
        str = "el-icon-place";
    }
    else if (typeid(T) == typeid(ReportInfo)) {
        str = "el-icon-document";
    }
    return str;
}

template<class T>
QJsonObject HtmlClient::getMenu(const TablePermission &tp) const
{
    QJsonObject menu;
    menu.insert(ekey(Menu::Id), QUuid::createUuid().toString(QUuid::Id128));
    menu.insert(ekey(Menu::Name), EnumTextCN::cn_EnumName(ename<T>()));
    menu.insert(ekey(Menu::Permission), eint(tp));
    menu.insert(ekey(Menu::Path), "/homepage/" + ename<T>());
    menu.insert(ekey(Menu::Icon), eicon<T>());
    return menu;
}

template<class T>
int HtmlClient::eint(const T &t) const
{
    return static_cast<int>(t);
}

template<class T>
QString HtmlClient::ekey(const T &t) const
{
    return Singleton::enumValueToKey(t);
}

template<class T>
QString HtmlClient::ename() const
{
    return Singleton::enumName<T>();
}
