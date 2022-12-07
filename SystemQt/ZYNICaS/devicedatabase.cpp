#include "devicedatabase.h"
#include "httppost.h"

DeviceDatabase::DeviceDatabase(QObject *parent)
    : QObject{parent},
      m_databaseName{"Device.db"},
      m_dataTable{"data"},
      m_batchTable{"batch"}
{
    // 建立和SQlite数据库的连接
    m_database = QSqlDatabase::addDatabase("QSQLITE", m_databaseName);
    // 设置数据库文件的名字
    m_database.setDatabaseName(m_databaseName);
    if (m_database.open()) {
        // 用于执行sql语句的对象
        QSqlQuery sqlQuery(m_database);
        // 建表
        QString dataTable = QString("CREATE TABLE %1("
                                    "mac TEXT PRIMARY KEY, "
                                    "deviceId TEXT NOT NULL, "
                                    "name TEXT, "
                                    "agentName TEXT, "
                                    "hospitalName TEXT, "
                                    "secret TEXT, "
                                    "place1Id TEXT NOT NULL, "
                                    "place1Name TEXT, "
                                    "place2Id TEXT, "
                                    "place2Name TEXT)")
                            .arg(m_dataTable);
        sqlQuery.prepare(dataTable);
        sqlQuery.exec();

        QString batchTable = QString("CREATE TABLE %1("
                                     "createTime TEXT NOT NULL, "
                                     "typeName TEXT, "
                                     "isReceived INT NOT NULL, "
                                     "id TEXT PRIMARY KEY, "
                                     "consumableTypeId TEXT, "
                                     "descript TEXT, "
                                     "totalCount INT NOT NULL, "
                                     "usedCount INT NOT NULL, "
                                     "deviceId TEXT NOT NULL, "
                                     "deviceName TEXT, "
                                     "noUploadCount INT NOT NULL, "
                                     "uuid TEXT NOT NULL, "
                                     "md5 TEXT NOT NULL)")
                            .arg(m_batchTable);
        sqlQuery.prepare(batchTable);
        sqlQuery.exec();
    }
    else {
        qWarning("数据库打开失败！");
    }
}

void DeviceDatabase::updateDeviceInfo(const DeviceInfo &deviceInfo)
{
    if (!macIntegralityCheck(getMac(), m_dataTable)) {
        return;
    }
    QSqlQuery sqlQuery(m_database);
    sqlQuery.prepare(QString("REPLACE INTO %1("
                             "mac, "
                             "deviceId, "
                             "name, "
                             "agentName, "
                             "hospitalName, "
                             "secret, "
                             "place1Id, "
                             "place1Name, "
                             "place2Id, "
                             "place2Name) "
                             "VALUES("
                             ":mac, "
                             ":deviceId, "
                             ":name, "
                             ":agentName, "
                             ":hospitalName, "
                             ":secret, "
                             ":place1Id, "
                             ":place1Name, "
                             ":place2Id, "
                             ":place2Name)")
                     .arg(m_dataTable));
    sqlQuery.bindValue(":mac", getMac());
    sqlQuery.bindValue(":deviceId", deviceInfo.id);
    sqlQuery.bindValue(":name", deviceInfo.name);
    sqlQuery.bindValue(":agentName", deviceInfo.agentName);
    sqlQuery.bindValue(":hospitalName", deviceInfo.hospitalName);
    sqlQuery.bindValue(":secret", deviceInfo.secret);
    sqlQuery.bindValue(":place1Id", deviceInfo.place1Id);
    sqlQuery.bindValue(":place1Name", deviceInfo.place1Name);
    sqlQuery.bindValue(":place2Id", deviceInfo.place2Id);
    sqlQuery.bindValue(":place2Name", deviceInfo.place2Name);
    sqlQuery.exec();
}

QString DeviceDatabase::getDeviceInfo(const QString &infoName)
{
    QSqlQuery sqlQuery(m_database);
    sqlQuery.exec(QString("SELECT %1 FROM %2").arg(infoName, m_dataTable));
    if (sqlQuery.next()) {
        return sqlQuery.value(0).toString();
    }
    return QString();
}

