#include "subtcpserver.h"
#include <QUuid>

#include "singleton.h"
#include "databasens.h"

using namespace DatabaseEnumNs;

SubTcpServer::SubTcpServer(const int index, QObject *parent)
    : QObject{parent}
    , m_index{index}
    , m_reportInfoStr{Singleton::enumName<ReportInfo>()}
    , m_reportTimeStr{Singleton::enumValueToKey(ReportInfo::reportTime)}
    , m_deviceIdStr{Singleton::enumValueToKey(ReportInfo::deviceId)}
{
    qRegisterMetaType<qintptr>("qintptr");
    m_db = QSqlDatabase::addDatabase(Singleton::databaseType(), QUuid::createUuid().toString());
    m_db.setHostName(Singleton::hostname());
    m_db.setPort(Singleton::databasePort());
    m_db.setDatabaseName(Singleton::databaseName());
    m_db.setUserName(Singleton::userName());
    m_db.setPassword(Singleton::password());
    if (!m_db.open()) {
        TIME_DEBUG()<<m_db.lastError();
    }
}

SubTcpServer::~SubTcpServer()
{
    m_db.close();
}

int SubTcpServer::index() const
{
    return m_index;
}

int SubTcpServer::count() const
{
    return m_clientList.count();
}

void SubTcpServer::newSocketDescriptor(qintptr socketDescriptor, int index)
{
    if (index != m_index) return;
    auto client = new TcpClientSocket(socketDescriptor, this);
    connect(client, &TcpClientSocket::send, this, &SubTcpServer::received);
    connect(this, &SubTcpServer::writeReady, client, &TcpClientSocket::writeReady);
    connect(this, &SubTcpServer::appendReports, client, &TcpClientSocket::appendReports);
    connect(client, &TcpClientSocket::disconnected, client, &TcpClientSocket::deleteLater);
    connect(client, &TcpClientSocket::destroyed, this, &SubTcpServer::clientDeleted);
    m_clientList.append(client);
}

