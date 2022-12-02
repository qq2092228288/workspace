#include "devicedatabase.h"
#include "httppost.h"

DeviceDatabase::DeviceDatabase(QObject *parent)
    : QObject{parent},
      m_databaseName{"Device.db"},
      m_dataTable{"data"},
      m_listTable{"list"},
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
        QString dataTable = QString("CREATE TABLE %1(mac TEXT PRIMARY KEY, "
                                    "deviceId TEXT, "
                                    "name TEXT, "
                                    "agentName TEXT, "
                                    "hospitalName TEXT, "
                                    "secret TEXT, "
                                    "place1Id TEXT, "
                                    "place1Name TEXT, "
                                    "place2Id TEXT, "
                                    "place2Name TEXT)")
                            .arg(m_dataTable);
        sqlQuery.prepare(dataTable);
        sqlQuery.exec();

        QString listTable = QString("CREATE TABLE %1(mac TEXT PRIMARY KEY, "
                                    "totalCount INT NOT NULL, "
                                    "usedCount INT NOT NULL, "
                                    "noUploadCount INT NOT NULL, "
                                    "uuid TEXT NOT NULL, "
                                    "md5 TEXT NOT NULL)")
                            .arg(m_listTable);
        sqlQuery.prepare(listTable);
        sqlQuery.exec();

        QString batchTable = QString("CREATE TABLE %1(consumableId TEXT PRIMARY KEY, "
                                    "totalCount INT NOT NULL, "
                                    "usedCount INT NOT NULL)")
                            .arg(m_batchTable);
        sqlQuery.prepare(batchTable);
        sqlQuery.exec();
    }
    else {
        qWarning("数据库打开失败！");
    }

    qDebug()<<__LINE__<<__FUNCTION__;
    QSqlQuery sqlQuery(m_database);
    sqlQuery.exec(QString("SELECT * FROM %1").arg(m_listTable));
    while (sqlQuery.next()) {
        qDebug()<<"mac"<<sqlQuery.value(0).toString();
        qDebug()<<"totalCount"<<sqlQuery.value(1).toInt();
        qDebug()<<"usedCount"<<sqlQuery.value(2).toInt();
        qDebug()<<"noUploadCount"<<sqlQuery.value(3).toInt();
        qDebug()<<"uuid"<<sqlQuery.value(4).toString();
        qDebug()<<"md5"<<sqlQuery.value(5).toString();
    }
    sqlQuery.exec(QString("SELECT * FROM %1").arg(m_batchTable));
    while (sqlQuery.next()) {
        qDebug()<<"consumableId"<<sqlQuery.value(0).toString();
        qDebug()<<"totalCount"<<sqlQuery.value(1).toInt();
        qDebug()<<"usedCount"<<sqlQuery.value(2).toInt();
    }
}

