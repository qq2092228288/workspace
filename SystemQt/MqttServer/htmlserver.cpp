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
    QSqlQuery sqlQuery(Singleton::getInstance()->database());
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
        mainUi(userInfo, Singleton::enumName<ReportInfo>());
    }
    else {
        emit sendText("密码错误");
    }
}

void HtmlServer::currentTable(const QJsonObject &object, const QString &table)
{
    mainUi(object, table);
}

void HtmlServer::execSqlStatement(const QJsonObject &object, const QString &tableName, const QString &statement)
{
    if (userInfoCheck(object)) {
        auto permission = UserPermissions(object.value(Singleton::enumValueToKey(AdministratorInfo::permission)).toInt(-1));
        switch (permission) {
        case UserPermissions::SuperAdministrator:
            break;
        case UserPermissions::SecondaryAdministrator:
            break;
        case UserPermissions::ReportSelectAndModify:
            break;
        case UserPermissions::ReportSelect:
            break;
        default:
            emit sendText("权限错误");
            break;
        }
        QSqlQuery sqlQuery(Singleton::getInstance()->database());
        sqlQuery.exec(statement);
        QString lasterror = sqlQuery.lastError().text();
        if (!lasterror.isEmpty()) {
            emit sendText(lasterror);
        }
        else {
            mainUi(object, tableName);
        }
    }
}

