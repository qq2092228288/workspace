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

HtmlClient::~HtmlClient()
{
    channel_ptr->deleteLater();
}

void HtmlClient::htmlCall(const QJsonObject &object)
{
    auto type = object.value(ekey(JsonData::Type));
    if (!type.isDouble()) {
        TIME_DEBUG()<<"Incorrect JSON data structure: "<<Singleton::jsonToString(object);
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
                sendNewData(NewDataType::LoginFailed);
            }
        }
        break;
    case HtmlCallType::UpdatePassword:
        if (-1 != checkUserInfo(data)) {
            if (updatePassword(data.value("fresh").toString())) {
                sendNewData(NewDataType::Relist);
            }
            else {
                sendNewData(NewDataType::OperationFailed);
            }
        }
        break;
    case HtmlCallType::InsertData:
        if (UserPermissions::SuperAdministrator == UserPermissions(checkUserInfo(data)) ||
                UserPermissions::SecondaryAdministrator == UserPermissions(checkUserInfo(data))) {
            if (insertData(data.value(ename<CommStruct>()).toObject())) {
                sendNewData(NewDataType::OperationSuccessful);
            }
            else {
                sendNewData(NewDataType::OperationFailed);
            }
        }
        else {
            sendNewData(NewDataType::InsufficientPermissions);
        }
        break;
    case HtmlCallType::DeleteData:
        if (UserPermissions::SuperAdministrator == UserPermissions(checkUserInfo(data))) {
            if (deleteData(data.value(ename<CommStruct>()).toObject())) {
                sendNewData(NewDataType::OperationSuccessful);
            }
            else {
                sendNewData(NewDataType::OperationFailed);
            }
        }
        else {
            sendNewData(NewDataType::InsufficientPermissions);
        }
        break;
    case HtmlCallType::UpdateData:
        if (UserPermissions::SuperAdministrator == UserPermissions(checkUserInfo(data)) ||
                UserPermissions::SecondaryAdministrator == UserPermissions(checkUserInfo(data))) {
            if (updateData(data.value(ename<CommStruct>()).toObject())) {
                sendNewData(NewDataType::OperationSuccessful);
            }
            else {
                sendNewData(NewDataType::OperationFailed);
            }
        }
        else {
            sendNewData(NewDataType::InsufficientPermissions);
        }
        break;
    case HtmlCallType::SelectData:
        if (-1 != checkUserInfo(data)) {
            sendNewData(NewDataType::SendData, getDBData(data.value(ekey(AdministratorInfo::permission)).toInt(-1)));
        }
        else {
            sendNewData(NewDataType::Relist);
        }
        break;
//    default:
//        TIME_DEBUG()<<"The value of 'type' is an undefined type: "<<Singleton::jsonToString(object);;
//        break;
    }
}

void HtmlClient::getReport(const QJsonObject &object)
{
    QSqlQuery query(Singleton::getInstance()->database());
    auto reportTime = object.value(ekey(ReportInfo::reportTime)).toString();
    auto deviceId = object.value(ekey(ReportInfo::deviceId)).toString();
    query.exec(QString("SELECT %1 FROM %2 WHERE %3 = '%4' AND %5 = '%6'")
               .arg(ekey(ReportInfo::reportData),
                    ename<ReportInfo>(),
                    ekey(ReportInfo::reportTime),
                    reportTime,
                    ekey(ReportInfo::deviceId),
                    deviceId));
    query.next();
    QJsonObject report;
    report.insert(ekey(ReportInfo::reportTime), reportTime);
    report.insert(ekey(ReportInfo::deviceId), deviceId);
    report.insert(ekey(ReportInfo::reportData), query.value(0).toString());
    emit reportData(report);
}

