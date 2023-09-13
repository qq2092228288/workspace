#include "mqttclient.h"
#include <singleton.h>
#include <databasens.h>
#include <QCoreApplication>
#include "reportset.h"
using namespace DatabaseEnumNs;

MqttClient::MqttClient(QObject *parent)
    : QObject{parent},
      m_appId{"92FB908B-79F3-4B31-9499-5E57A7239285"},
      m_databaseName{"Reports.db"},
      m_reportsTable{"reports"}
{
    qRegisterMetaType<QMqttTopicName>("QMqttTopicName");
    qRegisterMetaType<QMqttClient::ClientState>("ClientState");
    m_client = new QMqttClient(this);
    m_client->setHostname(Singleton::serverAddress());
    m_client->setPort(Singleton::mqttPort());
    connect(m_client, &QMqttClient::stateChanged, this, &MqttClient::stateChanged);
    connect(m_client, &QMqttClient::messageReceived, this, &MqttClient::messageReceived);
}

bool MqttClient::openDatabase()
{
    // 连接服务器
    m_client->connectToHost();
    // 建立和SQlite数据库的连接
    m_database = QSqlDatabase::addDatabase("QSQLITE", m_reportsTable);
    // 设置数据库文件的名字
    m_database.setDatabaseName(m_databaseName);
    if (m_database.open()) {
        // 用于执行sql语句的对象
        QSqlQuery sqlQuery(m_database);
        sqlQuery.exec(QString("CREATE TABLE %1("
                              "%2 REAL PRIMARY KEY NOT NULL, "
                              "%3 INT, "
                              "%4 TEXT)")
                      .arg(m_reportsTable,
                           Singleton::enumValueToKey(ReportTable::time),
                           Singleton::enumValueToKey(ReportTable::upload),
                           Singleton::enumValueToKey(ReportTable::data)));
        // 存储设备信息
        sqlQuery.exec(QString("CREATE TABLE %1("
                              "%2 VARCHAR(32) PRIMARY KEY NOT NULL,"
                              "%3 VARCHAR(32) NOT NULL,"
                              "%4 TEXT NOT NULL,"
                              "%5 TEXT NOT NULL,"
                              "%6 VARCHAR(32) NOT NULL,"
                              "%7 VARCHAR(32) NOT NULL,"
                              "%8 SMALLINT NOT NULL,"
                              "%9 INT NOT NULL,"
                              "%10 INT NOT NULL)")
                      .arg(Singleton::enumName<Device>(),
                           Singleton::enumValueToKey(Device::deviceId),
                           Singleton::enumValueToKey(Device::password),
                           Singleton::enumValueToKey(Device::placeId),
                           Singleton::enumValueToKey(Device::agentId),
                           Singleton::enumValueToKey(Device::adminId),
                           Singleton::enumValueToKey(Device::type),
                           Singleton::enumValueToKey(Device::status))
                      .arg(Singleton::enumValueToKey(CountType::totalCount),
                           Singleton::enumValueToKey(CountType::usedCount)));
        setLocalUserInfo();
        return true;
    }
    else {
        qWarning("数据库打开失败！");
    }
    return false;
}

bool MqttClient::deviceInfoIsEmpty()
{
    QSqlQuery sqlQuery(m_database);
    sqlQuery.exec(QString("SELECT %1, %2 FROM %3")
                  .arg(Singleton::enumValueToKey(Device::deviceId),
                       Singleton::enumValueToKey(Device::password),
                       Singleton::enumName<Device>()));
    return !sqlQuery.next();
}

void MqttClient::login(const QString &deviceId, const QString &password)
{
    if (m_client->state() == QMqttClient::Connected) {
        if (deviceId != m_deviceId) {
            // 取消订阅原ID
            m_client->unsubscribe(subTopic(m_deviceId));
        }
        m_client->subscribe(subTopic(deviceId));
        m_deviceId = deviceId;
        m_password = password;
        getDeviceInfo();
    }
    else {
        qWarning("服务器未连接！");
    }
}

void MqttClient::logout()
{
    QSqlQuery sqlQuery(m_database);
    if (sqlQuery.exec(QString("DELETE FROM %1").arg(Singleton::enumName<Device>()))){
        emit loggedOut();
    }
}

void MqttClient::insert(qint64 time, QString dataString)
{
    QSqlQuery sqlQuery(m_database);
    sqlQuery.prepare(QString("INSERT INTO %1(%2) VALUES(?, ?, ?)")
                     .arg(m_reportsTable, Singleton::enumKeys<ReportTable>().join(",")));
    sqlQuery.addBindValue(time);
    sqlQuery.addBindValue(0);
    sqlQuery.addBindValue(dataString);
    if (!sqlQuery.exec()) {
        qWarning("数据插入失败！");
        return;
    }
    uploadReport();
}

