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

private:
    // calculation md5
    QString md5Str(const DataList &dataList, const int &noUploadCount, const QString &uuid);

    DataList getDataList(const QString &consumableId);
    // 批次未上传
    int batchNoUploadCount(const QString &consumableId);
    // 数据完整性检测，若合法，则返回剩余量
    int databaseIntegralityCheck();
    // 校验MAC
    bool macIntegralityCheck(const QString &mac, const QString &tableName);
    // 获取MAC地址
    QString getMac() const;
signals:
    void requestUseConsumable(const QString &deviceId, const QString &consumableUsedData);

    void error();
private:
    const QString m_databaseName;
    const QString m_dataTable;
    const QString m_batchTable;
    QSqlDatabase m_database;
};

#endif // DEVICEDATABASE_H
