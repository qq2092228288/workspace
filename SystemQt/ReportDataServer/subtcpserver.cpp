#include "subtcpserver.h"
#include "config.h"
#include "singleton.h"
#include "databasens.h"

using namespace DatabaseEnumNs;

SubTcpServer::SubTcpServer(QObject *parent)
    : QObject{parent}
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

int SubTcpServer::count() const
{
    return m_clientList.count();
}

void SubTcpServer::newSocketDescriptor(qintptr socketDescriptor)
{
    auto client = new TcpClientSocket(socketDescriptor, this);
    connect(client, &TcpClientSocket::send, this, &SubTcpServer::received);
    connect(this, &SubTcpServer::writeReady, client, &TcpClientSocket::writeReady);
    connect(client, &TcpClientSocket::disconnected, client, &TcpClientSocket::deleteLater);
    connect(client, &TcpClientSocket::destroyed, this, &SubTcpServer::clientDeleted);
    m_clientList.append(client);
}

void SubTcpServer::received(qintptr socketDescriptor, TelegramType type, const QByteArray &data)
{
    auto object = QJsonDocument::fromJson(data).object();
    QSqlQuery sqlQuery(m_db);
    switch (type) {
    case TelegramType::SoftwareInfomation:
    {
        auto appIdString = Singleton::enumValueToKey(SoftwareManagement::appId);
        auto appId = object.value(appIdString);
        if (appId.type() != QJsonValue::Undefined) {
            sqlQuery.exec(QString("SELECT * FROM %1 WHERE %2 = '%3'")
                              .arg(Singleton::enumName<SoftwareManagement>(),
                                   appIdString,
                                   appId.toString()));
            if (sqlQuery.next()) {
                sendWrite(socketDescriptor, TelegramType::SoftwareInfomation, Singleton::getJsonObject(sqlQuery));
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
                sendWrite(socketDescriptor, TelegramType::UserInfomation, json);
            }
            else {
                QJsonObject json {
                    { Singleton::enumName<UserStatus>(), Singleton::enumValueToKey(UserStatus::passwordError) }
                };
                sendWrite(socketDescriptor, TelegramType::UserInfomation, json);
            }
        }
        else {
            QJsonObject json {
                { Singleton::enumName<UserStatus>(), Singleton::enumValueToKey(UserStatus::incompleteData) }
            };
            sendWrite(socketDescriptor, TelegramType::UserInfomation, json);
        }
    }
    break;
    case TelegramType::ReportData:
    {
        auto dstr = Singleton::enumValueToKey(ReportInfo::deviceId);
        auto deviceId = object.value(dstr).toString().replace("{", "'").replace("}", "'");
        if (deviceId.isEmpty()) {
            return;
        }
        else if ("*" == deviceId) {
            sqlQuery.exec(QString("SELECT * FROM %1 ORDER BY %2 DESC")
                              .arg(Singleton::enumName<ReportInfo>(),
                                   Singleton::enumValueToKey(ReportInfo::reportTime)));
        }
        else {
            sqlQuery.exec(QString("SELECT * FROM %1 WHERE %2 IN (%3) ORDER BY %4 DESC")
                              .arg(Singleton::enumName<ReportInfo>(),
                                   Singleton::enumValueToKey(ReportInfo::deviceId),
                                   deviceId,
                                   Singleton::enumValueToKey(ReportInfo::reportTime)));
        }
        QJsonArray array;
        while (sqlQuery.next()) {
            auto json = Singleton::getJsonObject(sqlQuery);
            auto temp = array;
            temp.append(json);
            if (Singleton::jsonToUtf8(temp).length() > MAX_DATA_LENGTH) {
                sendWrite(socketDescriptor, TelegramType::ReportData, array);
                array = QJsonArray();
            }
            if (Singleton::jsonToUtf8(json).length() < MAX_DATA_LENGTH) {
                array.append(json);
            }
        }
        if (!array.isEmpty()) {
            sendWrite(socketDescriptor, TelegramType::ReportData, array);
        }
    }
        break;
    default:
        break;
    }
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
void SubTcpServer::sendWrite(qintptr socketDescriptor, TelegramType type, const T &json)
{
    emit writeReady(socketDescriptor, TProfile(type, Singleton::jsonToUtf8(json)).toByteArray());
}
