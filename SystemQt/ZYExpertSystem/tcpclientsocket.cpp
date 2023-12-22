#include "tcpclientsocket.h"
#include "singleton.h"
#include "databasens.h"

using namespace DatabaseEnumNs;

QSqlDatabase &TcpClientSocket::db()
{
    return m_db;
}

void TcpClientSocket::connectToServer()
{
    connectToHost(Singleton::serverAddress(), 18080);
}

void TcpClientSocket::getSoftwareInfo()
{
    QJsonObject json {
        { Singleton::enumValueToKey(SoftwareManagement::appId), m_appId }
    };
    writeReady(TelegramType::SoftwareInfomation, Singleton::jsonToUtf8(json));
}

void TcpClientSocket::login(const QString &username, const QString &password)
{
    m_userInfo.clear();
    QJsonObject json {
        { Singleton::enumValueToKey(AdministratorInfo::adminId), username },
        { Singleton::enumValueToKey(AdministratorInfo::password), password }
    };
    writeReady(TelegramType::UserInfomation, Singleton::jsonToUtf8(json));
}

void TcpClientSocket::getReports()
{
    writeReady(TelegramType::ReportData, m_userInfo);
}

void TcpClientSocket::getNewReports()
{
    writeReady(TelegramType::NewReportData, m_userInfo);
}

void TcpClientSocket::writeReady(const TelegramType &type, const QByteArray &data)
{
    write(TProfile(type, data).toByteArray());
}

void TcpClientSocket::connectedSlot()
{
    if (m_db.open()) {
        QSqlQuery query(m_db);
        query.exec(QString("CREATE TABLE IF NOT EXISTS %1("
                           "%2   timestamp(3)    NOT NULL,"
                           "%3   varchar(32)     NOT NULL,"
                           "%4   text            NOT NULL,"
                           "%5   smallint        NOT NULL DEFAULT 0,"
                           "%6   text            NOT NULL,"
                           "PRIMARY KEY(%2, %3))")
                       .arg(Singleton::enumName<ReportInfo>(),
                            Singleton::enumValueToKey(ReportInfo::reportTime),
                            Singleton::enumValueToKey(ReportInfo::deviceId),
                            Singleton::enumValueToKey(ReportInfo::name),
                            Singleton::enumValueToKey(ReportInfo::modify),
                            Singleton::enumValueToKey(ReportInfo::reportData)));
    }
}

void TcpClientSocket::dataReceived()
{
    m_data.append(readAll());
    auto info = TProfile::baseInfo(m_data);
    if (info.index != -1 && info.length <= m_data.length()) {
        auto tp = TProfile::fromUtf8(m_data.mid(info.index, info.length));
        m_data = m_data.mid(info.index + info.length);
        switch (tp.telegramType()) {
        case TelegramType::SoftwareInfomation:
        {
            auto object = QJsonDocument::fromJson(tp.bodyData()).object();
            auto version = object.value(Singleton::enumValueToKey(SoftwareManagement::version)).toString();
            if (Singleton::compareVersion(version, QCoreApplication::applicationVersion())) {
                emit versionInfo(object);
            }
        }
            break;
        case TelegramType::UserInfomation:
            m_userInfo = tp.bodyData();
            emit userInfo(QJsonDocument::fromJson(tp.bodyData()).object());
            break;
        case TelegramType::ReportData:
        {
            auto array = QJsonDocument::fromJson(tp.bodyData()).array();
            foreach (auto value, array) {
                auto object = value.toObject();
                QSqlQuery query(m_db);
                query.prepare(QString("REPLACE INTO %1(%2) VALUES(:%3)")
                                  .arg(Singleton::enumName<ReportInfo>(),
                                       object.keys().join(", "),
                                       object.keys().join(", :")));
                for (auto it = object.constBegin(); it != object.constEnd(); ++it) {
                    auto value = it.value();
                    if (QJsonValue::Double == value.type()) {
                        query.bindValue(":" + it.key(), value.toDouble());
                    }
                    else {
                        query.bindValue(":" + it.key(), value.toString());
                    }
                }
                query.exec();
            }
            emit reportPulled();
            writeReady(TelegramType::ReportDataRequest, QByteArray());
        }
            break;
        case TelegramType::ReportDataReady:
            writeReady(TelegramType::ReportDataRequest, QByteArray());
            emit reportReady(Singleton::utf8ToJsonObject(tp.bodyData()).value("datacount").toInt());
            break;
        default:
            break;
        }
    }
}

TcpClientSocket::TcpClientSocket(QObject *parent)
    : QTcpSocket{parent}
    , m_appId{"08D41DD0-E740-4006-BD1C-132B2891042B"}
{
    if(QSqlDatabase::contains("qt_sql_default_connection"))
        m_db = QSqlDatabase::database("qt_sql_default_connection");
    else
        m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(":memory:");
    connect(this, &TcpClientSocket::connected, this, &TcpClientSocket::connectedSlot);
    connect(this, &TcpClientSocket::readyRead, this, &TcpClientSocket::dataReceived);
    connect(this, &TcpClientSocket::disconnected, this, &TcpClientSocket::connectToServer);
}

TcpClientSocket::~TcpClientSocket()
{

}

TcpClientSocket *TcpClientSocket::getInstance()
{
    static TcpClientSocket instance;
    return &instance;
}