void DeviceDatabase::updateConsumableList(const DataList &dataList, int noUploadCount)
{
    // 如果是http请求获取耗材列表
    if (RequestGetConsumableList == noUploadCount) {
        noUploadCount = batchNoUploadCount(dataList.id);
    }
    QSqlQuery sqlQuery(m_database);
    const QString uuid = QUuid::createUuid().toString();
    const QString md5 = md5Str(dataList, noUploadCount, uuid);

    sqlQuery.prepare(QString("REPLACE INTO %1("
                             "createTime, "
                             "typeName, "
                             "isReceived, "
                             "id, "
                             "consumableTypeId, "
                             "descript, "
                             "totalCount, "
                             "usedCount, "
                             "deviceId, "
                             "deviceName, "
                             "noUploadCount, "
                             "uuid, "
                             "md5) "
                             "VALUES("
                             ":createTime, "
                             ":typeName, "
                             ":isReceived, "
                             ":id, "
                             ":consumableTypeId, "
                             ":descript, "
                             ":totalCount, "
                             ":usedCount, "
                             ":deviceId, "
                             ":deviceName, "
                             ":noUploadCount, "
                             ":uuid, "
                             ":md5)")
                     .arg(m_batchTable));

    sqlQuery.bindValue(":createTime", dataList.createTime);
    sqlQuery.bindValue(":typeName", dataList.typeName);
    sqlQuery.bindValue(":isReceived", dataList.isReceived);
    sqlQuery.bindValue(":id", dataList.id);
    sqlQuery.bindValue(":consumableTypeId", dataList.consumableTypeId);
    sqlQuery.bindValue(":descript", dataList.descript);
    sqlQuery.bindValue(":totalCount", dataList.totalCount);
    sqlQuery.bindValue(":usedCount", dataList.usedCount);
    sqlQuery.bindValue(":deviceId", dataList.deviceId);
    sqlQuery.bindValue(":deviceName", dataList.deviceName);
    sqlQuery.bindValue(":noUploadCount", noUploadCount);
    sqlQuery.bindValue(":uuid", uuid);
    sqlQuery.bindValue(":md5", md5);

    sqlQuery.exec();
}

int DeviceDatabase::getConsumableSurplus()
{
    QSqlQuery sqlQuery(m_database);
    sqlQuery.exec(QString("SELECT SUM(totalCount), SUM(usedCount), SUM(noUploadCount) FROM %1").arg(m_batchTable));
    if (sqlQuery.next()) {
        return (sqlQuery.value(0).toInt() - sqlQuery.value(1).toInt() - sqlQuery.value(2).toInt());
    }
    return 0;
}

void DeviceDatabase::offlineUsed()
{
    QSqlQuery sqlQuery(m_database);
    sqlQuery.exec(QString("SELECT * FROM %1").arg(m_batchTable));
    while (sqlQuery.next()) {
        int totalCount = sqlQuery.value("totalCount").toInt();
        int usedCount = sqlQuery.value("usedCount").toInt();
        int noUploadCount = sqlQuery.value("noUploadCount").toInt();
        if (totalCount > usedCount + noUploadCount) {
            const QString consumableId = sqlQuery.value("id").toString();
            DataList dataList = getDataList(consumableId);
            updateConsumableList(dataList, batchNoUploadCount(consumableId) + 1);
            tryToUpload();
            break;
        }
    }
}

int DeviceDatabase::getNoUploadCount()
{
    QSqlQuery sqlQuery(m_database);
    sqlQuery.exec(QString("SELECT SUM(noUploadCount) FROM %1").arg(m_batchTable));
    if (sqlQuery.next()) {
        return sqlQuery.value(0).toInt();
    }
    return 0;
}

void DeviceDatabase::tryToUpload()
{
    if (getNoUploadCount() == 0) {
        return;
    }
    ConsumableUsedData consumableUsedData;

    QSqlQuery sqlQuery(m_database);
    // 查找
    sqlQuery.exec(QString("SELECT * FROM %1").arg(m_batchTable));
    while (sqlQuery.next()) {
        const QString consumableId = sqlQuery.value("id").toString();
        int usedCount = sqlQuery.value("usedCount").toInt();
        int noUploadCount = sqlQuery.value("noUploadCount").toInt();
        while (noUploadCount > 0) {
            consumableUsedData.append(consumableId, 1, ++usedCount);
            --noUploadCount;
        }
    }
    if (!consumableUsedData.m_array.isEmpty()) {
        emit requestUseConsumable(getDeviceInfo("deviceId"), consumableUsedData.toJsonStr());
    }
}