void DeviceDatabase::updateDeviceInfo(const DeviceInfo &deviceInfo)
{
    if (!macIntegralityCheck(getMac(), m_dataTable)) {
        return;
    }
    QSqlQuery sqlQuery(m_database);
    sqlQuery.prepare(QString("REPLACE INTO %1(mac, deviceId, name, agentName, hospitalName, "
                             "secret, place1Id, place1Name, place2Id, place2Name) "
                             "VALUES(:mac, :deviceId, :name, :agentName, :hospitalName, "
                             ":secret, :place1Id, :place1Name, :place2Id, :place2Name)")
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

void DeviceDatabase::addConsumable(const QString &id, const int &count)
{
    updateListTable(count, 0, 0);
    QSqlQuery sqlQuery(m_database);
    sqlQuery.prepare(QString("INSERT INTO %1(consumableId, totalCount, usedCount) "
                             "VALUES(:consumableId, :totalCount, :usedCount)").arg(m_batchTable));
    sqlQuery.bindValue(":consumableId", id);
    sqlQuery.bindValue(":totalCount", count);
    sqlQuery.bindValue(":usedCount", 0);
    sqlQuery.exec();
}

int DeviceDatabase::getConsumableSurplus()
{
    return databaseIntegralityCheck();
}

void DeviceDatabase::usedConsumable()
{
    updateListTable(0, 1, 1);
}

int DeviceDatabase::getNoUploadCount()
{
    if (-1 != databaseIntegralityCheck()) {
        QSqlQuery sqlQuery(m_database);
        sqlQuery.exec(QString("SELECT * FROM %1").arg(m_listTable));
        if (sqlQuery.next()) {
            return sqlQuery.value(3).toInt();
        }
    }
    return 0;
}

void DeviceDatabase::tryToUpload(int used)
{
    QSqlQuery sqlQuery(m_database);
    // 查找
    sqlQuery.exec(QString("SELECT * FROM %1").arg(m_batchTable));
    while (sqlQuery.next()) {
        if (used > 0) {
            ConsumableUsedData consumableUsedData;
            const QString consumableId = sqlQuery.value(0).toString();
            const int totalCount = sqlQuery.value(1).toInt();
            const int usedCount = sqlQuery.value(2).toInt();
            int temp = usedCount;
            while (totalCount > usedCount) {
                consumableUsedData.append(consumableId, 1, ++temp);
                if (--used == 0) {
                    break;
                }
            }
            if (!consumableUsedData.m_array.isEmpty()) {
                emit useConsumable(getDeviceInfo("deviceId"), consumableUsedData.toJsonStr(),
                                   consumableId, temp - usedCount);
            }
        }
        else {
            break;
        }
    }
}

void DeviceDatabase::updatebatchTable(const QString &consumableId, const int &usedCount)
{
    QSqlQuery sqlQuery(m_database);
    sqlQuery.exec(QString("SELECT * FROM %1 WHERE consumableId='%2'").arg(m_batchTable, consumableId));
    if (sqlQuery.next()) {
        int oldUsedCount = sqlQuery.value(2).toInt();
        sqlQuery.exec(QString("UPDATE %1 SET usedCount=%2 WHERE consumableId='%3'")
                      .arg(m_batchTable).arg(oldUsedCount + usedCount).arg(consumableId));
        updateListTable(0, 0, -usedCount);
    }
}

void DeviceDatabase::updateListTable(const int &cTotalCount, const int &cUsedCount, const int &cNoUploadCount)
{
    if (-1 == databaseIntegralityCheck()) {
        return;
    }
    QSqlQuery sqlQuery(m_database);
    sqlQuery.exec(QString("SELECT * FROM %1").arg(m_listTable));
    int totalCount = cTotalCount;
    int usedCount = cUsedCount;
    int noUploadCount = cNoUploadCount;
    if (sqlQuery.next()) {
        totalCount += sqlQuery.value(1).toInt();
        usedCount += sqlQuery.value(2).toInt();
        noUploadCount += sqlQuery.value(3).toInt();
    }
    const QString mac = getMac();
    const QString uuid = QUuid::createUuid().toString();
    qDebug()<<__LINE__<<__FUNCTION__<<"uuid:"<<uuid;
    const QString md5 = getMd5Str(mac, totalCount, usedCount, noUploadCount, uuid);
    sqlQuery.prepare(QString("REPLACE INTO %1(mac, totalCount, usedCount, noUploadCount, uuid, md5) "
                             "VALUES(:mac, :totalCount, :usedCount, :noUploadCount, :uuid, :md5)")
                     .arg(m_listTable));
    sqlQuery.bindValue(":mac", mac);
    sqlQuery.bindValue(":totalCount", totalCount);
    sqlQuery.bindValue(":usedCount", usedCount);
    sqlQuery.bindValue(":noUploadCount", noUploadCount);
    sqlQuery.bindValue(":uuid", uuid);
    sqlQuery.bindValue(":md5", md5);
    sqlQuery.exec();
}

QString DeviceDatabase::getMd5Str(const QString &mac, const int &totalCount, const int &usedCount,
                                  const int &noUploadCount, const QString &uuid)
{
    QString str = QString("mac=%1;totalCount=%2;usedCount=%3;noUploadCount=%4;uuid=%5;")
            .arg(mac).arg(totalCount).arg(usedCount).arg(noUploadCount).arg(uuid);
    QByteArray md5 = QCryptographicHash::hash(str.toLatin1(),QCryptographicHash::Md5);
    return QString(md5.toHex());
}

int DeviceDatabase::databaseIntegralityCheck()
{
    QSqlQuery sqlQuery(m_database);
    // 查找
    sqlQuery.exec(QString("SELECT * FROM %1").arg(m_listTable));
    if (sqlQuery.next()) {
        const QString mac = sqlQuery.value(0).toString();
        int totalCount = sqlQuery.value(1).toInt();
        int usedCount = sqlQuery.value(2).toInt();
        int noUploadCount = sqlQuery.value(3).toInt();
        const QString uuid = sqlQuery.value(4).toString();
        const QString md5 = sqlQuery.value(5).toString();
        if (mac == getMac() &&
            md5 == getMd5Str(mac, totalCount, usedCount, noUploadCount, uuid)) {
            return (totalCount - usedCount);
        }
    }
    else {  // 表格无数据
        return 0;
    }
    // mac 地址改变，或数据库被非法修改
    emit error();
    return -1;
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
