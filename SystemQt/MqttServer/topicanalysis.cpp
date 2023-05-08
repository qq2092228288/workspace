#include "topicanalysis.h"
#include "singleton.h"
#include <QSqlError>
#include <QSqlRecord>

TopicAnalysis::TopicAnalysis(QObject *parent)
    : QObject{parent}
{
    m_database = QSqlDatabase::addDatabase("QPSQL");
    m_database.setHostName(Singleton::hostname());
    m_database.setPort(Singleton::databasePort());
    m_database.setDatabaseName(Singleton::databaseName());
    m_database.setUserName(Singleton::userName());
    m_database.setPassword(Singleton::password());
}

void TopicAnalysis::createTables()
{
    if (m_database.open()) {
        qDebug()<<"Database opened successfully.";
    }
    else {
        qDebug()<<m_database.lastError();
    }
    QSqlQuery sqlQuery(m_database);
    sqlQuery.exec(QString("CREATE TABLE %1("
                          "%2   char(32)        NOT NULL PRIMARY KEY,"
                          "%3   varchar(32)     NOT NULL,"
                          "%4   varchar(128)    NOT NULL,"
                          "%5   varchar(128)    NOT NULL,"
                          "%6   text)")
                  .arg(Singleton::enumName<AgentInfo>(),
                       Singleton::enumValueToKey(AgentInfo::agentId),
                       Singleton::enumValueToKey(AgentInfo::name),
                       Singleton::enumValueToKey(AgentInfo::contact),
                       Singleton::enumValueToKey(AgentInfo::address),
                       Singleton::enumValueToKey(AgentInfo::remarks)));
    sqlQuery.exec(QString("CREATE TABLE %1("
                          "%2   varchar(32)     NOT NULL PRIMARY KEY,"
                          "%3   varchar(32)     NOT NULL,"
                          "%4   varchar(32)     NOT NULL,"
                          "%5   smallint        NOT NULL DEFAULT 0,"
                          "%6   text,"
                          "%7   text)")
                  .arg(Singleton::enumName<AdministratorInfo>(),
                       Singleton::enumValueToKey(AdministratorInfo::adminId),
                       Singleton::enumValueToKey(AdministratorInfo::password),
                       Singleton::enumValueToKey(AdministratorInfo::name),
                       Singleton::enumValueToKey(AdministratorInfo::permission),
                       Singleton::enumValueToKey(AdministratorInfo::uniqueIds),
                       Singleton::enumValueToKey(AdministratorInfo::remarks)));
    sqlQuery.exec(QString("CREATE TABLE %1("
                          "%2   char(32)        NOT NULL PRIMARY KEY,"
                          "%3   varchar(32)     NOT NULL,"
                          "%4   char(32)        NOT NULL,"
                          "%5   char(32)        NOT NULL,"
                          "%6   varchar(32)     NOT NULL,"
                          "%7   integer         NOT NULL,"
                          "%8   timestamp       NOT NULL)")
                  .arg(Singleton::enumName<AllocatedConsumables>(),
                       Singleton::enumValueToKey(AllocatedConsumables::consumablesId),
                       Singleton::enumValueToKey(AllocatedConsumables::type),
                       Singleton::enumValueToKey(AllocatedConsumables::uniqueId),
                       Singleton::enumValueToKey(AllocatedConsumables::agentId),
                       Singleton::enumValueToKey(AllocatedConsumables::adminId),
                       Singleton::enumValueToKey(AllocatedConsumables::count),
                       Singleton::enumValueToKey(AllocatedConsumables::time)));
    sqlQuery.exec(QString("CREATE TABLE %1("
                          "%2   char(32)        NOT NULL PRIMARY KEY,"
                          "%3   varchar(32)     NOT NULL,"
                          "%4   char(32)        NOT NULL,"
                          "%5   char(32)        NOT NULL,"
                          "%6   smallint        NOT NULL DEFAULT 0,"
                          "%7   integer         NOT NULL DEFAULT 0,"
                          "%8   integer         NOT NULL DEFAULT 0)")
                  .arg(Singleton::enumName<CombinedDevice>(),
                       Singleton::enumValueToKey(CombinedDevice::uniqueId),
                       Singleton::enumValueToKey(CombinedDevice::name),
                       Singleton::enumValueToKey(CombinedDevice::placeId),
                       Singleton::enumValueToKey(CombinedDevice::agentId),
                       Singleton::enumValueToKey(CombinedDevice::status),
                       Singleton::enumValueToKey(CombinedDevice::totalCount),
                       Singleton::enumValueToKey(CombinedDevice::usedCount)));
    sqlQuery.exec(QString("CREATE TABLE %1("
                          "%2   char(32)        NOT NULL PRIMARY KEY,"
                          "%3   varchar(32)     NOT NULL,"
                          "%4   varchar(64)     NOT NULL,"
                          "%5   smallint        NOT NULL DEFAULT 0,"
                          "%6   text)")
                  .arg(Singleton::enumName<Device>(),
                       Singleton::enumValueToKey(Device::deviceId),
                       Singleton::enumValueToKey(Device::type),
                       Singleton::enumValueToKey(Device::batch),
                       Singleton::enumValueToKey(Device::status),
                       Singleton::enumValueToKey(Device::remarks)));
    sqlQuery.exec(QString("CREATE TABLE %1("
                          "%2   macaddr         NOT NULL PRIMARY KEY,"
                          "%3   varchar(32)     NOT NULL,"
                          "%4   varchar(32)     NOT NULL,"
                          "%5   varchar(64)     NOT NULL,"
                          "%6   smallint        NOT NULL DEFAULT 0,"
                          "%7   text)")
                  .arg(Singleton::enumName<Computer>(),
                       Singleton::enumValueToKey(Computer::macAddress),
                       Singleton::enumValueToKey(Computer::type),
                       Singleton::enumValueToKey(Computer::systemVersion),
                       Singleton::enumValueToKey(Computer::batch),
                       Singleton::enumValueToKey(Computer::status),
                       Singleton::enumValueToKey(Computer::remarks)));
    sqlQuery.exec(QString("CREATE TABLE %1("
                          "%2   char(32)        NOT NULL,"
                          "%3   timestamp       NOT NULL,"
                          "%4   smallint        NOT NULL,"
                          "%5   text            NOT NULL,"
                          "PRIMARY KEY(%2,%3))")
                  .arg(Singleton::enumName<ReportInfo>(),
                       Singleton::enumValueToKey(ReportInfo::uniqueId),
                       Singleton::enumValueToKey(ReportInfo::time),
                       Singleton::enumValueToKey(ReportInfo::type),
                       Singleton::enumValueToKey(ReportInfo::reportData)));
    sqlQuery.exec(QString("CREATE TABLE %1("
                          "%2   varchar(128)    NOT NULL PRIMARY KEY,"
                          "%3   varchar(32)     NOT NULL,"
                          "%4   varchar(32)     NOT NULL,"
                          "%5   varchar(255)    NOT NULL,"
                          "%6   text            NOT NULL,"
                          "%7   timestamp       NOT NULL)")
                  .arg(Singleton::enumName<SoftwareManagement>(),
                       Singleton::enumValueToKey(SoftwareManagement::appId),
                       Singleton::enumValueToKey(SoftwareManagement::name),
                       Singleton::enumValueToKey(SoftwareManagement::version),
                       Singleton::enumValueToKey(SoftwareManagement::downloadPath),
                       Singleton::enumValueToKey(SoftwareManagement::content),
                       Singleton::enumValueToKey(SoftwareManagement::time)));
    sqlQuery.exec(QString("CREATE TABLE %1("
                          "%2   char(32)        NOT NULL PRIMARY KEY,"
                          "%3   varchar(128)    NOT NULL,"
                          "%4   varchar(128)    NOT NULL)")
                  .arg(Singleton::enumName<PlaceInfo>(),
                       Singleton::enumValueToKey(PlaceInfo::placeId),
                       Singleton::enumValueToKey(PlaceInfo::hostName),
                       Singleton::enumValueToKey(PlaceInfo::secondaryName)));
}