bool HtmlServer::userInfoCheck(const QJsonObject &userInfo)
{
    QSqlQuery sqlQuery(Singleton::getInstance()->database());
    QString w = Singleton::jsonToString(userInfo).replace("\":", "=").replace(",\"", " AND ")
            .replace("{\"", "").replace("}", "").replace("\"", "'");
    sqlQuery.exec(QString("SELECT * FROM %1 WHERE %2").arg(Singleton::enumName<AdministratorInfo>(), w));
    bool result = sqlQuery.next();
    if (!result) emit sendText("用户信息已修改, 请重新登录!");
    return result;
}
/**
 * tableList => database tables
 * tableTitle => table name(CN)
 * tableHeaders => table headers
 * dataList => table data
**/
void HtmlServer::mainUi(const QJsonObject &object, QString tableName)
{
    if (!userInfoCheck(object)) return;
    QSqlQuery sqlQuery(Singleton::getInstance()->database());
    auto permission = UserPermissions(object.value(Singleton::enumValueToKey(AdministratorInfo::permission)).toInt(-1));
    QStringList tableNames;
    switch (permission) {
    case UserPermissions::SuperAdministrator:
        tableNames<<Singleton::enumName<SoftwareManagement>();
    case UserPermissions::SecondaryAdministrator:
        tableNames<<Singleton::enumName<AgentInfo>()
                  <<Singleton::enumName<AdministratorInfo>()
                  <<Singleton::enumName<AllocatedConsumables>()
                  <<Singleton::enumName<CombinedDevice>()
                  <<Singleton::enumName<Device>()
                  <<Singleton::enumName<Computer>()
                  <<Singleton::enumName<ReportInfo>()
                  <<Singleton::enumName<PlaceInfo>();
        break;
    case UserPermissions::ReportSelectAndModify:
    case UserPermissions::ReportSelect:
        tableNames<<Singleton::enumName<ReportInfo>();
        break;
    default:
        emit sendText("权限错误");
        return;
    }

    if (EnumTextCN::compareEname<ReportInfo>(tableName)) {
        QStringList columns = Singleton::enumKeys<ReportInfo>();
        columns.removeLast();
        if (UserPermissions::ReportSelectAndModify == permission || UserPermissions::ReportSelect == permission) {
            QString uniqueIds = object.value(Singleton::enumValueToKey(AdministratorInfo::uniqueIds)
                                             .toLower()).toString().replace("{","'").replace("}","'");
            sqlQuery.exec(QString("SELECT %1 FROM %2 WHERE %3 IN(%4) ORDER BY %5 desc")
                          .arg(columns.join(","),
                               tableName,
                               Singleton::enumValueToKey(ReportInfo::uniqueId),
                               uniqueIds,
                               Singleton::enumValueToKey(ReportInfo::reportTime)));
        }
        else {
            sqlQuery.exec(QString("SELECT %1 FROM %2 ORDER BY %3 desc")
                          .arg(columns.join(","), tableName, Singleton::enumValueToKey(ReportInfo::reportTime)));
        }
    }
    else if (EnumTextCN::compareEname<AdministratorInfo>(tableName)) {
        sqlQuery.exec(QString("SELECT * FROM %1 WHERE %2>%3")
                      .arg(tableName, Singleton::enumValueToKey(AdministratorInfo::permission))
                      .arg(static_cast<int>(permission)));
    }
    else {
        sqlQuery.exec(QString("SELECT * FROM %1").arg(tableName));
    }
    // html
    QStringList mhtml;
    mhtml<<QString("<div class='list-wrapper'>");
    // left list
    foreach (auto tableName, tableNames) {
        mhtml<<QString("<div class='action'><div class='btn' id='%1' onclick='tablesBtnClick(this.id)'>%3</div></div>")
               .arg(tableName, EnumTextCN::cn_EnumName(tableName));
    }
    mhtml<<QString("</div>");

    // right data
    mhtml<<QString("<div class='data-wrapper'>");
    // table
    mhtml<<QString("<table class='table' border='2'>");
    // caption
    mhtml<<QString("<caption class='cap'>%1</caption>").arg(EnumTextCN::cn_EnumName(tableName));
    // thead
    mhtml<<QString("<thead>");
    mhtml<<QString("<tr>");
    // headers
    auto record = sqlQuery.record();
    for (int i = 0; i < record.count(); ++i) {
        mhtml<<QString("<th id='%1'>%2</th>").arg(record.fieldName(i), EnumTextCN::cn_EnumValue(tableName, record.fieldName(i)));
    }
    mhtml<<QString("<th width='95px'>操作</th>");
    mhtml<<QString("</tr>");
    mhtml<<QString("</thead>");
    // tbody
    mhtml<<QString("<tbody onclick='tbodyClick(event)'>");
    // data
    bool uneditable = false;
    if (EnumTextCN::compareEname<ReportInfo>(tableName)) {
        uneditable = true;
    }
    if (!uneditable) {  // this row is insert data
        mhtml<<QString("<tr>");
        for (int i = 0; i < record.count(); ++i) {
            if (uneditable || record.fieldName(i).indexOf("time", 0, Qt::CaseInsensitive) >= 0 ||
                    (i == 0 && (record.fieldName(i).compare(Singleton::enumValueToKey(AgentInfo::agentId), Qt::CaseInsensitive) == 0 ||
                                record.fieldName(i).compare(Singleton::enumValueToKey(PlaceInfo::placeId), Qt::CaseInsensitive) == 0))) {
                mhtml<<QString("<td align='center'>自动生成</td>");
            }
            else {
                mhtml<<QString("<td contenteditable='true'></td>");
            }
        }
        mhtml<<QString("<td class='cell' width='100px'><button id='append' class='cell-btn'>新增</button></td>");
        mhtml<<QString("</tr>");
    }
    if (EnumTextCN::compareEname<AllocatedConsumables>(tableName)) {
        uneditable = true;
    }
    while (sqlQuery.next()) {
        mhtml<<QString("<tr>");
        for (int i = 0; i < record.count(); ++i) {
            auto value = sqlQuery.value(i);
            QString property = QString(" contenteditable='%1'").arg(uneditable || 0 == i ? "false" : "true");
            switch (value.type()) {
            case QVariant::Type::Int:
                mhtml<<QString("<td%1>%2</td>").arg(property).arg(value.toInt());
                break;
            case QVariant::Type::Double:
                mhtml<<QString("<td%1>%2</td>").arg(property).arg(value.toDouble());
                break;
            case QVariant::Type::Bool:
                mhtml<<QString("<td%1>%2</td>").arg(property).arg(value.toBool());
                break;
            case QVariant::Type::DateTime:
                mhtml<<QString("<td>%1</td>").arg(value.toDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz"));
                break;
            default:
                if (!uneditable && i == 1 && record.fieldName(i).compare(Singleton::enumValueToKey(AdministratorInfo::password), Qt::CaseInsensitive) == 0)
                    mhtml<<QString("<td>%1</td>").arg(value.toString());
                else mhtml<<QString("<td%1>%2</td>").arg(property, value.toString());
                break;
            }
        }
        mhtml<<QString("<td class='cell' width='100px'>");
        switch (permission) {
        case UserPermissions::SuperAdministrator:
            mhtml<<QString("<button id='delete' class='cell-btn'>删除</button>");
        case UserPermissions::SecondaryAdministrator:
            if (!(uneditable || EnumTextCN::compareEname<AllocatedConsumables>(tableName)))
                mhtml<<QString(" <button id='update' class='cell-btn'>修改</button>");
        case UserPermissions::ReportSelectAndModify:
        case UserPermissions::ReportSelect:
            if (uneditable)  mhtml<<QString(" <button id='show' class='cell-btn'>查看</button>");
            break;
        }
        mhtml<<QString("</td>");
        mhtml<<QString("</tr>");
    }
    mhtml<<QString("</tbody>");
    mhtml<<QString("</table>");
    mhtml<<QString("</div>");
    emit sendText(htmlJsonString(mhtml.join("")));
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
    foreach (auto tableName, tableNames) {
        divs += QString("<div class='action'><div class='btn' id='%1' onclick='tablesBtnClick(this.id)'>%3</div></div>")
                .arg(tableName, EnumTextCN::cn_EnumName(tableName));
    }
    return divs;
}

QString HtmlServer::dataList(QSqlQuery &sqlQuery, const int &permission, bool show)
{
    QString dataList;
    auto per = UserPermissions(permission);
    if (!show && (UserPermissions::SuperAdministrator == per || UserPermissions::SecondaryAdministrator == per)) {
        dataList = QString("<tr>");
        for (int index = 0; index < sqlQuery.record().count(); ++index) {
            dataList += QString("<td contenteditable='true'></td>");
        }
        dataList += QString("<td class='cell' width='100px'><button id='append' class='cell-btn'>新增</button></td></tr>");
    }
    while (sqlQuery.next()) {
        // one row
        QString tr("<tr>");
        for (int index = 0; index < sqlQuery.record().count(); ++index) {
            auto value = sqlQuery.value(index);
            QString edit = (show || 0 == index ? QString() : QString("contenteditable='true'"));
            switch (value.type()) {
            case QVariant::Type::Int:
                tr += QString("<td %1>%2</td>").arg(edit).arg(value.toInt());
                break;
            case QVariant::Type::Double:
                tr += QString("<td %1>%2</td>").arg(edit).arg(value.toDouble());
                break;
            case QVariant::Type::Bool:
                tr += QString("<td %1>%2</td>").arg(edit).arg(value.toBool());
                break;
            case QVariant::Type::DateTime:
                tr += QString("<td>%1</td>").arg(value.toDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz"));
                break;
            default:
                tr += QString("<td %1>%2</td>").arg(edit, value.toString());
                break;
            }
        }
        QString btn("<td class='cell' width='100px'>");
        switch (UserPermissions(permission)) {
        case UserPermissions::SuperAdministrator:
            if (!show) btn += QString("<button id='delete' class='cell-btn'>删除</button>");
        case UserPermissions::SecondaryAdministrator:
            if (!show) btn += QString(" <button id='update' class='cell-btn'>修改</button>");
        case UserPermissions::ReportSelectAndModify:
        case UserPermissions::ReportSelect:
            if (show) btn += QString(" <button id='show' class='cell-btn'>查看</button>");
        }
        dataList += (tr + btn + QString("</td></tr>"));
    }
    return dataList;
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