void DeviceDatabase::uploaded()
{
    QStringList consumableIdList;
    QSqlQuery sqlQuery(m_database);
    // 查找
    sqlQuery.exec(QString("SELECT * FROM %1").arg(m_batchTable));
    while (sqlQuery.next()) {
        int noUploadCount = sqlQuery.value("noUploadCount").toInt();
        if (noUploadCount != 0) {
            consumableIdList<<sqlQuery.value("id").toString();
        }
    }
    foreach (const QString &consumableId, consumableIdList) {
        DataList dataList = getDataList(consumableId);
        dataList.usedCount += batchNoUploadCount(consumableId);
        updateConsumableList(dataList, 0);
    }
}

QString DeviceDatabase::md5Str(const DataList &dataList, const int &noUploadCount, const QString &uuid)
{
    QString str = QString("createTime=%1;typeName=%2;isReceived=%3;id=%4;consumableTypeId=%5;"
                          "descript=%6;totalCount=%7;usedCount=%8;deviceId=%9;deviceName=%10;"
                          "noUploadCount=%11;uuid=%12;")
            .arg(dataList.createTime, dataList.typeName)
            .arg(dataList.isReceived)
            .arg(dataList.id, dataList.consumableTypeId, dataList.descript)
            .arg(dataList.totalCount)
            .arg(dataList.usedCount)
            .arg(dataList.deviceId, dataList.deviceName)
            .arg(noUploadCount)
            .arg(uuid);
    QByteArray md5 = QCryptographicHash::hash(str.toLatin1(),QCryptographicHash::Md5);
    return QString(md5.toHex());
}

DataList DeviceDatabase::getDataList(const QString &consumableId)
{
    DataList dataList;
    QSqlQuery sqlQuery(m_database);
    sqlQuery.exec(QString("SELECT * FROM %1 WHERE id='%2'").arg(m_batchTable, consumableId));
    if (sqlQuery.next()) {
        dataList.createTime = sqlQuery.value("createTime").toString();
        dataList.typeName = sqlQuery.value("typeName").toString();
        dataList.isReceived = sqlQuery.value("isReceived").toInt();
        dataList.id = sqlQuery.value("id").toString();
        dataList.consumableTypeId = sqlQuery.value("consumableTypeId").toString();
        dataList.descript = sqlQuery.value("descript").toString();
        dataList.totalCount = sqlQuery.value("totalCount").toInt();
        dataList.usedCount = sqlQuery.value("usedCount").toInt();
        dataList.deviceId = sqlQuery.value("deviceId").toString();
        dataList.deviceName = sqlQuery.value("deviceName").toString();
    }
    return dataList;
}

int DeviceDatabase::batchNoUploadCount(const QString &consumableId)
{
    QSqlQuery sqlQuery(m_database);
    sqlQuery.exec(QString("SELECT noUploadCount FROM %1 WHERE id='%2'").arg(m_batchTable, consumableId));
    if (sqlQuery.next()) {
        return sqlQuery.value(0).toInt();
    }
    return 0;
}

int DeviceDatabase::databaseIntegralityCheck()
{
    QSqlQuery sqlQuery(m_database);
    // 查找
    sqlQuery.exec(QString("SELECT * FROM %1").arg(m_batchTable));
    while (sqlQuery.next()) {
        const QString consumableId = sqlQuery.value("id").toString();
        DataList dataList = getDataList(consumableId);
        int noUploadCount = sqlQuery.value("noUploadCount").toInt();
        const QString uuid = sqlQuery.value("uuid").toString();
        const QString md5 = sqlQuery.value("md5").toString();
        if (md5 != md5Str(dataList, noUploadCount, uuid)) { // 数据库被非法修改
            emit error();
            return -1;
        }
    }
    return getConsumableSurplus();
}

bool DeviceDatabase::macIntegralityCheck(const QString &mac, const QString &tableName)
{
    QSqlQuery sqlQuery(m_database);
    // 查找
    sqlQuery.exec(QString("SELECT * FROM %1").arg(tableName));
    if (sqlQuery.next()) {  // 有数据
        sqlQuery.exec(QString("SELECT * FROM %1 WHERE mac='%2'").arg(tableName, mac));
        if (!sqlQuery.next()) { // mac改变
            return false;
        }
    }
    return true;    // 无数据或mac存在
}

QString DeviceDatabase::getMac() const
{
    QList<QNetworkInterface> nets = QNetworkInterface::allInterfaces();// 获取所有网络接口列表
    for(int i = 0; i < nets.count(); i ++)
    {
        // 如果此网络接口不是回环地址，则就是我们需要找的Mac地址
        if(!nets[i].flags().testFlag(QNetworkInterface::IsLoopBack)) {
            return nets[i].hardwareAddress();
            break;
        }
    }
    return QString();
}