void TopicAnalysis::messageAnalysis(const QByteArray &message, const QMqttTopicName &topic)
{
    if (message.isEmpty()) {
        emit error(MessageError::IsEmpty);
        return;
    }
    auto doc = QJsonDocument::fromJson(message);
    if (doc.isNull()) {
        emit error(MessageError::IncorrectFormat);
        return;
    }
    if (topic.levelCount() != 3) {
        emit error(MessageError::TopicInvalid);
        return;
    }
    auto level1 = Singleton::enumKeyToValue<PrimaryTopic>(topic.levels().at(0));
    auto level2 = Singleton::enumKeyToValue<SecondaryTopic>(topic.levels().at(1));
    if (-1 != level1 && -1 != level2) {
        auto id = topic.levels().at(2);
        switch (PrimaryTopic(level1)) {
        case PrimaryTopic::request:
            // respone to client or admin request and publish message
            if (legalUniqueId(id) || legalAdminId(id)) {
                emit error(MessageError::IllegalId);
            }
            response(message, topic);
            break;
        case PrimaryTopic::append:
            // admin append new data
            databaseOperation(message, topic, DatabaseOperation::Insert);
            break;
        case PrimaryTopic::update:
            // admin update data
            databaseOperation(message, topic, DatabaseOperation::Update);
            break;
        case PrimaryTopic::remove:
            // admin remove data
            databaseOperation(message, topic, DatabaseOperation::Delete);
            break;
        case PrimaryTopic::select:
        {
            auto data = databaseOperation(message, topic, DatabaseOperation::Select);
            if (!data.isNull()) {
                emit messagePublish(Singleton::getTopicName(ResponseTopic::response, getSTopic(topic), id), data);
            }
        }
            break;
        default:
            emit error(MessageError::NoAnalysis);
            break;
        }
    }
    else {
        emit error(MessageError::TopicInvalid);
    }
}

