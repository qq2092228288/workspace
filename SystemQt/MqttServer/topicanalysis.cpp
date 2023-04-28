#include "topicanalysis.h"
#include "singleton.h"

TopicAnalysis::TopicAnalysis(QObject *parent)
    : QObject{parent}
{
    m_database = QSqlDatabase::addDatabase("QODBC");
    m_database.setHostName("localhost");
    m_database.setPort(3306);
    m_database.setDatabaseName("mysql");    // ODBC DSN name
    m_database.setUserName("admin");
    m_database.setPassword("zeyao2022");
}

void TopicAnalysis::createTables()
{
    DatabaseManager databaseManager(m_database);
    QSqlQuery sqlQuery(m_database);
    QString setutf8 = QString("ENGINE=InnoDB DEFAULT character set = utf8");
    sqlQuery.exec(QString("CREATE TABLE IF NOT EXISTS %1("
                          "%2   char(32)        NOT NULL PRIMARY KEY,"
                          "%3   varchar(32)     NOT NULL,"
                          "%4   varchar(128)    NOT NULL,"
                          "%5   varchar(128)    NOT NULL,"
                          "%6   longtext)%7")
                  .arg(Singleton::enumName<AgentInfo>(),
                       Singleton::enumValueToKey(AgentInfo::agentId),
                       Singleton::enumValueToKey(AgentInfo::name),
                       Singleton::enumValueToKey(AgentInfo::contact),
                       Singleton::enumValueToKey(AgentInfo::address),
                       Singleton::enumValueToKey(AgentInfo::remarks), setutf8));
    sqlQuery.exec(QString("CREATE TABLE IF NOT EXISTS %1("
                          "%2   varchar(32)     NOT NULL PRIMARY KEY,"
                          "%3   varchar(32)     NOT NULL,"
                          "%4   varchar(32)     NOT NULL,"
                          "%5   tinyint         NOT NULL DEFAULT 0,"
                          "%6   longtext)%7")
                  .arg(Singleton::enumName<AdministratorInfo>(),
                       Singleton::enumValueToKey(AdministratorInfo::adminId),
                       Singleton::enumValueToKey(AdministratorInfo::password),
                       Singleton::enumValueToKey(AdministratorInfo::name),
                       Singleton::enumValueToKey(AdministratorInfo::permission),
                       Singleton::enumValueToKey(AdministratorInfo::remarks), setutf8));
    sqlQuery.exec(QString("CREATE TABLE IF NOT EXISTS %1("
                          "%2   char(32)        NOT NULL PRIMARY KEY,"
                          "%3   varchar(32)     NOT NULL,"
                          "%4   char(32)        NOT NULL,"
                          "%5   char(32)        NOT NULL,"
                          "%6   varchar(32)     NOT NULL,"
                          "%7   int             NOT NULL,"
                          "%8   datetime(3)     NOT NULL)%9")
                  .arg(Singleton::enumName<AllocatedConsumables>(),
                       Singleton::enumValueToKey(AllocatedConsumables::consumablesId),
                       Singleton::enumValueToKey(AllocatedConsumables::type),
                       Singleton::enumValueToKey(AllocatedConsumables::uniqueId),
                       Singleton::enumValueToKey(AllocatedConsumables::agentId),
                       Singleton::enumValueToKey(AllocatedConsumables::adminId),
                       Singleton::enumValueToKey(AllocatedConsumables::count),
                       Singleton::enumValueToKey(AllocatedConsumables::time), setutf8));
    sqlQuery.exec(QString("CREATE TABLE IF NOT EXISTS %1("
                          "%2   char(32)        NOT NULL PRIMARY KEY,"
                          "%3   varchar(32)     NOT NULL,"
                          "%4   char(32)        NOT NULL,"
                          "%5   char(32)        NOT NULL,"
                          "%6   tinyint         NOT NULL DEFAULT 0,"
                          "%7   int             NOT NULL DEFAULT 0,"
                          "%8   int             NOT NULL DEFAULT 0)%9")
                  .arg(Singleton::enumName<CombinedDevice>(),
                       Singleton::enumValueToKey(CombinedDevice::uniqueId),
                       Singleton::enumValueToKey(CombinedDevice::name),
                       Singleton::enumValueToKey(CombinedDevice::placeId),
                       Singleton::enumValueToKey(CombinedDevice::agentId),
                       Singleton::enumValueToKey(CombinedDevice::status),
                       Singleton::enumValueToKey(CombinedDevice::totalCount),
                       Singleton::enumValueToKey(CombinedDevice::usedCount), setutf8));
    sqlQuery.exec(QString("CREATE TABLE IF NOT EXISTS %1("
                          "%2   char(32)        NOT NULL PRIMARY KEY,"
                          "%3   varchar(32)     NOT NULL,"
                          "%4   varchar(64)     NOT NULL,"
                          "%5   tinyint         NOT NULL DEFAULT 0,"
                          "%6   longtext)%7")
                  .arg(Singleton::enumName<Device>(),
                       Singleton::enumValueToKey(Device::deviceId),
                       Singleton::enumValueToKey(Device::type),
                       Singleton::enumValueToKey(Device::batch),
                       Singleton::enumValueToKey(Device::status),
                       Singleton::enumValueToKey(Device::remarks), setutf8));
    sqlQuery.exec(QString("CREATE TABLE IF NOT EXISTS %1("
                          "%2   char(17)        NOT NULL PRIMARY KEY,"
                          "%3   varchar(32)     NOT NULL,"
                          "%4   varchar(32)     NOT NULL,"
                          "%5   varchar(64)     NOT NULL,"
                          "%6   tinyint         NOT NULL DEFAULT 0,"
                          "%7   longtext)%8")
                  .arg(Singleton::enumName<Computer>(),
                       Singleton::enumValueToKey(Computer::macAddress),
                       Singleton::enumValueToKey(Computer::type),
                       Singleton::enumValueToKey(Computer::systemVersion),
                       Singleton::enumValueToKey(Computer::batch),
                       Singleton::enumValueToKey(Computer::status),
                       Singleton::enumValueToKey(Computer::remarks), setutf8));
    sqlQuery.exec(QString("CREATE TABLE IF NOT EXISTS %1("
                          "%2   char(32)        NOT NULL,"
                          "%3   datetime(3)     NOT NULL,"
                          "%4   longtext        NOT NULL,"
                          "PRIMARY KEY(%2,%3))%5")
                  .arg(Singleton::enumName<ReportInfo>(),
                       Singleton::enumValueToKey(ReportInfo::uniqueId),
                       Singleton::enumValueToKey(ReportInfo::reportTime),
                       Singleton::enumValueToKey(ReportInfo::reportData), setutf8));
    sqlQuery.exec(QString("CREATE TABLE IF NOT EXISTS %1("
                          "%2   varchar(128)    NOT NULL PRIMARY KEY,"
                          "%3   varchar(32)     NOT NULL,"
                          "%4   varchar(32)     NOT NULL,"
                          "%5   varchar(255)    NOT NULL,"
                          "%6   text            NOT NULL,"
                          "%7   datetime(3)     NOT NULL)%8")
                  .arg(Singleton::enumName<SoftwareManagement>(),
                       Singleton::enumValueToKey(SoftwareManagement::appId),
                       Singleton::enumValueToKey(SoftwareManagement::name),
                       Singleton::enumValueToKey(SoftwareManagement::version),
                       Singleton::enumValueToKey(SoftwareManagement::downloadPath),
                       Singleton::enumValueToKey(SoftwareManagement::content),
                       Singleton::enumValueToKey(SoftwareManagement::time), setutf8));
    sqlQuery.exec(QString("CREATE TABLE IF NOT EXISTS %1("
                          "%2   char(32)        NOT NULL PRIMARY KEY,"
                          "%3   varchar(128)    NOT NULL,"
                          "%4   varchar(128)    NOT NULL)%5")
                  .arg(Singleton::enumName<PlaceInfo>(),
                       Singleton::enumValueToKey(PlaceInfo::placeId),
                       Singleton::enumValueToKey(PlaceInfo::hostName),
                       Singleton::enumValueToKey(PlaceInfo::secondaryName), setutf8));
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
        DatabaseManager databaseManager(m_database);

        auto id = topic.levels().at(2);
        auto sTopic = SecondaryTopic(level2);

        switch (PrimaryTopic(level1)) {
        case PrimaryTopic::request:
            // respone to client or admin request and publish message
            request(message, sTopic, id);
            break;
        case PrimaryTopic::append:
            // admin append new data
            databaseOperation(message, sTopic, DatabaseOperation::Insert);
            break;
        case PrimaryTopic::update:
            // admin update data
            databaseOperation(message, sTopic, DatabaseOperation::Update);
            break;
        case PrimaryTopic::remove:
            // admin remove data
            databaseOperation(message, sTopic, DatabaseOperation::Delete);
            break;
        default:
            emit error(MessageError::NoAnalysis);
            break;
        }
    }
    else {
        emit error(MessageError::TopicInvalid);
    }
    emit error(MessageError::NoError);
}

