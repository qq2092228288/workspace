#include "mqttclient.h"
#include "singleton.h"
#include "databasens.h"
#include "threadservice.h"
#include <QNetworkInterface>
#include <QNetworkProxy>
#include <QCoreApplication>
#include <QTimer>

using namespace DatabaseEnumNs;

QSqlDatabase &MqttClient::db()
{
    return m_db;
}

void MqttClient::connectToHost()
{
    m_client->connectToHost();
}

void MqttClient::login(const QString &username, const QString &password)
{
    m_username = username;
    m_deviceIds.clear();
    QJsonObject object {
        { Singleton::enumValueToKey(AdministratorInfo::adminId), username },
        { Singleton::enumValueToKey(AdministratorInfo::password), password }
    };
    m_client->publish(Singleton::getTopicName(PrimaryTopic::request, SecondaryTopic::loginIn, m_mac),
                      Singleton::jsonToUtf8(object), 2, false);
}

void MqttClient::getReports()
{
    if (m_deviceIds.isEmpty()) {
        return;
    }
    if ("*" == m_deviceIds) {
        getReport(m_deviceIds);
    }
    else {
        auto list = m_deviceIds.split(",");
        foreach (auto deviceId, list) {
            getReport(deviceId);
        }
    }
}

void MqttClient::serverConnected()
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
    m_client->subscribe(QMqttTopicFilter(Singleton::enumValueToKey(ResponseTopic::response) + "/+/" + m_mac));
    m_client->publish(Singleton::getTopicName(PrimaryTopic::request, SecondaryTopic::software, m_mac),
                      Singleton::jsonToUtf8(QJsonObject {
                                                { Singleton::enumValueToKey(SoftwareManagement::appId), m_appId }
                                            }), 0, false);
}

void MqttClient::messageReceived(const QByteArray &message, const QMqttTopicName &topic)
{
    auto json = QJsonDocument::fromJson(message).object();
    switch (Singleton::enumKeyToValue<SecondaryTopic>(topic.levels().at(1))) {
    case SecondaryTopic::loginIn:
        m_deviceIds = json.value(Singleton::enumValueToKey(ReportInfo::deviceId))
                .toString().remove("{").remove("}");
        emit loginStatus(json);
        break;
    case SecondaryTopic::software:
        if (compareVersion(json.value(Singleton::enumValueToKey(SoftwareManagement::version)).toString(),
                           QCoreApplication::applicationVersion())) {
            emit newVerion(json);
        }
        break;
    case SecondaryTopic::reports:
    {
        auto array = QJsonDocument::fromJson(message).array();
        foreach (auto value, array) {
            auto object = value.toObject();
            QSqlQuery query(m_db);
            query.prepare(QString("REPLACE INTO %1(%2) VALUES(:%3)")
                       .arg(Singleton::enumName<ReportInfo>(), object.keys().join(", "), object.keys().join(", :")));
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
    }
        break;
    default:
        break;
    }

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

void MqttClient::getReport(const QString &deviceId)
{
    QJsonObject object {
        { Singleton::enumValueToKey(ReportInfo::reportTime), QJsonValue() },
        { Singleton::enumValueToKey(ReportInfo::deviceId), deviceId },
        { Singleton::enumValueToKey(ReportInfo::name), QJsonValue() },
        { Singleton::enumValueToKey(ReportInfo::modify), QJsonValue() },
        { Singleton::enumValueToKey(ReportInfo::reportData), QJsonValue() }
    };
    m_client->publish(Singleton::getTopicName(PrimaryTopic::select, SecondaryTopic::reports, m_mac),
                      Singleton::jsonToUtf8(object), 2, false);
}

MqttClient::MqttClient(QObject *parent)
    : QObject{parent},
      m_appId{"08D41DD0-E740-4006-BD1C-132B2891042B"}
{
    foreach (auto net, QNetworkInterface::allInterfaces()) {
        auto flags = net.flags();
        m_mac = net.hardwareAddress().remove(QRegExp("[^0-9A-Za-z]"));
        if (flags.testFlag(QNetworkInterface::IsRunning) && flags.testFlag(QNetworkInterface::IsUp) &&
            !flags.testFlag(QNetworkInterface::IsLoopBack)) {
            if (!net.humanReadableName().contains("VMware") && !net.humanReadableName().contains("Loopback")) {
                break;
            }
        }
    }
    if(QSqlDatabase::contains("qt_sql_default_connection"))
        m_db = QSqlDatabase::database("qt_sql_default_connection");
    else
        m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(":memory:");

    m_client = new QMqttClient;
    ThreadService::getInstance()->objectMoveToThread(m_client);
    m_client->setHostname(Singleton::serverAddress());
    m_client->setPort(Singleton::mqttPort());

    connect(m_client, &QMqttClient::disconnected, this, &MqttClient::connectToHost);
    connect(m_client, &QMqttClient::connected, this, &MqttClient::serverConnected);
    connect(m_client, &QMqttClient::stateChanged, this, &MqttClient::stateChanged);
    connect(m_client, &QMqttClient::messageReceived, this, &MqttClient::messageReceived);
}

MqttClient::~MqttClient()
{
    if (m_db.isOpen()) {
        m_db.close();
    }
    delete m_client;
}

MqttClient *MqttClient::getInstance()
{
    static MqttClient instance;
    return &instance;
}