bool TopicAnalysis::legalAdminId(const QString &id)
{
    QSqlQuery sqlQuery(m_database);
    sqlQuery.exec(QString("SELECT * FROM %1 WHERE %2 = '%3'")
                  .arg(Singleton::enumName<AdministratorInfo>(),
                       Singleton::enumValueToKey(AdministratorInfo::adminId), id));
    return sqlQuery.next();
}

bool TopicAnalysis::legalUniqueId(const QString &id)
{
    QSqlQuery sqlQuery(m_database);
    sqlQuery.exec(QString("SELECT * FROM %1 WHERE %2 = '%3'")
                  .arg(Singleton::enumName<CombinedDevice>(),
                       Singleton::enumValueToKey(CombinedDevice::uniqueId), id));
    return sqlQuery.next();
}

void TopicAnalysis::response(const QByteArray &message, const QMqttTopicName &topic)
{
    auto sTopic = getSTopic(topic);
    auto id = topic.levels().at(2);
    auto object = QJsonDocument::fromJson(message).object();
    QByteArray data;
    QSqlQuery sqlQuery(m_database);
    switch (sTopic) {
    case SecondaryTopic::deviceInfo:
        // first upload offline used data, then get device info
        sqlQuery.exec(QString("SELECT * FROM %1 WHERE %2 = '%3'")
                      .arg(Singleton::enumName<CombinedDevice>(),
                           Singleton::enumValueToKey(CombinedDevice::uniqueId),
                           id));
        if (sqlQuery.next()) {
            data = Singleton::jsonToUtf8(getJsonObject(sqlQuery, Singleton::enumKeys<CombinedDevice>()));
        }
        break;
    case SecondaryTopic::uploadData:
    {
        auto reportTimeString = Singleton::enumValueToKey(ReportInfo::time);
        auto reportDataString = Singleton::enumValueToKey(ReportInfo::reportData);
        auto reprotTime = object.value(reportTimeString);
        auto reprotData = object.value(reportDataString);
        // insert valid data, if exist, ignore
        if (reprotTime.type() != QJsonValue::Undefined && reprotData != QJsonValue::Undefined) {
            databaseOperation(message, topic, DatabaseOperation::Insert);
            QJsonObject json;
            json.insert(reportTimeString, reprotTime.toString());
            data = Singleton::jsonToUtf8(json);
        }
        else {
            emit error(MessageError::RequiredDataIsIncomplete);
        }
    }
        break;
    case SecondaryTopic::software:
    {
        auto appIdString = Singleton::enumValueToKey(SoftwareManagement::appId);
        auto appId = object.value(appIdString);
        if (appId.type() != QJsonValue::Undefined) {
            sqlQuery.exec(QString("SELECT * FROM %1 WHERE %2 = '%3'")
                          .arg(Singleton::enumName<SoftwareManagement>(),
                               appIdString,
                               appId.toString()));
            if (sqlQuery.next()) {
                data = Singleton::jsonToUtf8(getJsonObject(sqlQuery, Singleton::enumKeys<SoftwareManagement>()));
            }
        }
        else {
            emit error(MessageError::RequiredDataIsIncomplete);
        }
    }
        break;
    case SecondaryTopic::signIn:
    {
        auto passwordString = Singleton::enumValueToKey(AdministratorInfo::password);
        auto password = object.value(passwordString);
        if (password.type() != QJsonValue::Undefined) {
            sqlQuery.exec(QString("SELECT * FROM %1 WHERE %2 = '%3' AND %4 = '%5'")
                          .arg(Singleton::enumName<AdministratorInfo>(),
                               Singleton::enumValueToKey(AdministratorInfo::adminId),
                               id,
                               passwordString,
                               password.toString()));
            if (sqlQuery.next()) {
                QJsonObject json;
                json.insert(Singleton::enumName<UserStatus>(),
                            Singleton::enumValueToKey(UserStatus::passwordCorrect));
                data = Singleton::jsonToUtf8(json);
            }
            else {
                QJsonObject json;
                json.insert(Singleton::enumName<UserStatus>(),
                            Singleton::enumValueToKey(UserStatus::passwordError));
                data = Singleton::jsonToUtf8(json);
            }
        }
        else {
            emit error(MessageError::RequiredDataIsIncomplete);
        }
    }
        break;
    case SecondaryTopic::changePassword:
    {
        auto oldPasswordString = Singleton::enumValueToKey(RemarksType::oldPassword);
        auto oldPassword = object.value(oldPasswordString);
        auto newPassword = object.value(Singleton::enumValueToKey(RemarksType::newPassword));
        if (oldPassword.type() != QJsonValue::Undefined && newPassword.type() != QJsonValue::Undefined) {
            sqlQuery.exec(QString("SELECT * FROM %1 WHERE %2 = '%3' AND %4 = '%5'")
                          .arg(Singleton::enumName<AdministratorInfo>(),
                               Singleton::enumValueToKey(AdministratorInfo::adminId),
                               id,
                               Singleton::enumValueToKey(AdministratorInfo::password),
                               oldPassword.toString()));
            if (sqlQuery.next()) {
                auto remarksString = sqlQuery.value(Singleton::enumValueToKey(AdministratorInfo::remarks)).toString();
                auto remarks = Singleton::utf8ToJsonArray(remarksString.toUtf8());
                QJsonObject oldPasswordJson;
                oldPasswordJson.insert(oldPasswordString, oldPassword.toString());
                oldPasswordJson.insert(Singleton::enumValueToKey(RemarksType::mtime),
                                       QDateTime::currentDateTime().toString("yyyyMMddhhmmss"));
                remarks.append(oldPasswordJson);
                sqlQuery.exec(QString("UPDATE %1 SET %2 = '%3', %4 = '%5' WHERE %6 = '%7'")
                              .arg(Singleton::enumName<AdministratorInfo>(),
                                   Singleton::enumValueToKey(AdministratorInfo::password),
                                   newPassword.toString(),
                                   Singleton::enumValueToKey(AdministratorInfo::remarks),
                                   Singleton::jsonToString(remarks),
                                   Singleton::enumValueToKey(AdministratorInfo::adminId),
                                   id));
                QJsonObject json;
                json.insert(Singleton::enumName<UserStatus>(),
                            Singleton::enumValueToKey(UserStatus::passwordChanged));
                data = Singleton::jsonToUtf8(json);
            }
            else {
                QJsonObject json;
                json.insert(Singleton::enumName<UserStatus>(),
                            Singleton::enumValueToKey(UserStatus::passwordModificationFailed));
                data = Singleton::jsonToUtf8(json);
            }
        }
        else {
            emit error(MessageError::RequiredDataIsIncomplete);
        }
    }
        break;
    default:
        emit error(MessageError::NoAnalysis);
        break;
    }
    if (!data.isNull()) {
        emit messagePublish(Singleton::getTopicName(ResponseTopic::response, sTopic, id), data);
    }
}