int MqttClient::surplus()
{
    QSqlQuery sqlQuery(m_database);
    sqlQuery.prepare(QString("SELECT %1 - %2 - (SELECT COUNT(*) FROM %3 WHERE %4 != ?) AS surplus FROM %5 WHERE %6=?")
                     .arg(Singleton::enumValueToKey(CountType::totalCount),
                          Singleton::enumValueToKey(CountType::usedCount),
                          m_reportsTable,
                          Singleton::enumValueToKey(ReportTable::upload),
                          Singleton::enumName<Device>(),
                          Singleton::enumValueToKey(Device::deviceId)));
    sqlQuery.addBindValue(ReportState::Uploaded);
    sqlQuery.addBindValue(m_deviceId);

    sqlQuery.exec();
    if (sqlQuery.next()) {
        return sqlQuery.value(0).toInt();
    }
    return -1;
}

void MqttClient::uploadReport()
{
    if (m_client->state() != QMqttClient::Connected) return;
    QSqlQuery sqlQuery(m_database);
    sqlQuery.exec(QString("SELECT * FROM %1 WHERE %2 < %3")
                  .arg(m_reportsTable,
                       Singleton::enumValueToKey(ReportTable::upload))
                  .arg(ReportState::Uploaded));
    if (sqlQuery.next()) {
        auto data = sqlQuery.value(Singleton::enumValueToKey(ReportTable::data)).toString();
        QJsonObject object;
        object.insert(Singleton::enumValueToKey(ReportInfo::reportTime),
                      QDateTime::fromMSecsSinceEpoch(sqlQuery.value(Singleton::enumValueToKey(ReportTable::time))
                                                     .toLongLong()).toString("yyyy-MM-dd hh:mm:ss.zzz"));
        object.insert(Singleton::enumValueToKey(ReportInfo::deviceId), m_deviceId);
        auto pInfo = Singleton::utf8ToJsonObject(data.toUtf8()).value("patientInfo").toObject();
        object.insert(Singleton::enumValueToKey(ReportInfo::name),
                      pInfo.value("medicalRecordNumber").toString() + "-" + pInfo.value("patientName").toString());
        object.insert(Singleton::enumValueToKey(ReportInfo::modify), 0);
        object.insert(Singleton::enumValueToKey(ReportInfo::reportData), data);

        publish(Singleton::getTopicName(PrimaryTopic::request, SecondaryTopic::uploadData, m_deviceId),
                          Singleton::jsonToUtf8(object),
                          2,
                          false);
        if (sqlQuery.next()) {
            QTimer::singleShot(10000, this, &MqttClient::uploadReport);
        }
    }
}

void MqttClient::getSoftwareInfo()
{
    QJsonObject object;
    object.insert(Singleton::enumValueToKey(SoftwareManagement::appId), m_appId);
    publish(Singleton::getTopicName(PrimaryTopic::request, SecondaryTopic::software, m_deviceId),
            Singleton::jsonToUtf8(object), 0, false);
}

void MqttClient::publish(const QMqttTopicName &topic, const QByteArray &message, quint8 qos, bool retain)
{
    m_client->publish(topic, message, qos, retain);
}

void MqttClient::getDeviceInfo()
{
    QJsonObject object;
    object.insert(Singleton::enumValueToKey(Device::password), m_password);
    publish(Singleton::getTopicName(PrimaryTopic::request, SecondaryTopic::deviceInfo, m_deviceId),
            Singleton::jsonToUtf8(object), 2, false);
}

void MqttClient::pullConclusion()
{
    QSqlQuery sqlQuery(m_database);
    sqlQuery.exec(QString("SELECT %1 FROM %2 WHERE %3 = %4")
                  .arg(Singleton::enumValueToKey(ReportTable::time),
                       m_reportsTable,
                       Singleton::enumValueToKey(ReportTable::upload))
                  .arg(ReportState::Uploaded));
    QJsonArray array;
    while (sqlQuery.next()) {
        array.append(QDateTime::fromMSecsSinceEpoch(sqlQuery.value(0).toLongLong()).toString("yyyy-MM-dd hh:mm:ss.zzz"));
    }
    if (!array.isEmpty()) {
        publish(Singleton::getTopicName(PrimaryTopic::request, SecondaryTopic::consultation, m_deviceId),
                          Singleton::jsonToUtf8(QJsonObject{{ Singleton::enumValueToKey(ReportInfo::reportTime), array }}),
                          2,
                          false);
    }
    else {
        emit pulled(PullState::NotPulling);
    }
}

void MqttClient::connectToHost()
{
    m_client->connectToHost();
}

void MqttClient::stateChanged(QMqttClient::ClientState state)
{
    if (QMqttClient::Connected == state) {
        emit connected();
        if (!m_deviceId.isEmpty()) {
            m_client->subscribe(subTopic(m_deviceId));
            getDeviceInfo();
            getSoftwareInfo();
            uploadReport();
        }
    }
    else if (QMqttClient::Disconnected == state) {
        // reconnect
        QTimer::singleShot(30*1000, this, &MqttClient::connectToHost);
    }
}

