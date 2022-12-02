#ifndef DEVICEDATABASE_H
#define DEVICEDATABASE_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QDebug>
#include <QUuid>
#include <QCryptographicHash>
#include <QNetworkInterface>
#include <QMap>

struct DeviceInfo;
class DeviceDatabase : public QObject
{
    Q_OBJECT
public:
    explicit DeviceDatabase(QObject *parent = nullptr);
public slots:
    // 更新设备信息
    void updateDeviceInfo(const DeviceInfo &deviceInfo);
    // 获取设备信息
    QString getDeviceInfo(const QString &infoName);
    // 增加耗材
    void addConsumable(const QString &id, const int &count);
    // 材料剩余
    int getConsumableSurplus();
    // 使用耗材
    void usedConsumable();
    // 获取离线未上传耗材数量
    int getNoUploadCount();
    // 尝试上传使用耗材数量
    void tryToUpload(int used);

    void updatebatchTable(const QString &consumableId, const int &usedCount);

private:
    // 值变化量
    void updateListTable(const int &cTotalCount, const int &cUsedCount, const int &cNoUploadCount);
    // calculation md5
    QString getMd5Str(const QString &mac, const int &totalCount, const int &usedCount,
                      const int &noUploadCount, const QString &uuid);
    // 数据完整性检测，若合法，则返回剩余量
    int databaseIntegralityCheck();
    // 校验MAC
    bool macIntegralityCheck(const QString &mac, const QString &tableName);
    // 获取MAC地址
    QString getMac() const;
signals:
//    void currentCount(const int &totalCount, const int &usedCount);
    void useConsumable(const QString &deviceId, const QString &consumableUsedData,
                       const QString &consumableId, const int &usedCount);
    void error();
private:
    const QString m_databaseName;
    const QString m_dataTable;
    const QString m_listTable;
    const QString m_batchTable;
    QSqlDatabase m_database;
};

#endif // DEVICEDATABASE_H