QByteArray TopicAnalysis::databaseOperation(const QByteArray &message, const QMqttTopicName &topic,
                                            const DatabaseOperation &type)
{
    auto sTopic = getSTopic(topic);
    auto object = QJsonDocument::fromJson(message).object();
    switch (sTopic) {
    case SecondaryTopic::uploadData:
        dbOperation<ReportInfo>(object, type, Singleton::enumValueToKey(ReportInfo::uniqueId)
                                + "," + Singleton::enumValueToKey(ReportInfo::time));
        break;
    case SecondaryTopic::device:
        return dbOperation(object, type, Device::deviceId);
        break;
    case SecondaryTopic::computer:
        return dbOperation(object, type, Computer::macAddress);
        break;
    case SecondaryTopic::combinedDevice:
        return dbOperation(object, type, CombinedDevice::uniqueId);
        break;
    case SecondaryTopic::allocatedConsumables:
        return dbOperation(object, type, AllocatedConsumables::consumablesId);
        break;
    case SecondaryTopic::place:
        return dbOperation(object, type, PlaceInfo::placeId);
        break;
    case SecondaryTopic::agent:
        return dbOperation(object, type, AgentInfo::agentId);
        break;
    case SecondaryTopic::reports:
        return dbOperation(object, type, ReportInfo::uniqueId);
        break;
    default:
        emit error(MessageError::NoAnalysis);
        break;
    }
    return QByteArray();
}

