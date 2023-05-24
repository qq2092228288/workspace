#include "htmlclient.h"
#include "singleton.h"



HtmlClient::HtmlClient(WebSocketTransport *client, QObject *parent)
    : QObject{parent},
      channel_ptr{new QWebChannel}
{
    channel_ptr->connectTo(client);
    channel_ptr->registerObject(QStringLiteral("core"), this);
}
void HtmlClient::loginUi()
{
    emit sendText((xmlToJsonString(":/xml/loginUi.xml")));
}

void HtmlClient::login(const QString &adminId, const QString &password)
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

void HtmlClient::currentTable(const QJsonObject &object, const QString &tableName)
{
    mainUi(object, tableName);
}

void HtmlClient::execSqlStatement(const QJsonObject &object, const QString &tableName, const QString &statement)
{
    if (userInfoCheck(object)) {
        QString sql = statement.trimmed();
        auto permission = UserPermissions(object.value(Singleton::enumValueToKey(AdministratorInfo::permission)).toInt(-1));
        switch (permission) {
        case UserPermissions::SuperAdministrator:
            break;
        case UserPermissions::SecondaryAdministrator:
            if (0 == sql.indexOf("delete ", 0, Qt::CaseInsensitive)) {
                emit sendText("用户权限不足");
                return;
            }
            break;
        case UserPermissions::ReportSelectAndModify:
            if (0 != tableName.compare(Singleton::enumName<ReportInfo>(), Qt::CaseInsensitive) ||
                    0 == sql.indexOf("delete ", 0, Qt::CaseInsensitive) ||
                    0 == sql.indexOf("insert ", 0, Qt::CaseInsensitive)) {
                emit sendText("用户权限不足");
                return;
            }
            break;
        case UserPermissions::ReportSelect:
            if (0 != tableName.compare(Singleton::enumName<ReportInfo>(), Qt::CaseInsensitive) ||
                    0 != sql.indexOf("select ", 0, Qt::CaseInsensitive)) {
                emit sendText("用户权限不足");
                return;
            }
            break;
        default:
            emit sendText("用户权限错误");
            return;
            break;
        }
        QSqlQuery sqlQuery(Singleton::getInstance()->database());
        sqlQuery.exec(sql);
        QString lasterror = sqlQuery.lastError().text();
        if (!lasterror.isEmpty()) {
            emit sendText(lasterror);
        }
        else {
            if (0 == sql.indexOf("select ", 0, Qt::CaseInsensitive) && sqlQuery.next()) {
                emit sendReport(Singleton::getJsonObject(sqlQuery, columnNames(sqlQuery.record())));
            }
            else {
               mainUi(object, tableName);
            }
        }
    }
}

bool HtmlClient::userInfoCheck(const QJsonObject &userInfo)
{
    QStringList w;
    for (auto it = userInfo.begin(); it != userInfo.end(); ++it) {
        w<<(it.key() + "='" + (QJsonValue::Double == it.value().type() ?
            QString::number(it.value().toDouble()) : it.value().toString()) + "'");
    }
    QSqlQuery sqlQuery(Singleton::getInstance()->database());
    sqlQuery.exec(QString("SELECT * FROM %1 WHERE %2").arg(Singleton::enumName<AdministratorInfo>(), w.join(" AND ")));
    bool result = sqlQuery.next();
    if (!result) emit sendText("用户信息已修改, 请重新登录!");
    return result;
}

void HtmlClient::mainUi(const QJsonObject &object, const QString &tableName)
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
#if ENABLE_COMBINE_DEVICE
                  <<Singleton::enumName<CombinedDevice>()
#endif
                  <<Singleton::enumName<Device>()
#if ENABLE_COMBINE_DEVICE
                  <<Singleton::enumName<Computer>()
#endif
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
#if ENABLE_COMBINE_DEVICE
            QString uniqueIds = object.value(Singleton::enumValueToKey(AdministratorInfo::uniqueIds)
                                             .toLower()).toString().replace("{","'").replace("}","'");
            sqlQuery.exec(QString("SELECT %1 FROM %2 WHERE %3 IN(%4) ORDER BY %5 desc")
                          .arg(columns.join(","),
                               tableName,
                               Singleton::enumValueToKey(ReportInfo::uniqueId),
                               uniqueIds,
                               Singleton::enumValueToKey(ReportInfo::reportTime)));