void HtmlClient::consultation(const QJsonObject &object)
{
    auto str = QString("reportConclusion");
    auto reportTime = object.value(ekey(ReportInfo::reportTime)).toString();
    auto deviceId = object.value(ekey(ReportInfo::deviceId)).toString();
    auto reportConclusion = object.value(str).toString();
    if (reportTime.isEmpty() || deviceId.isEmpty() || reportConclusion.isEmpty()) {
        sendNewData(NewDataType::OperationFailed);
    }
    else {
        QSqlQuery query(Singleton::getInstance()->database());
        query.exec(QString("SELECT %1 FROM %2 WHERE %3 = '%4' AND %5 = '%6'")
                   .arg(ekey(ReportInfo::reportData),
                        ename<ReportInfo>(),
                        ekey(ReportInfo::reportTime),
                        reportTime,
                        ekey(ReportInfo::deviceId),
                        deviceId));
        query.next();
        auto reportData = Singleton::utf8ToJsonObject(query.value(0).toString().toUtf8());
        reportData.insert(str, reportConclusion);
        query.exec(QString("UPDATE %1 SET %2 = '1', %3 = '%4' WHERE %5 = '%6' AND %7 = '%8'")
                   .arg(ename<ReportInfo>(),
                        ekey(ReportInfo::modify),
                        ekey(ReportInfo::reportData),
                        Singleton::jsonToString(reportData),
                        ekey(ReportInfo::reportTime),
                        reportTime,
                        ekey(ReportInfo::deviceId),
                        deviceId));
        if (query.lastError().type() == QSqlError::NoError) {
            sendNewData(NewDataType::OperationSuccessful);
        }
        else {
            sendNewData(NewDataType::OperationFailed);
        }
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

int HtmlClient::checkUserInfo(const QJsonObject &userInfo)
{
    QStringList filter;
    foreach (auto key, Singleton::enumKeys<AdministratorInfo>()) {
        auto value = userInfo.value(key);
        if (value.isDouble()) {
            filter<<QString("%1 = '%2'").arg(key).arg(value.toDouble());
        }
        else {
            filter<<QString("%1 = '%2'").arg(key, value.toString());
        }
    }
    QSqlQuery query(Singleton::getInstance()->database());
    query.exec(QString("SELECT %1 FROM %2 WHERE %3")
               .arg(ekey(AdministratorInfo::permission), ename<AdministratorInfo>(), filter.join(" AND ")));
    if (query.next()) {
        this->userInfo = userInfo;
        return query.value(0).toInt();
    }
    return -1;
}

bool HtmlClient::updatePassword(const QString &fresh) const
{
    auto adminId = userInfo.value(ekey(AdministratorInfo::adminId)).toString();
    QSqlQuery query(Singleton::getInstance()->database());
    query.exec(QString("UPDATE %1 SET %2 = '%3' WHERE %4 = '%5'")
               .arg(ename<AdministratorInfo>(),
                    ekey(AdministratorInfo::password),
                    fresh,
                    ekey(AdministratorInfo::adminId),
                    adminId));
    query.exec(QString("SELECT * FROM %1 WHERE %2 = '%3' AND %4 = '%5'")
               .arg(ename<AdministratorInfo>(),
                    ekey(AdministratorInfo::adminId),
                    adminId,
                    ekey(AdministratorInfo::password),
                    fresh));
    if (query.next()) {
        return true;
    }
    return false;
}

bool HtmlClient::insertData(const QJsonObject &json) const
{
    auto tableName = json.value(ekey(CommStruct::TableName)).toString();
    auto array = json.value(ekey(CommStruct::Criteria)).toArray();
    QStringList columns, values;
    foreach (auto value, array) {
        auto object = value.toObject();
        columns<<object.value(ekey(CommStruct::ColumnName)).toString();
        auto colvalue = object.value(ekey(CommStruct::ColumnValue));
        if (colvalue.isString()) {
            auto str = colvalue.toString();
            values<<("default" == str ? str : QString("'%1'").arg(str));
        }
        else {
            values<<QString("'%1'").arg(colvalue.toDouble());
        }
    }
    QSqlQuery query(Singleton::getInstance()->database());
    query.exec(QString("INSERT INTO %1(%2) VALUES(%3)").arg(tableName, columns.join(" , "), values.join(" , ")));
    return (query.lastError().type() == QSqlError::NoError);
}

bool HtmlClient::deleteData(const QJsonObject &json) const
{
    auto tableName = json.value(ekey(CommStruct::TableName)).toString();
    auto array = json.value(ekey(CommStruct::Criteria)).toArray();
    QStringList criteria;
    foreach (auto value, array) {
        auto object = value.toObject();
        auto colvalue = object.value(ekey(CommStruct::ColumnValue));
        if (colvalue.isString()) {
            criteria<<QString("%1 = '%2'").arg(object.value(ekey(CommStruct::ColumnName)).toString(), colvalue.toString());
        }
        else {
            criteria<<QString("%1 = '%2'").arg(object.value(ekey(CommStruct::ColumnName)).toString())
                      .arg(colvalue.toDouble());
        }
    }
    QSqlQuery query(Singleton::getInstance()->database());
    query.exec(QString("DELETE FROM %1 WHERE %2").arg(tableName, criteria.join(" AND ")));
    return (query.lastError().type() == QSqlError::NoError);
}

bool HtmlClient::updateData(const QJsonObject &json) const
{
    auto tableName = json.value(ekey(CommStruct::TableName)).toString();
    auto setArray = json.value(ekey(CommStruct::Set)).toArray();
    QStringList set;
    foreach (auto value, setArray) {
        auto object = value.toObject();
        auto colvalue = object.value(ekey(CommStruct::ColumnValue));
        if (colvalue.isString()) {
            set<<QString("%1 = '%2'").arg(object.value(ekey(CommStruct::ColumnName)).toString(), colvalue.toString());
        }
        else {
            set<<QString("%1 = '%2'").arg(object.value(ekey(CommStruct::ColumnName)).toString())
                 .arg(colvalue.toDouble());
        }
    }
    auto criteriaArray = json.value(ekey(CommStruct::Criteria)).toArray();
    QStringList criteria;
    foreach (auto value, criteriaArray) {
        auto object = value.toObject();
        auto colvalue = object.value(ekey(CommStruct::ColumnValue));
        if (colvalue.isString()) {
            criteria<<QString("%1 = '%2'").arg(object.value(ekey(CommStruct::ColumnName)).toString(), colvalue.toString());
        }
        else {
            criteria<<QString("%1 = '%2'").arg(object.value(ekey(CommStruct::ColumnName)).toString())
                      .arg(colvalue.toDouble());
        }
    }
    QSqlQuery query(Singleton::getInstance()->database());
    query.exec(QString("UPDATE %1 SET %2 WHERE %3").arg(tableName, set.join(" , ") , criteria.join(" AND ")));
    return (query.lastError().type() == QSqlError::NoError);
}

QJsonObject HtmlClient::getDBData(const int &p)
{
    QJsonObject object;
    switch (UserPermissions(p)) {
    case UserPermissions::SuperAdministrator:
        object.insert(ename<SoftwareManagement>(), tableData(getCnColumns<SoftwareManagement>(),
                          QString("SELECT %1, %2, %3, %4, %5 FROM %6 ORDER BY %7 DESC")
                          .arg(ekey(SoftwareManagement::appId),
                               ekey(SoftwareManagement::name),
                               ekey(SoftwareManagement::version),
                               ekey(SoftwareManagement::downloadPath),
                               ekey(SoftwareManagement::content),
                               ename<SoftwareManagement>(),
                               ekey(SoftwareManagement::createTime))));
        object.insert(ename<AdministratorInfo>(), tableData(getCnColumns<AdministratorInfo>(),
                          QString("SELECT * FROM %1 WHERE %2 > %3 ORDER BY %2")
                          .arg(ename<AdministratorInfo>(), ekey(AdministratorInfo::permission))
                          .arg(userInfo.value(ekey(AdministratorInfo::permission)).toDouble())));
    case UserPermissions::SecondaryAdministrator:
        object.insert(ename<AgentInfo>(), tableData(getCnColumns<AgentInfo>(),
                          QString("SELECT * FROM %1").arg(ename<AgentInfo>())));
        object.insert(ename<AllocatedConsumables>(), tableData(getCnColumns<AllocatedConsumables>(),
                          QString("SELECT a.%1, a.%2, a.%3, b.%4 AS %5, c.%6 AS %7, a.%8 "
                                  "FROM %9 AS a "
                                  "LEFT JOIN %10 AS b ON a.%11 = b.%12 "
                                  "LEFT JOIN %13 AS c ON a.%14 = c.%15 "
                                  "ORDER BY a.%1 DESC")
                          .arg(ekey(AllocatedConsumables::createTime),
                               ekey(AllocatedConsumables::type),
                               ekey(AllocatedConsumables::deviceId),
                               ekey(AgentInfo::name),
                               ekey(NewColumns::AgentName),
                               ekey(AdministratorInfo::name),
                               ekey(NewColumns::AdminName),
                               ekey(AllocatedConsumables::count),
                               ename<AllocatedConsumables>())
                          .arg(ename<AgentInfo>(),
                               ekey(AllocatedConsumables::agentId),
                               ekey(AgentInfo::agentId),
                               ename<AdministratorInfo>(),
                               ekey(AllocatedConsumables::adminId),
                               ekey(AdministratorInfo::adminId))));
        object.insert(ename<Device>(), tableData(getCnColumns<Device>(),
                          QString("SELECT a.%1, a.%2, CONCAT(b.%3, '&', b.%4) AS %5, c.%6 AS %7, d.%8 AS %9, "
                                  "a.%10, a.%11, COALESCE(e.%12, 0) AS %12, COALESCE(f.%13, 0) AS %13 "
                                  "FROM %14 AS a "
                                  "LEFT JOIN %15 AS b ON a.%16 = b.%17 "
                                  "LEFT JOIN %18 AS c ON a.%19 = c.%20 "
                                  "LEFT JOIN %21 AS d ON a.%22 = d.%23 "
                                  "LEFT JOIN (SELECT %24, SUM(%25) AS %12 FROM %26 GROUP BY %24) AS e ON a.%1 = e.%24 "
                                  "LEFT JOIN (SELECT %27, COUNT(%27) AS %13 FROM %28 GROUP BY %27) AS f ON a.%1 = f.%27 "
                                  "ORDER BY a.%1 DESC")
                          .arg(ekey(Device::deviceId), ekey(Device::password), ekey(PlaceInfo::hostName),
                               ekey(PlaceInfo::secondaryName), ekey(NewColumns::PlaceName), ekey(AgentInfo::name),
                               ekey(NewColumns::AgentName), ekey(AdministratorInfo::name), ekey(NewColumns::AdminName))
                          .arg(ekey(Device::type), ekey(Device::status), ekey(NewColumns::TotalCount),
                               ekey(NewColumns::UsedCount), ename<Device>(), ename<PlaceInfo>(),
                               ekey(Device::placeId), ekey(PlaceInfo::placeId), ename<AgentInfo>())
                          .arg(ekey(Device::agentId), ekey(AgentInfo::agentId), ename<AdministratorInfo>(),
                               ekey(Device::adminId), ekey(AdministratorInfo::adminId),
                               ekey(AllocatedConsumables::deviceId), ekey(AllocatedConsumables::count),
                               ename<AllocatedConsumables>(), ekey(ReportInfo::deviceId))
                          .arg(ename<ReportInfo>())));
        object.insert(ename<PlaceInfo>(), tableData(getCnColumns<PlaceInfo>(),
                          QString("SELECT * FROM %1").arg(ename<PlaceInfo>())));
    case UserPermissions::ReportSelectAndModify:
    case UserPermissions::ReportSelect:
#if 0
        object.insert(ename<ReportInfo>(), tableData(getCnColumns<ReportInfo>(),
                          QString("SELECT %1, %2, %3, %4 FROM %5 ORDER BY %1 DESC")
                          .arg(ekey(ReportInfo::reportTime),
                               ekey(ReportInfo::deviceId),
                               ekey(ReportInfo::name),
                               ekey(ReportInfo::modify),
                               ename<ReportInfo>())));
#else
    {
        QString wstr = userInfo.value(ekey(AdministratorInfo::deviceIds)).toString().replace("{", "'").replace("}", "'");
        if (wstr == "*") {
            wstr.clear();
        }
        else {
            wstr = QString("WHERE %1 IN (%2)").arg(ekey(ReportInfo::deviceId), wstr);
        }
        QSqlQuery query(Singleton::getInstance()->database());
        query.exec(QString("SELECT %1, %2, %3, %4 FROM %5 ORDER BY %1 DESC")
                   .arg(ekey(ReportTable::reportTime),
                        ekey(ReportTable::deviceId),
                        ekey(ReportInfo::reportData),
                        ekey(ReportTable::modify),
                        ename<ReportInfo>() + " " + wstr));
        QJsonArray data;
        while (query.next()) {
            QJsonObject json;
            json.insert(ekey(ReportTable::reportTime),
                        query.value(0).toDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz"));
            json.insert(ekey(ReportTable::deviceId),
                        query.value(1).toString());
            auto reportData = Singleton::utf8ToJsonObject(query.value(2).toString().toUtf8());
            auto place = reportData.value("place").toObject();
            json.insert(ekey(ReportTable::primaryPlace),
                        place.value(ekey(ReportTable::primaryPlace, false)).toString());
            json.insert(ekey(ReportTable::secondaryPlace),
                        place.value(ekey(ReportTable::secondaryPlace, false)).toString());
            auto position = reportData.value("position").toArray();
            json.insert(ekey(ReportTable::position),
                        position.at(1).toObject().value("reportTime").isUndefined() ? "单" : "双");
            auto patientInfo = reportData.value("patientInfo").toObject();
            json.insert(ekey(ReportTable::medicalRecordNumber),
                        patientInfo.value(ekey(ReportTable::medicalRecordNumber, false)).toString());
            json.insert(ekey(ReportTable::patientName),
                        patientInfo.value(ekey(ReportTable::patientName, false)).toString().mid(0, 1) + "**");
            json.insert(ekey(ReportTable::sex),
                        patientInfo.value(ekey(ReportTable::sex, false)).toString());
            json.insert(ekey(ReportTable::age),
                        patientInfo.value(ekey(ReportTable::age, false)).toString());
            json.insert(ekey(ReportTable::height),
                        patientInfo.value(ekey(ReportTable::height, false)).toString());
            json.insert(ekey(ReportTable::weight),
                        patientInfo.value(ekey(ReportTable::weight, false)).toString());
            json.insert(ekey(ReportTable::modify),
                        query.value(3).toDouble() == 0 ? "否" : "是");
            data.append(json);
        }
        QJsonArray columns;
        for (int i = 0, count = QMetaEnum::fromType<ReportTable>().keyCount(); i < count; ++i) {
            auto key = ReportTable(i);
            QJsonObject column;
            column.insert(ekey(TableData::En), ekey(key));
            column.insert(ekey(TableData::Cn), cn_NewColumns(key));
            columns<<column;
        }
        QJsonObject reportDataObject;
        reportDataObject.insert(ekey(TableData::Columns), columns);
        reportDataObject.insert(ekey(TableData::Data), data);
        object.insert(ename<ReportInfo>(), reportDataObject);
    }
#endif
        break;
    }
    return object;
}

QJsonObject HtmlClient::tableData(const QStringList &cnNames, const QString &sqlStatement)
{
    QJsonObject object;
    QSqlQuery query(Singleton::getInstance()->database());
    query.exec(sqlStatement);

    QJsonArray columns;
    auto record = query.record();
    for (int i = 0; i < record.count(); ++i) {
        QJsonObject column;
        column.insert(ekey(TableData::En), record.fieldName(i));
        column.insert(ekey(TableData::Cn), cnNames.at(i));
        columns<<column;
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
        menu.append(getMenu<SoftwareManagement>(TablePermission::InsertDeleteUpdate));
        menu.append(getMenu<AdministratorInfo>(TablePermission::InsertDeleteUpdate));
        menu.append(getMenu<AgentInfo>(TablePermission::InsertDeleteUpdate));
        menu.append(getMenu<AllocatedConsumables>(TablePermission::InsertDeleteUpdate));
        menu.append(getMenu<Device>(TablePermission::InsertDeleteUpdate));
        menu.append(getMenu<PlaceInfo>(TablePermission::InsertDeleteUpdate));
        menu.append(getMenu<ReportInfo>(TablePermission::DeleteView));
        break;
    case UserPermissions::SecondaryAdministrator:
        menu.append(getMenu<AgentInfo>(TablePermission::InsertUpdate));
        menu.append(getMenu<AllocatedConsumables>(TablePermission::Insert));
        menu.append(getMenu<Device>(TablePermission::InsertUpdate));
        menu.append(getMenu<PlaceInfo>(TablePermission::InsertUpdate));
        menu.append(getMenu<ReportInfo>(TablePermission::View));
        break;
    case UserPermissions::ReportSelectAndModify:
        menu.append(getMenu<ReportInfo>(TablePermission::View));
        break;
    case UserPermissions::ReportSelect:
        menu.append(getMenu<ReportInfo>(TablePermission::View));
        break;
    }
    object.insert(ename<Menu>(), menu);
    return object;
}

QString HtmlClient::cn_NewColumns(const NewColumns &newColumns) const
{
    switch (newColumns) {
    case NewColumns::AgentName:
        return "经销商姓名";
    case NewColumns::AdminName:
        return "创建人";
    case NewColumns::PlaceName:
        return "场所";
    case NewColumns::TotalCount:
        return "总数量";
    case NewColumns::UsedCount:
        return "已使用数量";
    }
    return nullptr;
}

QString HtmlClient::cn_NewColumns(const ReportTable &newColumns) const
{
    switch (newColumns) {
    case ReportTable::reportTime:
        return "报告时间";
    case ReportTable::deviceId:
        return "设备编号";
    case ReportTable::primaryPlace:
        return "医院";
    case ReportTable::secondaryPlace:
        return "科室";
    case ReportTable::position:
        return "体位模式";
    case ReportTable::medicalRecordNumber:
        return "病历号";
    case ReportTable::patientName:
        return "姓名";
    case ReportTable::sex:
        return "性别";
    case ReportTable::age:
        return "年龄(岁)";
    case ReportTable::height:
        return "身高(cm)";
    case ReportTable::weight:
        return "体重(kg)";
    case ReportTable::modify:
        return "已会诊";
    }
    return nullptr;
}

template <class T>
QStringList HtmlClient::getCnColumns() const
{
    QStringList cns;
    if (typeid(T) == typeid(SoftwareManagement)) {
        cns<<EnumTextCN::cn_SoftwareManagement(SoftwareManagement::appId)
           <<EnumTextCN::cn_SoftwareManagement(SoftwareManagement::name)
           <<EnumTextCN::cn_SoftwareManagement(SoftwareManagement::version)
           <<EnumTextCN::cn_SoftwareManagement(SoftwareManagement::downloadPath)
           <<EnumTextCN::cn_SoftwareManagement(SoftwareManagement::content);
    }
    else if (typeid(T) == typeid(AdministratorInfo)) {
        cns<<EnumTextCN::cn_AdministratorInfo(AdministratorInfo::adminId)
           <<EnumTextCN::cn_AdministratorInfo(AdministratorInfo::password)
           <<EnumTextCN::cn_AdministratorInfo(AdministratorInfo::name)
           <<EnumTextCN::cn_AdministratorInfo(AdministratorInfo::permission)
           <<EnumTextCN::cn_AdministratorInfo(AdministratorInfo::deviceIds)
           <<EnumTextCN::cn_AdministratorInfo(AdministratorInfo::remarks);
    }
    else if (typeid(T) == typeid(AgentInfo)) {
        cns<<EnumTextCN::cn_AgentInfo(AgentInfo::agentId)
           <<EnumTextCN::cn_AgentInfo(AgentInfo::name)
           <<EnumTextCN::cn_AgentInfo(AgentInfo::contact)
           <<EnumTextCN::cn_AgentInfo(AgentInfo::address)
           <<EnumTextCN::cn_AgentInfo(AgentInfo::remarks);
    }
    else if (typeid(T) == typeid(AllocatedConsumables)) {
        cns<<EnumTextCN::cn_AllocatedConsumables(AllocatedConsumables::createTime)
           <<EnumTextCN::cn_AllocatedConsumables(AllocatedConsumables::type)
           <<EnumTextCN::cn_AllocatedConsumables(AllocatedConsumables::deviceId)
           <<cn_NewColumns(NewColumns::AgentName)
           <<cn_NewColumns(NewColumns::AdminName)
           <<EnumTextCN::cn_AllocatedConsumables(AllocatedConsumables::count);
    }
    else if (typeid(T) == typeid(Device)) {
        cns<<EnumTextCN::cn_Device(Device::deviceId)
           <<EnumTextCN::cn_Device(Device::password)
           <<cn_NewColumns(NewColumns::PlaceName)
           <<cn_NewColumns(NewColumns::AgentName)
           <<cn_NewColumns(NewColumns::AdminName)
           <<EnumTextCN::cn_Device(Device::type)
           <<EnumTextCN::cn_Device(Device::status)
           <<cn_NewColumns(NewColumns::TotalCount)
           <<cn_NewColumns(NewColumns::UsedCount);
    }
    else if (typeid(T) == typeid(PlaceInfo)) {
        cns<<EnumTextCN::cn_PlaceInfo(PlaceInfo::placeId)
           <<EnumTextCN::cn_PlaceInfo(PlaceInfo::hostName)
           <<EnumTextCN::cn_PlaceInfo(PlaceInfo::secondaryName);
    }
    else if (typeid(T) == typeid(ReportInfo)) {
        cns<<EnumTextCN::cn_ReportInfo(ReportInfo::reportTime)
           <<EnumTextCN::cn_ReportInfo(ReportInfo::deviceId)
           <<EnumTextCN::cn_ReportInfo(ReportInfo::name)
           <<EnumTextCN::cn_ReportInfo(ReportInfo::modify);
    }
    return cns;
}

template<class T>
QString HtmlClient::eicon() const
{
    QString str;
    if (typeid(T) == typeid(SoftwareManagement)) {
        str = "StarFilled";
    }
    else if (typeid(T) == typeid(AdministratorInfo)) {
        str = "UserFilled";
    }
    else if (typeid(T) == typeid(AgentInfo)) {
        str = "Avatar";
    }
    else if (typeid(T) == typeid(AllocatedConsumables)) {
        str = "WalletFilled";
    }
    else if (typeid(T) == typeid(Device)) {
        str = "Platform";
    }
    else if (typeid(T) == typeid(PlaceInfo)) {
        str = "LocationFilled";
    }
    else if (typeid(T) == typeid(ReportInfo)) {
        str = "Document";
    }
    return str;
}

template<class T>
QJsonObject HtmlClient::getMenu(const int &tp) const
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
QString HtmlClient::ekey(const T &t, bool lower) const
{
    auto str = QString(QMetaEnum::fromType<T>().valueToKey(static_cast<int>(t)));
    if (lower) {
        return str.toLower();
    }
    return str;
}

template<class T>
QString HtmlClient::ename() const
{
    return Singleton::enumName<T>();
}
