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
    write(TelegramProfile(type, data).toByteArray());
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
    auto info = TelegramProfile::baseInfo(m_data);
    if (info.index != -1 && info.length <= m_data.length()) {
        auto tp = TelegramProfile::fromUtf8(m_data.mid(info.index, info.length));
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
        case TelegramType::ReportDataReady:
        {
            auto array = Singleton::utf8ToJsonArray(tp.bodyData());
            auto rstr = Singleton::enumValueToKey(ReportInfo::reportTime);
            auto dstr = Singleton::enumValueToKey(ReportInfo::deviceId);
            for (int i = 0; i < array.count(); ++i) {
                auto value = array.at(i).toString();
                auto index = value.indexOf("&");
                m_queue.enqueue(Singleton::jsonToUtf8(QJsonObject {
                    {rstr, value.mid(0, index)},
                    {dstr, value.mid(index + 1)}
                }));
            }
            emit reportReady(array.count());
            requestReport();
        }
            break;
        case TelegramType::ReportSent:
        {
            auto object = Singleton::utf8ToJsonObject(tp.bodyData());
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
            emit reportPulled();
            requestReport();
        }
            break;
        default:
            break;
        }
    }
}

void TcpClientSocket::requestReport()
{
    if (!m_queue.isEmpty()) {
        writeReady(TelegramType::ReportDataRequest, m_queue.dequeue());
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
    if (m_db.isOpen()) {
        m_db.close();
    }
    disconnect(this, &TcpClientSocket::connected, this, &TcpClientSocket::connectedSlot);
    disconnect(this, &TcpClientSocket::readyRead, this, &TcpClientSocket::dataReceived);
    disconnect(this, &TcpClientSocket::disconnected, this, &TcpClientSocket::connectToServer);
}

TcpClientSocket *TcpClientSocket::getInstance()
{
    static TcpClientSocket instance;
    return &instance;
}
