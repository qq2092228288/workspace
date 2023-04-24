#include "topicanalysis.h"


TopicAnalysis::TopicAnalysis(QObject *parent)
    : QObject{parent}
{
    m_database = QSqlDatabase::addDatabase("QODBC");
    m_database.setHostName("127.0.0.1");
    m_database.setPort(3306);
    m_database.setDatabaseName("mysql");    // ODBC DSN name
    m_database.setUserName("root");
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
                       Singleton::enumValueToKey(AdministratorInfo::amdinId),
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
                          "%8   tinyint         NOT NULL DEFAULT 0,"
                          "%9   bigint          NOT NULL DEFAULT 0,"
                          "%10  bigint          NOT NULL DEFAULT 0)%11")
                  .arg(Singleton::enumName<AllocatedConsumables>(),
                       Singleton::enumValueToKey(AllocatedConsumables::consumablesId),
                       Singleton::enumValueToKey(AllocatedConsumables::type),
                       Singleton::enumValueToKey(AllocatedConsumables::uniqueId),
                       Singleton::enumValueToKey(AllocatedConsumables::agentId),
                       Singleton::enumValueToKey(AllocatedConsumables::adminId),
                       Singleton::enumValueToKey(AllocatedConsumables::count),
                       Singleton::enumValueToKey(AllocatedConsumables::status),
                       Singleton::enumValueToKey(AllocatedConsumables::time),
                       Singleton::enumValueToKey(AllocatedConsumables::receiveTime), setutf8));
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
                          "%3   bigint          NOT NULL,"
                          "%4   longtext,"
                          "PRIMARY KEY(%5,%6))%7")
                  .arg(Singleton::enumName<ReportInfo>(),
                       Singleton::enumValueToKey(ReportInfo::uniqueId),
                       Singleton::enumValueToKey(ReportInfo::reportTime),
                       Singleton::enumValueToKey(ReportInfo::data),
                       Singleton::enumValueToKey(ReportInfo::uniqueId),
                       Singleton::enumValueToKey(ReportInfo::reportTime), setutf8));
    sqlQuery.exec(QString("CREATE TABLE IF NOT EXISTS %1("
                          "%2   varchar(128)    NOT NULL PRIMARY KEY,"
                          "%3   varchar(32)     NOT NULL,"
                          "%4   varchar(32)     NOT NULL,"
                          "%5   varchar(255)    NOT NULL,"
                          "%6   text            NOT NULL,"
                          "%7   bigint          NOT NULL)%8")
                  .arg(Singleton::enumName<SoftwareManagement>(),
                       Singleton::enumValueToKey(SoftwareManagement::appId),
                       Singleton::enumValueToKey(SoftwareManagement::name),
                       Singleton::enumValueToKey(SoftwareManagement::version),
                       Singleton::enumValueToKey(SoftwareManagement::path),
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
            break;
        case PrimaryTopic::update:
            // admin update data
            break;
        case PrimaryTopic::remove:
            // admin remove data
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

void TopicAnalysis::request(const QByteArray &message, const SecondaryTopic &sTopic, const QString &uniqueId)
{
    QSqlQuery sqlQuery(m_database);
    switch (sTopic) {
    case SecondaryTopic::deviceInfo:
        sqlQuery.exec(QString("SELECT * FROM %1 WHERE %2 = '%3'")
                      .arg(Singleton::enumName<CombinedDevice>(),
                           Singleton::enumValueToKey(CombinedDevice::uniqueId),
                           uniqueId));
        if (sqlQuery.next()) {
            auto topic = Singleton::getTopicName(PrimaryTopic::response, sTopic, uniqueId);
            auto data = Singleton::jsonToUtf8(getJsonObject(sqlQuery, Singleton::enumKeys<CombinedDevice>()));
            emit messagePublish(topic, data);
        }
        break;
    case SecondaryTopic::uploadData:
        break;
    default:
        break;
    }
}

QJsonObject TopicAnalysis::getJsonObject(const QSqlQuery &sqlQuery, const QStringList &keys)
{
    QJsonObject object;
    foreach (auto key, keys) {
        auto value = sqlQuery.value(key);
        switch (value.type()) {
        case QVariant::Type::String:
            object.insert(key, value.toString());
            break;
        case QVariant::Type::Int:
            object.insert(key, value.toInt());
            break;
        case QVariant::Type::Double:
            object.insert(key, value.toDouble());
            break;
        case QVariant::Type::Bool:
            object.insert(key, value.toBool());
            break;
        default:
            object.insert(key, value.toString());
            break;
        }
    }
    return object;
}