void TopicAnalysis::request(const QByteArray &message, const SecondaryTopic &sTopic, const QString &id)
{
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
        auto reportTimeString = Singleton::enumValueToKey(ReportInfo::reportTime);
        auto reportDataString = Singleton::enumValueToKey(ReportInfo::reportData);
        auto reprotTime = object.value(reportTimeString);
        auto reprotData = object.value(reportDataString);
        // insert valid data, if exist, ignore
        if (reprotTime.type() != QJsonValue::Undefined && reprotData != QJsonValue::Undefined) {
            databaseOperation(message, sTopic, DatabaseOperation::Insert);
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
    case SecondaryTopic::reports:

        break;
    default:
        emit error(MessageError::NoAnalysis);
        break;
    }
    if (!data.isNull()) {
        emit messagePublish(Singleton::getTopicName(PrimaryTopic::response, sTopic, id), data);
    }
}

void TopicAnalysis::databaseOperation(const QByteArray &message, const SecondaryTopic &sTopic,
                                      const DatabaseOperation &type)
{
    auto object = QJsonDocument::fromJson(message).object();
    switch (sTopic) {
    case SecondaryTopic::uploadData:
        dbOperation<ReportInfo>(object, type, ReportInfo::uniqueId);
        break;
    case SecondaryTopic::device:
        dbOperation<Device>(object, type, Device::deviceId);
        break;
    case SecondaryTopic::computer:
        dbOperation<Computer>(object, type, Computer::macAddress);
        break;
    case SecondaryTopic::combinedDevice:
        dbOperation<CombinedDevice>(object, type, CombinedDevice::uniqueId);
        break;
    case SecondaryTopic::allocatedConsumables:
        dbOperation<AllocatedConsumables>(object, type, AllocatedConsumables::consumablesId);
        break;
    case SecondaryTopic::place:
        dbOperation<PlaceInfo>(object, type, PlaceInfo::placeId);
        break;
    case SecondaryTopic::agent:
        dbOperation<AgentInfo>(object, type, AgentInfo::agentId);
        break;
    default:
        emit error(MessageError::NoAnalysis);
        break;
    }
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

template<class T, class T1>
void TopicAnalysis::dbOperation(const QJsonObject &object, const DatabaseOperation &type, const T1 &column)
{
    QStringList keys = Singleton::enumKeys<T>();
    keys.sort();
    QStringList okeys = object.keys();
    okeys.sort();
    if (keys != okeys) {
        emit error(MessageError::RequiredDataIsIncomplete);
        return;
    }
    QString columnName = Singleton::enumValueToKey(column);
    QSqlQuery sqlQuery(m_database);
    auto tableName = Singleton::enumName<T>();
    switch (type) {
    case DatabaseOperation::Insert:
    {
        QStringList strs;
        foreach (auto str, okeys) {
            strs<<(":" + str);
        }
        sqlQuery.prepare(QString("INSERT IGNORE INTO %1(%2) VALUES(%3)")
                         .arg(tableName, okeys.join(", "), strs.join(", ")));
        bindValue(sqlQuery, object);
        break;
    }
    case DatabaseOperation::Update:
    {
        QStringList strs;
        foreach (auto str, okeys) {
            strs<<QString("%1=:%1").arg(str);
        }
        sqlQuery.prepare(QString("UPDATE %1 SET %2 WHERE %3 = :%3")
                         .arg(tableName, strs.join(", "), columnName));
        bindValue(sqlQuery, object);
    }
        break;
    case DatabaseOperation::Delete:
        sqlQuery.prepare(QString("DELETE FROM %1 WHERE %2 = ?").arg(tableName, columnName));
        sqlQuery.addBindValue(object.value(columnName).toString());
        break;
    }
    sqlQuery.exec();
}


