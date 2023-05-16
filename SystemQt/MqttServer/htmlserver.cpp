#include "htmlserver.h"
#include "singleton.h"
#include "databasens.h"

using namespace DatabaseEnumNs;

HtmlServer::HtmlServer(QObject *parent)
    : QObject{parent},
      server_ptr{new QWebSocketServer(QStringLiteral("Html Server"), QWebSocketServer::NonSecureMode)},
      clientWrapper_ptr{new WebSocketClientWrapper(server_ptr.get())},
      channel_ptr{new QWebChannel}
{

}

void HtmlServer::startListening()
{
    if (!server_ptr->listen(QHostAddress::Any, Singleton::listenPort())) {
        qFatal("Failed to open web socket server.");
        exit(1) ;
    }
    else {
        TIME_DEBUG()<<"The server is listening on port: "<<Singleton::listenPort();
    }
    QObject::connect(clientWrapper_ptr.get(), &WebSocketClientWrapper::clientConnected,
                     channel_ptr.get(), &QWebChannel::connectTo);
    // 'core' is called by the HTML client
    channel_ptr->registerObject(QStringLiteral("core"), this);
}

void HtmlServer::loginUi()
{
    emit sendText((xmlToJsonString(":/xml/loginUi.xml")));
}

void HtmlServer::login(const QString &adminId, const QString &password)
{
    if (adminId.isEmpty() || password.isEmpty())
    {
        emit sendText("用户名和密码不能为空");
        return;
    }
    auto db = Singleton::getInstance()->database();
    QSqlQuery sqlQuery(db);
    auto aidstr = Singleton::enumValueToKey(AdministratorInfo::adminId);
    auto pwstr = Singleton::enumValueToKey(AdministratorInfo::password);
    sqlQuery.prepare(QString("SELECT * FROM %1 WHERE %2 = ? AND %3 = ?")
                     .arg(Singleton::enumName<AdministratorInfo>(), aidstr, pwstr));
    sqlQuery.addBindValue(adminId);
    sqlQuery.addBindValue(password);
    sqlQuery.exec();
    if (sqlQuery.next()) {
        auto userInfo = Singleton::getJsonObject(sqlQuery, columnNames(sqlQuery.record()));
        emit sendUserInfo(userInfo);
        mainUi(userInfo);
    }
    else {
        emit sendText("密码错误");
    }
}

void HtmlServer::currentTable(const QJsonObject &object, const QString &table)
{
    mainUi(object, table);
}

void HtmlServer::mainUi(const QJsonObject &object, QString tableName)
{
    if (tableName.isEmpty()) tableName = Singleton::enumName<ReportInfo>();
    auto db = Singleton::getInstance()->database();
    QSqlQuery sqlQuery(db);
    auto permission = UserPermissions(object.value(Singleton::enumValueToKey(AdministratorInfo::permission)).toInt(-1));
    QString html = htmlJsonString("<div class='list-wrapper'>names</div><div class='data-wrapper'>datas</div>");
    QStringList tableNames;
    switch (permission) {
    case UserPermissions::SuperAdministrator:
        tableNames<<Singleton::enumName<AgentInfo>()
                  <<Singleton::enumName<AdministratorInfo>()
                  <<Singleton::enumName<AllocatedConsumables>()
                  <<Singleton::enumName<CombinedDevice>()
                  <<Singleton::enumName<Device>()
                  <<Singleton::enumName<Computer>()
                  <<Singleton::enumName<ReportInfo>()
                  <<Singleton::enumName<SoftwareManagement>()
                  <<Singleton::enumName<PlaceInfo>();
        break;
    case UserPermissions::SecondaryAdministrator:
        tableNames<<Singleton::enumName<AgentInfo>()
                  <<Singleton::enumName<AllocatedConsumables>()
                  <<Singleton::enumName<CombinedDevice>()
                  <<Singleton::enumName<Device>()
                  <<Singleton::enumName<Computer>()
                  <<Singleton::enumName<ReportInfo>()
                  <<Singleton::enumName<PlaceInfo>();
        break;
    case UserPermissions::ReportSelectAndModify:
        tableNames<<Singleton::enumName<ReportInfo>();
        break;
    case UserPermissions::ReportSelect:
        tableNames<<Singleton::enumName<ReportInfo>();
        break;
    default:
        emit sendText("权限错误");
        return;
    }
    html.replace("names", tableDivs(tableNames));
    sqlQuery.exec(QString("SELECT * FROM %1").arg(tableName));
    html.replace("datas", dataDivs(sqlQuery, tableName));
    emit sendText(html);
}

QStringList HtmlServer::columnNames(const QSqlRecord &record)
{
    QStringList names;
    for (int var = 0; var < record.count(); ++var) {
        names<<record.fieldName(var);
    }
    return names;
}
/**
 * clicking on btn means selecting the corresponding table,
 * id is an attribute of btn,
 * userInfo from html,
 * currentTable is htmlserver slots;
**/
QString HtmlServer::tableDivs(const QStringList &tableNames)
{
    QString divs;

    QString func = QString("\\\"(function(id){"
                           "var elements = document.getElementsByClassName('btn');"
                           "let array = [];"
                           "Array.prototype.forEach.call(elements, function (element) {"
                           "    array.push(element.id);"
                           "});"
                           "core.currentTable(userInfo, id);"
                           "})(this.id)\\\"");
    foreach (auto tableName, tableNames) {
        divs += QString("<div class='action'><div class='btn' id='%1' onclick=%2>%3</div></div>")
                .arg(tableName, func, EnumTextCN::cn_EnumName(tableName));
    }
    return divs;
}

QString HtmlServer::dataDivs(QSqlQuery &sqlQuery, const QString &tableName)
{
    // table
    QString divs = QString("<table class='table' border='2'>");
    // table caption
    divs += QString("<caption class='cap'>%1</caption>").arg(EnumTextCN::cn_EnumName(tableName));
    QStringList keys = columnNames(sqlQuery.record());
    // headers
    divs += QString("<tr>");
    foreach (auto key, keys) {
        divs += QString("<th>%1</th>").arg(EnumTextCN::cn_EnumValue(tableName, key));
    }
    divs += QString("</tr>");
    // data
    while (sqlQuery.next()) {
        QString div = QString("<tr>");
        foreach (auto key, keys) {
            auto value = sqlQuery.value(key);
            switch (value.type()) {
            case QVariant::Type::Int:
                div += QString("<td>%1</td>").arg(value.toInt());
                break;
            case QVariant::Type::Double:
                div += QString("<td>%1</td>").arg(value.toDouble());
                break;
            case QVariant::Type::Bool:
                div += QString("<td>%1</td>").arg(value.toBool());
                break;
            case QVariant::Type::DateTime:
                div += QString("<td>%1</td>").arg(value.toDateTime().toString("yyyy-MM-ddThh:mm:ss.zzz"));
                break;
            default:
                div += QString("<td>%1</td>").arg(value.toString());
                break;
            }
        }
        div += QString("</tr>");
        divs += div;
    }
    divs += QString("</table>");
    return divs;
}

QString HtmlServer::xmlToJsonString(const QString &xmlPath)
{
    QFile file(xmlPath);
    if (!file.open(QFile::ReadOnly)) {
        return QString();
    }
    QString data = QString(file.readAll());
    file.close();
    return htmlJsonString(data);
}

QString HtmlServer::htmlJsonString(const QString &data)
{
    QJsonObject object;
    object.insert("html", data.simplified());
    return Singleton::jsonToString(object);
}
