#include "tcpclientsocket.h"
#include <QThread>
#include "databasesingleton.h"
#include "singleton.h"
#include "databasens.h"

using namespace DatabaseEnumNs;

TcpClientSocket::TcpClientSocket(qintptr socketDescriptor, QObject *parent)
    : QTcpSocket{parent}
{
    setSocketDescriptor(socketDescriptor);
    connect(this, &TcpClientSocket::readyRead, this, &TcpClientSocket::dataReceived);
    connect(this, &TcpClientSocket::disconnected, this, &TcpClientSocket::disconnectedSlot);
    connect(this, &TcpClientSocket::disconnected, this, &TcpClientSocket::deleteLater);
}

TcpClientSocket::~TcpClientSocket()
{
    qDebug()<<__FUNCTION__;
}

void TcpClientSocket::disconnectedSlot()
{
    emit breakLink(socketDescriptor());
}

void TcpClientSocket::dataReceived()
{
    m_data.append(readAll());
    auto info = TProfile::baseInfo(m_data);
    if (info.index != -1 && info.length <= m_data.length()) {
        auto tp = TProfile::fromUtf8(m_data.mid(info.index, info.length));
        m_data = m_data.mid(info.index + info.length);
        if (tp.telegramType() != TelegramType::InvalidType) {
            handling(tp.telegramType(), QJsonDocument::fromJson(tp.bodyData()).object());
        }
    }
}

void TcpClientSocket::handling(TelegramType type, const QJsonObject &data)
{
    if (data.empty()) {
        return;
    }
    auto db = DatabaseSingleton::getInstance()->db();
    if (!db.open()) {
        TIME_DEBUG()<<db.lastError();
        return;
    }
    QSqlQuery sqlQuery(db);
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
                writeReady(TelegramType::UserInfomation, QJsonObject {
                    { Singleton::enumName<UserStatus>(), Singleton::enumValueToKey(UserStatus::passwordCorrect) },
                    { Singleton::enumValueToKey(ReportInfo::deviceId), sqlQuery.value(0).toString() }
                });
            }
            else {
                writeReady(TelegramType::UserInfomation, QJsonObject {
                    { Singleton::enumName<UserStatus>(), Singleton::enumValueToKey(UserStatus::passwordError) }
                });
            }
        }
        else {
            writeReady(TelegramType::UserInfomation, QJsonObject {
                { Singleton::enumName<UserStatus>(), Singleton::enumValueToKey(UserStatus::incompleteData) }
            });
        }
    }
    break;
    case TelegramType::ReportData:
        break;
    default:
        break;
    }
    if (db.isOpen()) {
        db.close();
    }
}

void TcpClientSocket::writeReady(TelegramType type, const QJsonObject &json)
{
    write(TProfile(type, Singleton::jsonToUtf8(json)).toByteArray());
}
