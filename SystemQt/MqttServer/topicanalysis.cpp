#include "topicanalysis.h"
#include <singleton.h>

TopicAnalysis::TopicAnalysis(QObject *parent)
    : QObject{parent}
{
    m_database = QSqlDatabase::addDatabase("QPSQL");
    m_database.setHostName(Singleton::hostname());
    m_database.setPort(Singleton::databasePort());
    m_database.setDatabaseName(Singleton::databaseName());
    m_database.setUserName(Singleton::userName());
    m_database.setPassword(Singleton::password());

    Singleton::getInstance()->setDatabase(&m_database);
}

void TopicAnalysis::createTables()
{
    if (m_database.open()) {
        TIME_DEBUG()<<"Database opened successfully.";
    }
    else {
        TIME_DEBUG()<<m_database.lastError();
    }
    QSqlQuery sqlQuery(m_database);
    sqlQuery.exec(QString("CREATE TABLE %1("
                          "%2   uuid            NOT NULL PRIMARY KEY DEFAULT gen_random_uuid(),"
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
#if ENABLE_COMBINE_DEVICE
                       Singleton::enumValueToKey(AdministratorInfo::uniqueIds),
#else
                       Singleton::enumValueToKey(AdministratorInfo::deviceIds),
#endif
                       Singleton::enumValueToKey(AdministratorInfo::remarks)));
    sqlQuery.exec(QString("CREATE TABLE %1("
                          "%2   timestamp(3)    NOT NULL PRIMARY KEY DEFAULT now(),"
                          "%3   varchar(32)     NOT NULL,"
                          "%4   varchar(32)     NOT NULL,"
                          "%5   uuid            NOT NULL,"
                          "%6   varchar(32)     NOT NULL,"
                          "%7   integer         NOT NULL)")
                  .arg(Singleton::enumName<AllocatedConsumables>(),
                       Singleton::enumValueToKey(AllocatedConsumables::createTime),
                       Singleton::enumValueToKey(AllocatedConsumables::type),
#if ENABLE_COMBINE_DEVICE
                       Singleton::enumValueToKey(AllocatedConsumables::uniqueId),
#else
                       Singleton::enumValueToKey(AllocatedConsumables::deviceId),
#endif
                       Singleton::enumValueToKey(AllocatedConsumables::agentId),
                       Singleton::enumValueToKey(AllocatedConsumables::adminId),
                       Singleton::enumValueToKey(AllocatedConsumables::count)));
#if ENABLE_COMBINE_DEVICE
    sqlQuery.exec(QString("CREATE TABLE %1("
                          "%2   varchar(32)     NOT NULL PRIMARY KEY,"
                          "%3   varchar(32)     NOT NULL,"
                          "%4   varchar(32)     NOT NULL,"
                          "%5   varchar(32)     NOT NULL,"
                          "%6   varchar(32)     NOT NULL,"
                          "%7   varchar(32)     NOT NULL,"
                          "%8   smallint        NOT NULL DEFAULT 0,"
                          "%9   integer         NOT NULL DEFAULT 0,"
                          "%10  integer         NOT NULL DEFAULT 0)")
                  .arg(Singleton::enumName<CombinedDevice>(),
                       Singleton::enumValueToKey(CombinedDevice::uniqueId),
                       Singleton::enumValueToKey(CombinedDevice::name),
                       Singleton::enumValueToKey(CombinedDevice::deviceId),
                       Singleton::enumValueToKey(CombinedDevice::macAddress),
                       Singleton::enumValueToKey(CombinedDevice::placeId),
                       Singleton::enumValueToKey(CombinedDevice::agentId),
                       Singleton::enumValueToKey(CombinedDevice::status),
                       Singleton::enumValueToKey(CombinedDevice::totalCount))
                  .arg(Singleton::enumValueToKey(CombinedDevice::usedCount)));
    sqlQuery.exec(QString("CREATE TABLE %1("
                          "%2   varchar(32)     NOT NULL PRIMARY KEY,"
                          "%3   varchar(32)     NOT NULL,"
                          "%4   varchar(64)     NOT NULL,"
                          "%5   smallint        NOT NULL DEFAULT 0,"
                          "%6   text            NOT NULL)")
                  .arg(Singleton::enumName<Device>(),
                       Singleton::enumValueToKey(Device::deviceId),
                       Singleton::enumValueToKey(Device::type),
                       Singleton::enumValueToKey(Device::batch),
                       Singleton::enumValueToKey(Device::status),
                       Singleton::enumValueToKey(Device::remarks)));
#else
    sqlQuery.exec(QString("CREATE TABLE %1("
                          "%2   varchar(32)     NOT NULL PRIMARY KEY,"
                          "%3   varchar(32)     NOT NULL,"
                          "%4   uuid            NOT NULL,"
                          "%5   uuid            NOT NULL,"
                          "%6   varchar(32)     NOT NULL,"
                          "%7   varchar(32)     NOT NULL,"
                          "%8   smallint        NOT NULL DEFAULT 0)")
                  .arg(Singleton::enumName<Device>(),
                       Singleton::enumValueToKey(Device::deviceId),
                       Singleton::enumValueToKey(Device::password),
                       Singleton::enumValueToKey(Device::placeId),
                       Singleton::enumValueToKey(Device::agentId),
                       Singleton::enumValueToKey(Device::adminId),
                       Singleton::enumValueToKey(Device::type),
                       Singleton::enumValueToKey(Device::status)));
#endif
#if ENABLE_COMBINE_DEVICE
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
#endif
    sqlQuery.exec(QString("CREATE TABLE %1("
                          "%2   timestamp(3)    NOT NULL,"
                          "%3   varchar(32)     NOT NULL,"
                          "%4   text            NOT NULL,"
                          "%5   smallint        NOT NULL DEFAULT 0,"
                          "%6   text            NOT NULL,"
                          "PRIMARY KEY(%2,%3))")
                  .arg(Singleton::enumName<ReportInfo>(),
                       Singleton::enumValueToKey(ReportInfo::reportTime),
#if ENABLE_COMBINE_DEVICE
                       Singleton::enumValueToKey(ReportInfo::uniqueId),
#else
                       Singleton::enumValueToKey(ReportInfo::deviceId),
#endif
                       Singleton::enumValueToKey(ReportInfo::name),
                       Singleton::enumValueToKey(ReportInfo::modify),
                       Singleton::enumValueToKey(ReportInfo::reportData)));
    sqlQuery.exec(QString("CREATE TABLE %1("
                          "%2   varchar(128)    NOT NULL PRIMARY KEY,"
                          "%3   varchar(32)     NOT NULL,"
                          "%4   varchar(32)     NOT NULL,"
                          "%5   varchar(255)    NOT NULL,"
                          "%6   text            NOT NULL,"
                          "%7   timestamp(3)    NOT NULL DEFAULT now())")
                  .arg(Singleton::enumName<SoftwareManagement>(),
                       Singleton::enumValueToKey(SoftwareManagement::appId),
                       Singleton::enumValueToKey(SoftwareManagement::name),
                       Singleton::enumValueToKey(SoftwareManagement::version),
                       Singleton::enumValueToKey(SoftwareManagement::downloadPath),
                       Singleton::enumValueToKey(SoftwareManagement::content),
                       Singleton::enumValueToKey(SoftwareManagement::createTime)));
    sqlQuery.exec(QString("CREATE TABLE %1("
                          "%2   uuid            NOT NULL PRIMARY KEY DEFAULT gen_random_uuid(),"
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
    auto id = topic.levels().at(2);
    switch (Singleton::enumKeyToValue<PrimaryTopic>(topic.levels().at(0))) {
    case PrimaryTopic::request:
        // respone to client or admin request and publish message
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

bool TopicAnalysis::legalAdminId(const QString &id)
{
    QSqlQuery sqlQuery(m_database);
    sqlQuery.exec(QString("SELECT * FROM %1 WHERE %2 = '%3'")
                  .arg(Singleton::enumName<AdministratorInfo>(),
                       Singleton::enumValueToKey(AdministratorInfo::adminId), id));
    return sqlQuery.next();
}

bool TopicAnalysis::legalDeviceId(const QString &id)
{
    QSqlQuery sqlQuery(m_database);
    sqlQuery.exec(QString("SELECT * FROM %1 WHERE %2 = '%3'")
                  .arg(Singleton::enumName<Device>(),
                       Singleton::enumValueToKey(Device::deviceId), id));
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
        sqlQuery.prepare(QString("SELECT *, "
                                 "(SELECT SUM(%1) AS totalCount FROM %2 WHERE %3 = ?), "
                                 "(SELECT COUNT(*) AS usedCount FROM %4 WHERE %3 = ?)"
                                 "FROM %5 WHERE %3 = ? AND %6 = ?")
                         .arg(Singleton::enumValueToKey(AllocatedConsumables::count),
                              Singleton::enumName<AllocatedConsumables>(),
                              Singleton::enumValueToKey(Device::deviceId),
                              Singleton::enumName<ReportInfo>(),
                              Singleton::enumName<Device>(),
                              Singleton::enumValueToKey(Device::password)));
        sqlQuery.addBindValue(id);
        sqlQuery.addBindValue(id);
        sqlQuery.addBindValue(id);
        sqlQuery.addBindValue(object.value(Singleton::enumValueToKey(Device::password)).toString());
        sqlQuery.exec();
        if (sqlQuery.next()) {
            data = Singleton::jsonToUtf8(Singleton::getJsonObject(sqlQuery));
        }
        else {
            data = QString("登陆失败，请确认信息是否正确！").toUtf8();
        }
        break;
    case SecondaryTopic::uploadData:
    {
        auto reportTimeString = Singleton::enumValueToKey(ReportInfo::reportTime);
        auto reportTime = object.value(reportTimeString);
        auto reportData = object.value(Singleton::enumValueToKey(ReportInfo::reportData));
        // insert valid data, if exist, ignore
        if (reportTime.type() != QJsonValue::Undefined && reportData != QJsonValue::Undefined) {
            databaseOperation(message, topic, DatabaseOperation::Insert);
            QJsonObject json;
            json.insert(reportTimeString, reportTime.toString());
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
                data = Singleton::jsonToUtf8(Singleton::getJsonObject(sqlQuery));
            }
        }
        else {
            emit error(MessageError::RequiredDataIsIncomplete);
        }
    }
        break;
    case SecondaryTopic::signIn:
    {
        auto pstr = Singleton::enumValueToKey(AdministratorInfo::password);
        auto password = object.value(pstr);
        if (password.type() != QJsonValue::Undefined) {
            sqlQuery.exec(QString("SELECT * FROM %1 WHERE %2 = '%3' AND %4 = '%5'")
                          .arg(Singleton::enumName<AdministratorInfo>(),
                               Singleton::enumValueToKey(AdministratorInfo::adminId),
                               id,
                               pstr,
                               password.toString()));
            if (sqlQuery.next()) {
                QJsonObject json {
                    { Singleton::enumName<UserStatus>(), Singleton::enumValueToKey(UserStatus::passwordCorrect) }
                };
                data = Singleton::jsonToUtf8(json);
            }
            else {
                QJsonObject json {
                    { Singleton::enumName<UserStatus>(), Singleton::enumValueToKey(UserStatus::passwordError) }
                };
                data = Singleton::jsonToUtf8(json);
            }
        }
        else {
            emit error(MessageError::RequiredDataIsIncomplete);
        }
    }
        break;
    case SecondaryTopic::loginIn:
    {
        auto astr = Singleton::enumValueToKey(AdministratorInfo::adminId);
        auto pstr = Singleton::enumValueToKey(AdministratorInfo::password);
        auto adminId = object.value(astr);
        auto password = object.value(pstr);
        if (adminId.type() != QJsonValue::Undefined && password.type() != QJsonValue::Undefined) {
            sqlQuery.exec(QString("SELECT %1 FROM %2 WHERE %3 = '%4' AND %5 = '%6'")
                          .arg(Singleton::enumValueToKey(AdministratorInfo::deviceIds),
                               Singleton::enumName<AdministratorInfo>(),
                               astr,
                               adminId.toString(),
                               pstr,
                               password.toString()));
            if (sqlQuery.next()) {
                QJsonObject json {
                    { Singleton::enumName<UserStatus>(), Singleton::enumValueToKey(UserStatus::passwordCorrect) },
                    { Singleton::enumValueToKey(ReportInfo::deviceId), sqlQuery.value(0).toString() }
                };
                data = Singleton::jsonToUtf8(json);
            }
            else {
                QJsonObject json {
                    { Singleton::enumName<UserStatus>(), Singleton::enumValueToKey(UserStatus::passwordError) }
                };
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
                                       QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
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
    case SecondaryTopic::consultation:
    {
        auto times = Singleton::jsonToString(QJsonDocument::fromJson(message).object()
                                             .value(Singleton::enumValueToKey(ReportInfo::reportTime)).toArray());
        times.replace("[", "(").replace("]", ")").replace("\"", "'");
        QSqlQuery sqlQuery(m_database);
        sqlQuery.exec(QString("SELECT %1, %2 FROM %3 WHERE %4 = '%5' AND %6 = '1' AND %7 IN %8")
                      .arg(Singleton::enumValueToKey(ReportInfo::reportTime),
                           Singleton::enumValueToKey(ReportInfo::reportData),
                           Singleton::enumName<ReportInfo>(),
                           Singleton::enumValueToKey(ReportInfo::deviceId),
                           id,
                           Singleton::enumValueToKey(ReportInfo::modify),
                           Singleton::enumValueToKey(ReportInfo::reportTime),
                           times));
        QJsonObject json;
        while (sqlQuery.next()) {
            auto reportData = Singleton::utf8ToJsonObject(sqlQuery.value(1).toString().toUtf8());
            json.insert(QString::number(sqlQuery.value(0).toDateTime().toMSecsSinceEpoch()),
                        QJsonObject
                        {
                            { "consultation", reportData.value("consultation").toObject() },
                            { "reportConclusion", reportData.value("reportConclusion").toString() }
                        });
        }
        data = Singleton::jsonToUtf8(json);
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
#if ENABLE_COMBINE_DEVICE
        dbOperation<ReportInfo>(object, type, Singleton::enumValueToKey(ReportInfo::uniqueId)
                                + "," + Singleton::enumValueToKey(ReportInfo::reportTime));
#else
        dbOperation<ReportInfo>(object, type, Singleton::enumValueToKey(ReportInfo::deviceId)
                                + "," + Singleton::enumValueToKey(ReportInfo::reportTime), topic);
#endif
        break;
    case SecondaryTopic::device:
        return dbOperation(object, type, Device::deviceId, topic);
        break;
#if ENABLE_COMBINE_DEVICE
    case SecondaryTopic::computer:
        return dbOperation(object, type, Computer::macAddress);
        break;
#endif
#if ENABLE_COMBINE_DEVICE
    case SecondaryTopic::combinedDevice:
        return dbOperation(object, type, CombinedDevice::uniqueId);
        break;
#endif
    case SecondaryTopic::allocatedConsumables:
        return dbOperation(object, type, AllocatedConsumables::createTime, topic);
        break;
    case SecondaryTopic::place:
        return dbOperation(object, type, PlaceInfo::placeId, topic);
        break;
    case SecondaryTopic::agent:
        return dbOperation(object, type, AgentInfo::agentId, topic);
        break;
    case SecondaryTopic::reports:
#if ENABLE_COMBINE_DEVICE
        return dbOperation(object, type, ReportInfo::uniqueId);
#else
        return dbOperation(object, type, ReportInfo::deviceId, topic);
#endif
        break;
    default:
        emit error(MessageError::NoAnalysis);
        break;
    }
    return QByteArray();
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
QByteArray TopicAnalysis::dbOperation(const QJsonObject &object, const DatabaseOperation &type,
                                      const QString &primaryKey, const QMqttTopicName &topic)
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
            sqlQuery.prepare(QString("SELECT * FROM %1 ORDER BY %2 DESC")
                             .arg(tableName, Singleton::enumKeys<T>().first()));
        }
        else {
            sqlQuery.prepare(QString("SELECT * FROM %1 WHERE %2 = ? ORDER BY %3 DESC")
                             .arg(tableName, primaryKey, Singleton::enumKeys<T>().first()));
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
        array.append(Singleton::getJsonObject(sqlQuery));
        auto data = Singleton::jsonToUtf8(array);
        if (data.length() > 524288) {
            emit messagePublish(Singleton::getTopicName(ResponseTopic::response, getSTopic(topic), topic.levels().at(2)), data);
            array = QJsonArray();
        }
    }
    if (!array.isEmpty())
        return Singleton::jsonToUtf8(array);
    return QByteArray();
}

template<class T>
QByteArray TopicAnalysis::dbOperation(const QJsonObject &object, const DatabaseOperation &type,
                                      const T &column, const QMqttTopicName &topic)
{
    return dbOperation<T>(object, type, Singleton::enumValueToKey(column), topic);
}