void SubTcpServer::received(qintptr socketDescriptor, TelegramType type, const QByteArray &data)
{
    // auto sqlQuery = new QSqlQuery(m_db);
    QSqlQuery sqlQuery(m_db);
    sqlQuery.setForwardOnly(true);
    auto object = QJsonDocument::fromJson(data).object();
    switch (type) {
    case TelegramType::SoftwareInfomation:
    {
        auto appIdString = Singleton::enumValueToKey(SoftwareManagement::appId);
        auto appId = object.value(appIdString);
        if (appId.type() != QJsonValue::Undefined) {
            sqlQuery.prepare(QString("SELECT * FROM %1 WHERE %2 = :%2")
                                  .arg(Singleton::enumName<SoftwareManagement>(), appIdString));
            sqlQuery.addBindValue(appId.toString());
            sqlQuery.exec();
            if (sqlQuery.next()) {
                clientWrite(socketDescriptor, TelegramType::SoftwareInfomation, Singleton::getJsonObject(sqlQuery));
            }
        }
    }
        break;
    case TelegramType::UserInfomation:
    {
        auto astr = Singleton::enumValueToKey(AdministratorInfo::adminId);
        auto pstr = Singleton::enumValueToKey(AdministratorInfo::password);
        auto adminId = object.value(astr);
        auto password = object.value(pstr);
        if (adminId.type() != QJsonValue::Undefined && password.type() != QJsonValue::Undefined) {
            sqlQuery.prepare(QString("SELECT %1 FROM %2 WHERE %3 = :%3 AND %4 = :%4")
                                  .arg(Singleton::enumValueToKey(AdministratorInfo::deviceIds),
                                       Singleton::enumName<AdministratorInfo>(), astr, pstr));
            sqlQuery.addBindValue(adminId.toString());
            sqlQuery.addBindValue(password.toString());
            sqlQuery.exec();
            if (sqlQuery.next()) {
                QJsonObject json {
                    { Singleton::enumName<UserStatus>(), Singleton::enumValueToKey(UserStatus::passwordCorrect) },
                    { Singleton::enumValueToKey(ReportInfo::deviceId), sqlQuery.value(0).toString() }
                };
                clientWrite(socketDescriptor, TelegramType::UserInfomation, json);
            }
            else {
                QJsonObject json {
                    { Singleton::enumName<UserStatus>(), Singleton::enumValueToKey(UserStatus::passwordError) }
                };
                clientWrite(socketDescriptor, TelegramType::UserInfomation, json);
            }
        }
        else {
            QJsonObject json {
                { Singleton::enumName<UserStatus>(), Singleton::enumValueToKey(UserStatus::incompleteData) }
            };
            clientWrite(socketDescriptor, TelegramType::UserInfomation, json);
        }
    }
    break;
    case TelegramType::ReportData:
    case TelegramType::NewReportData:
    {
        auto deviceId = object.value(m_deviceIdStr).toString().replace("{", "'").replace("}", "'");
        if (!deviceId.isEmpty()) {
            if (TelegramType::NewReportData == type) {
                auto time = QDateTime::currentDateTime().addDays(-1).toString("yyyy-MM-dd hh:mm:ss.zzz");
                if ("*" == deviceId) {
                    sqlQuery.prepare(QString("SELECT %1, %2 FROM %3 WHERE %1 > :%1 ORDER BY %1 DESC")
                                          .arg(m_reportTimeStr, m_deviceIdStr, m_reportInfoStr));
                    sqlQuery.addBindValue(time);
                }
                else {
                    sqlQuery.prepare(QString("SELECT %1, %2 FROM %3 WHERE %2 IN (%4) AND %1 > :%1 ORDER BY %1 DESC")
                                          .arg(m_reportTimeStr, m_deviceIdStr, m_reportInfoStr, deviceId));
                    sqlQuery.addBindValue(time);
                }
            }
            else {
                if ("*" == deviceId) {
                    sqlQuery.prepare(QString("SELECT %1, %2 FROM %3 ORDER BY %1 DESC")
                                          .arg(m_reportTimeStr, m_deviceIdStr, m_reportInfoStr));
                }
                else {
                    sqlQuery.prepare(QString("SELECT %1, %2 FROM %3 WHERE %2 IN (%4) ORDER BY %1 DESC")
                                          .arg(m_reportTimeStr, m_deviceIdStr, m_reportInfoStr, deviceId));
                }
            }
            sqlQuery.exec();
            QJsonArray reportArray;
            while (sqlQuery.next()) {
                reportArray.append(QString("%1&%2").arg(sqlQuery.value(0).toDateTime()
                                                            .toString("yyyy-MM-dd hh:mm:ss.zzz"),
                                                        sqlQuery.value(1).toString()));
            }
            clientWrite(socketDescriptor, TelegramType::ReportDataReady, reportArray);
        }
    }
        break;
    case TelegramType::ReportDataRequest:
        sqlQuery.prepare(QString("SELECT * FROM %1 WHERE %2 = :%2 AND %3 = :%3")
                          .arg(m_reportInfoStr, m_reportTimeStr, m_deviceIdStr));
        sqlQuery.addBindValue(object.value(m_reportTimeStr).toString());
        sqlQuery.addBindValue(object.value(m_deviceIdStr).toString());
        sqlQuery.exec();
        if (sqlQuery.next()) {
            clientWrite(socketDescriptor, TelegramType::ReportSent, Singleton::getJsonObject(sqlQuery));
        }
        break;
    default:
        break;
    }
    sqlQuery.clear();
    sqlQuery.finish();
    // delete sqlQuery;
    // sqlQuery = nullptr;
}

void SubTcpServer::clientDeleted(QObject *client)
{
    for (int i = 0, length = m_clientList.length(); i < length; ++i) {
        if (m_clientList.at(i) == client) {
            m_clientList.removeAt(i);
            break;
        }
    }
}

template <class T>
void SubTcpServer::clientWrite(qintptr socketDescriptor, TelegramType type, const T &json)
{
    emit writeReady(socketDescriptor, TelegramProfile(type, Singleton::jsonToUtf8(json)).toByteArray());
}