#else
            QString uniqueIds = object.value(Singleton::enumValueToKey(AdministratorInfo::deviceIds)
                                             .toLower()).toString().replace("{","'").replace("}","'");
            sqlQuery.exec(QString("SELECT %1 FROM %2 WHERE %3 IN(%4) ORDER BY %5 desc")
                          .arg(columns.join(","),
                               tableName,
                               Singleton::enumValueToKey(ReportInfo::deviceId),
                               uniqueIds,
                               Singleton::enumValueToKey(ReportInfo::reportTime)));
#endif
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
    foreach (auto str, tableNames) {
        mhtml<<QString("<div class='action'><div class='btn' id='%1' onclick='tablesBtnClick(this.id)'>%3</div></div>")
               .arg(str, EnumTextCN::cn_EnumName(str));
    }
    mhtml<<QString("</div>");

    // right data
    mhtml<<QString("<div class='data-wrapper'>");
    // table
    mhtml<<QString("<table class='table' border='2'>");
    // caption
    mhtml<<QString("<caption id='%1' class='cap'>%2</caption>").arg(tableName, EnumTextCN::cn_EnumName(tableName));
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
                mhtml<<QString("<td align='center'>default</td>");
            }
            else {
                mhtml<<QString("<td contenteditable='true'></td>");
            }
        }
        mhtml<<QString("<td class='cell' width='100px'><button id='append' class='cell-btn'>新增</button></td>");
        mhtml<<QString("</tr>");
    }
    while (sqlQuery.next()) {
        mhtml<<QString("<tr>");
        for (int i = 0; i < record.count(); ++i) {
            auto value = sqlQuery.value(i);
            QString property = (uneditable || 0 == i || EnumTextCN::compareEname<AllocatedConsumables>(tableName)) ?
                        QString() : QString(" contenteditable='true'");
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
                if (!uneditable && i == 1 && record.fieldName(i)
                        .compare(Singleton::enumValueToKey(AdministratorInfo::password), Qt::CaseInsensitive) == 0)
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
            if (uneditable)
                mhtml<<QString(" <button id='show' class='cell-btn'>查看</button>");
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

QStringList HtmlClient::softManagementUi(const UserPermissions &permission)
{
    QString tableName = Singleton::enumName<SoftwareManagement>();
    QStringList mhtml;
    // right data
    mhtml<<QString("<div class='data-wrapper'>");
    // table
    mhtml<<QString("<table class='table' border='2'>");
    // caption
    mhtml<<QString("<caption id='%1' class='cap'>%2</caption>").arg(tableName, EnumTextCN::cn_EnumName(tableName));
    // thead
    mhtml<<QString("<thead>");
    mhtml<<QString("<tr>");
    // headers
    QSqlQuery sqlQuery(Singleton::getInstance()->database());
    sqlQuery.exec(QString("SELECT * FROM %1").arg(tableName));
    auto record = sqlQuery.record();
    for (int i = 0; i < record.count(); ++i) {
        mhtml<<QString("<th id='%1'>%2</th>").arg(record.fieldName(i), EnumTextCN::cn_EnumValue(tableName, record.fieldName(i)));
    }
    mhtml<<QString("<th width='95px'>操作</th>");
    mhtml<<QString("</tr>");
    mhtml<<QString("</thead>");
    // tbody
    mhtml<<QString("<tbody onclick='tbodyClick(event)'>");
    // append
    if (UserPermissions::SuperAdministrator == permission || UserPermissions::SecondaryAdministrator == permission) {
        mhtml<<QString("<tr>");
        for (int i = 0; i < record.count(); ++i) {
            if (0 == record.fieldName(i).compare(Singleton::enumValueToKey(SoftwareManagement::createTime), Qt::CaseInsensitive)) {
                mhtml<<QString("<td align='center'>default</td>");
            }
            else {
                mhtml<<QString("<td contenteditable='true'></td>");
            }
        }
        mhtml<<QString("<td class='cell' width='100px'><button id='append' class='cell-btn'>新增</button></td>");
        mhtml<<QString("</tr>");
    }
    // data

    mhtml<<QString("</tbody>");
    mhtml<<QString("</table>");
    mhtml<<QString("</div>");
    return mhtml;
}

QStringList HtmlClient::columnNames(const QSqlRecord &record)
{
    QStringList names;
    for (int var = 0; var < record.count(); ++var) {
        names<<record.fieldName(var);
    }
    return names;
}

QString HtmlClient::xmlToJsonString(const QString &xmlPath)
{
    QFile file(xmlPath);
    if (!file.open(QFile::ReadOnly)) {
        return QString();
    }
    QString data = QString(file.readAll());
    file.close();
    return htmlJsonString(data);
}

QString HtmlClient::htmlJsonString(const QString &data)
{
    QJsonObject object;
    object.insert("html", data.simplified());
    return Singleton::jsonToString(object);
}