QJsonObject TopicAnalysis::getJsonObject(const QSqlQuery &sqlQuery, const QStringList &keys)
{
    QJsonObject object;
    foreach (auto key, keys) {
        auto value = sqlQuery.value(key);
        switch (value.type()) {
        case QVariant::Type::Int:
            object.insert(key, value.toInt());
            break;
        case QVariant::Type::Double:
            object.insert(key, value.toDouble());
            break;
        case QVariant::Type::Bool:
            object.insert(key, value.toBool());
            break;
        case QVariant::Type::DateTime:
            object.insert(key, value.toDateTime().toString("yyyy-MM-ddThh:mm:ss.zzz"));
            break;
        default:
            object.insert(key, value.toString());
            break;
        }
    }
    return object;
}

void TopicAnalysis::bindValue(QSqlQuery &sqlQuery, const QJsonObject &object)
{
    for (auto it = object.begin(); it != object.end(); ++it) {
        auto key = ":" + it.key();
        auto value = it.value();
        switch (value.type()) {
        case QJsonValue::String:
            sqlQuery.bindValue(key, value.toString());
            break;
        case QJsonValue::Bool:
            sqlQuery.bindValue(key, value.toBool());
            break;
        case QJsonValue::Double:
            sqlQuery.bindValue(key, value.toDouble());
            break;
        case QJsonValue::Array:
            sqlQuery.bindValue(key, Singleton::jsonToString(value.toArray()));
            break;
        case QJsonValue::Object:
            sqlQuery.bindValue(key, Singleton::jsonToString(value.toObject()));
            break;
        default:
            break;
        }
    }
}

SecondaryTopic TopicAnalysis::getSTopic(const QMqttTopicName &topic) const
{
    return SecondaryTopic(Singleton::enumKeyToValue<SecondaryTopic>(topic.levels().at(1)));
}

template <class T>
QByteArray TopicAnalysis::dbOperation(const QJsonObject &object, const DatabaseOperation &type, const QString &primaryKey)
{
    QStringList keys = Singleton::enumKeys<T>();
    keys.sort();
    QStringList okeys = object.keys();
    okeys.sort();
    if (keys != okeys) {
        emit error(MessageError::RequiredDataIsIncomplete);
        return QByteArray();
    }
    QSqlQuery sqlQuery(m_database);
    auto tableName = Singleton::enumName<T>();
    switch (type) {
    case DatabaseOperation::Select:
    {
        auto value = object.value(primaryKey).toString();
        if (value == "*" || value.isEmpty()) { // select all
            sqlQuery.prepare(QString("SELECT * FROM %1").arg(tableName));
        }
        else {
            sqlQuery.prepare(QString("SELECT * FROM %1 WHERE %2 = ?").arg(tableName, primaryKey));
            sqlQuery.addBindValue(value);
        }
    }
        break;
    case DatabaseOperation::Insert:
    {
        QStringList strs;
        foreach (auto str, okeys) {
            strs<<(":" + str);
        }
        sqlQuery.prepare(QString("INSERT INTO %1(%2) VALUES(%3) ON CONFLICT(%4) DO NOTHING")
                         .arg(tableName, okeys.join(", "), strs.join(", "), primaryKey));
        bindValue(sqlQuery, object);
    }
        break;
    case DatabaseOperation::Update:
    {
        QStringList strs;
        foreach (auto str, okeys) {
            strs<<QString("%1=:%1").arg(str);
        }
        sqlQuery.prepare(QString("UPDATE %1 SET %2 WHERE %3 = :%3")
                         .arg(tableName, strs.join(", "), primaryKey));
        bindValue(sqlQuery, object);
    }
        break;
    case DatabaseOperation::Delete:
        sqlQuery.prepare(QString("DELETE FROM %1 WHERE %2 = ?").arg(tableName, primaryKey));
        sqlQuery.addBindValue(object.value(primaryKey).toString());
        break;
    }
    sqlQuery.exec();
    QJsonArray array;
    while (sqlQuery.next()) {
        // send select data
        array.append(getJsonObject(sqlQuery, Singleton::enumKeys<T>()));
    }
    if (!array.isEmpty())
        return Singleton::jsonToUtf8(array);
    return QByteArray();
}

template<class T>
QByteArray TopicAnalysis::dbOperation(const QJsonObject &object, const DatabaseOperation &type, const T &column)
{
    return dbOperation<T>(object, type, Singleton::enumValueToKey(column));
}
