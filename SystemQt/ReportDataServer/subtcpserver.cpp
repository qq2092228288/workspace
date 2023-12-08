#include "subtcpserver.h"
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

void SubTcpServer::sendWrite(qintptr socketDescriptor, TelegramType type, const QJsonObject &json)
{
    emit writeReady(socketDescriptor, TProfile(type, Singleton::jsonToUtf8(json)).toByteArray());
}

void SubTcpServer::received(qintptr socketDescriptor, TelegramType type, const QJsonObject &data)
{
    qDebug()<<socketDescriptor<<type<<data;
    QSqlQuery sqlQuery(m_db);
    switch (type) {
    case TelegramType::UserInfomation:
    {
        auto astr = Singleton::enumValueToKey(AdministratorInfo::adminId);
        auto pstr = Singleton::enumValueToKey(AdministratorInfo::password);
        auto adminId = data.value(astr);
        auto password = data.value(pstr);
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
        auto deviceId = data.value(dstr).toString().replace("{", "'").replace("}", "'");
        qDebug()<<dstr<<deviceId;
        if (deviceId.isEmpty()) {
            return;
        }
        else if ("*" == deviceId){
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
        while (sqlQuery.next()) {
            qDebug()<<"report data";
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