void MqttClient::messageReceived(const QByteArray &message, const QMqttTopicName &topic)
{
    if (topic.levelCount() != 3) {
        return;
    }
    if (message.isNull()) {
        return;
    }
    auto object = QJsonDocument::fromJson(message).object();
    if (object.isEmpty()) {
        emit messageFromServer(QString(message));
        return;
    }
    QSqlQuery sqlQuery(m_database);
    auto level2 = Singleton::enumKeyToValue<SecondaryTopic>(topic.levels().at(1));
    switch (SecondaryTopic(level2)) {
    case SecondaryTopic::deviceInfo:
    {
        // get device info
        QStringList strs;
        QStringList okeys = object.keys();
        foreach (auto str, okeys) {
            strs<<(":" + str);
        }
        sqlQuery.prepare(QString("REPLACE INTO %1(%2) VALUES(%3)")
                         .arg(Singleton::enumName<Device>(),
                              okeys.join(", "),
                              strs.join(", ")));
        bindValue(sqlQuery, object);
        if (sqlQuery.exec()) {
            emit deviceInfoReceived(true);
            emit surplusCount(surplus());
        }
        else {
            emit deviceInfoReceived(false);
        }
    }
        break;
    case SecondaryTopic::uploadData:
    {
        // server received
        auto reportTime = object.value(Singleton::enumValueToKey(ReportInfo::reportTime)).toString();
        if (!reportTime.isEmpty()) {
            auto time = QDateTime::fromString(reportTime, "yyyy-MM-dd hh:mm:ss.zzz").toMSecsSinceEpoch();
            if (!sqlQuery.exec(QString("UPDATE %1 SET %2 = %3 WHERE %4 = %5")
                               .arg(m_reportsTable,
                                    Singleton::enumValueToKey(ReportTable::upload))
                               .arg(ReportState::Uploaded)
                               .arg(Singleton::enumValueToKey(ReportTable::time))
                               .arg(time))) {
                qWarning("数据更新失败！");
            }
        }
        // retrieve
        getDeviceInfo();
    }
        break;
    case SecondaryTopic::software:
    {
        // received software info
        if (compareVersion(object.value(Singleton::enumValueToKey(SoftwareManagement::version)).toString(),
                           QCoreApplication::applicationVersion())) {
            emit newVerion(object);
        }
    }
        break;
    case SecondaryTopic::consultation:
        foreach (auto key, object.keys()) {
            sqlQuery.exec(QString("SELECT %1 FROM %2 WHERE %3 = '%4'")
                          .arg(Singleton::enumValueToKey(ReportTable::data),
                               m_reportsTable,
                               Singleton::enumValueToKey(ReportTable::time),
                               key));
            if (sqlQuery.next()) {
                auto data = Singleton::utf8ToJsonObject(sqlQuery.value(0).toString().toUtf8());
                data.insert(ReportDataName::ekey(ReportDataName::reportConclusion), object.value(key).toString());
                sqlQuery.exec(QString("UPDATE %1 SET %2 = '%3', %4 = '%5' WHERE %6 = '%7'")
                              .arg(m_reportsTable,
                                   Singleton::enumValueToKey(ReportTable::data),
                                   Singleton::jsonToString(data),
                                   Singleton::enumValueToKey(ReportTable::upload))
                              .arg(ReportState::Modified)
                              .arg(Singleton::enumValueToKey(ReportTable::time),
                                   key));
            }
        }
        emit pulled( object.isEmpty() ? PullState::NoData : PullState::Pulled);
        break;
    default:
        break;
    }
}

QMqttTopicFilter MqttClient::subTopic(const QString &deviceId)
{
    return QMqttTopicFilter(Singleton::enumValueToKey(ResponseTopic::response) + "/+/" + deviceId);
}

bool MqttClient::compareVersion(QString ver1, QString ver2)
{
    QStringList list1 = ver1.replace(QRegExp("[a-zA-Z() ]"), "").split(".");
    QStringList list2 = ver2.replace(QRegExp("[a-zA-Z() ]"), "").split(".");
    if (list1.count() == list2.count()) {
        for (int index = 0; index < list1.count(); ++index) {
            if (list1.at(index).toInt() > list2.at(index).toInt()) {
                return true;
            }
            else if (list1.at(index).toInt() < list2.at(index).toInt()) {
                return false;
            }
        }
    }
    else {
        return true;
    }
    return false;
}

void MqttClient::bindValue(QSqlQuery &sqlQuery, const QJsonObject &object)
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

void MqttClient::setLocalUserInfo()
{
    QSqlQuery sqlQuery(m_database);
    sqlQuery.exec(QString("SELECT %1, %2 FROM %3")
                  .arg(Singleton::enumValueToKey(Device::deviceId),
                       Singleton::enumValueToKey(Device::password),
                       Singleton::enumName<Device>()));
    if (sqlQuery.next()) {
        m_deviceId = sqlQuery.value(0).toString();
        m_password = sqlQuery.value(1).toString();
    }
}
