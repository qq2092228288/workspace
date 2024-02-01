#include "subtcpserver.h"
#include <QUuid>
#include "config.h"
#include "singleton.h"
#include "databasens.h"

using namespace DatabaseEnumNs;

SubTcpServer::SubTcpServer(const int index, QObject *parent)
    : QObject{parent}
    , m_index{index}
{
    qRegisterMetaType<qintptr>("qintptr");
    m_db = QSqlDatabase::addDatabase(Singleton::databaseType(), QUuid::createUuid().toString());
    m_db.setHostName(Singleton::hostname());
    m_db.setPort(Singleton::databasePort());
    m_db.setDatabaseName(Singleton::databaseName());
    m_db.setUserName(Singleton::userName());
    m_db.setPassword(Singleton::password());
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
    if (!m_db.open()) {
        TIME_DEBUG()<<m_db.lastError();
        return;
    }
    auto sqlQuery = new QSqlQuery(m_db);
    auto object = QJsonDocument::fromJson(data).object();
    switch (type) {
    case TelegramType::SoftwareInfomation:
    {
        auto appIdString = Singleton::enumValueToKey(SoftwareManagement::appId);
        auto appId = object.value(appIdString);
        if (appId.type() != QJsonValue::Undefined) {
            sqlQuery->exec(QString("SELECT * FROM %1 WHERE %2 = '%3'")
                              .arg(Singleton::enumName<SoftwareManagement>(),
                                   appIdString,
                                   appId.toString()));
            if (sqlQuery->next()) {
                clientWrite(socketDescriptor, TelegramType::SoftwareInfomation, Singleton::getJsonObject(*sqlQuery));
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
            sqlQuery->exec(QString("SELECT %1 FROM %2 WHERE %3 = '%4' AND %5 = '%6'")
                              .arg(Singleton::enumValueToKey(AdministratorInfo::deviceIds),
                                   Singleton::enumName<AdministratorInfo>(),
                                   astr,
                                   adminId.toString(),
                                   pstr,
                                   password.toString()));
            if (sqlQuery->next()) {
                QJsonObject json {
                    { Singleton::enumName<UserStatus>(), Singleton::enumValueToKey(UserStatus::passwordCorrect) },
                    { Singleton::enumValueToKey(ReportInfo::deviceId), sqlQuery->value(0).toString() }
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
        auto deviceId = object.value(Singleton::enumValueToKey(ReportInfo::deviceId))
                            .toString().replace("{", "'").replace("}", "'");
        if (!deviceId.isEmpty()) {
            auto tableName = Singleton::enumName<ReportInfo>();
            auto distr = Singleton::enumValueToKey(ReportInfo::deviceId);
            auto rtstr = Singleton::enumValueToKey(ReportInfo::reportTime);
            if (TelegramType::NewReportData == type) {
                auto time = QDateTime::currentDateTime().addDays(-1).toString("yyyy-MM-dd hh:mm:ss.zzz");
                if ("*" == deviceId) {
                    sqlQuery->exec(QString("SELECT * FROM %1 WHERE %2 > '%3' ORDER BY %2 DESC")
                                       .arg(tableName, rtstr, time));
                }
                else {
                    sqlQuery->exec(QString("SELECT * FROM %1 WHERE %2 IN (%3) AND %4 > '%5' ORDER BY %4 DESC")
                                       .arg(tableName, distr, deviceId, rtstr, time));
                }
            }
            else {
                if ("*" == deviceId) {
                    sqlQuery->exec(QString("SELECT * FROM %1 ORDER BY %2 DESC").arg(tableName, rtstr));
                }
                else {
                    sqlQuery->exec(QString("SELECT * FROM %1 WHERE %2 IN (%3) ORDER BY %4 DESC")
                                       .arg(tableName, distr, deviceId, rtstr));
                }
            }
            int dataCount = 0;
            QJsonArray array;
            while (sqlQuery->next()) {
                auto json = Singleton::getJsonObject(*sqlQuery);
                auto temp = array;
                temp.append(json);
                if (Singleton::jsonToUtf8(temp).length() > MAX_DATA_LENGTH) {
                    emit appendReports(socketDescriptor, array);
                    array = QJsonArray();
                    ++dataCount;
                }
                if (Singleton::jsonToUtf8(json).length() < MAX_DATA_LENGTH) {
                    array.append(json);
                }
            }
            if (!array.isEmpty()) {
                emit appendReports(socketDescriptor, array);
                ++dataCount;
            }
            clientWrite(socketDescriptor, TelegramType::ReportDataReady,
                        QJsonObject{ { "datacount" , dataCount } });
        }
    }
        break;
    default:
        break;
    }
    sqlQuery->clear();
    sqlQuery->finish();
    delete sqlQuery;
    sqlQuery = nullptr;
    m_db.close();
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
