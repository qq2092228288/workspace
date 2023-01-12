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

class DeviceInfo;
class DataList;
class DeviceDatabase : public QObject
{
    Q_OBJECT
public:
    explicit DeviceDatabase(QObject *parent = nullptr);
    // 数据完整性检测
    bool databaseIntegralityCheck();
    // 当前设备的所有信息
    DeviceInfo deviceInfo();
public slots:
    // 更新设备信息
    void updateDeviceInfo(const DeviceInfo &deviceInfo);
    // 获取设备信息
    QString getDeviceInfo(const QString &infoName);
    // 更新耗材列表
    void updateConsumableList(const DataList &dataList, int noUploadCount);
    // 材料剩余
    int getConsumableSurplus();
    // 离线使用一个耗材
    void offlineUsed();
    // 获取离线未上传耗材数量
    int getNoUploadCount();
    // 尝试上传使用耗材数量
    void tryToUpload();
    // 已上传
    void uploaded();
    // 清空数据
    void clearTables();
private:
    // calculation md5
    QString md5Str(const DataList &dataList, const int &noUploadCount, const QString &uuid);

    DataList getDataList(const QString &consumableId);
    // 批次未上传
    int batchNoUploadCount(const QString &consumableId);
    // 获取MAC地址
    QString getMac() const;
signals:
    void requestUseConsumable(const QString &deviceId, const QString &consumableUsedData);
private:
    // 数据库名
    const QString m_databaseName;
    // 设备数据表
    const QString m_dataTable;
    // 耗材数据表
    const QString m_batchTable;
    // 数据库类
    QSqlDatabase m_database;
};

#endif // DEVICEDATABASE_H
