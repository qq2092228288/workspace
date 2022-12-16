#ifndef HTTPPOST_H
#define HTTPPOST_H

#include <QObject>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QHttpMultiPart>
#include <QFileInfo>
#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>
#include <QPixmap>
#include <QBuffer>
#include <QTimer>
#include <QEventLoop>
#include <QHostInfo>
#include <QUuid>
#include <QRandomGenerator>
#include <QCryptographicHash>
#include <QScopedPointer>
#include "customctrl.h"

class JsonObjectData
{
public:
    JsonObjectData() {}
    JsonObjectData(const QJsonObject &object);
    virtual void print() const;
    QJsonObject object();
protected:
    QString find(const QString &key);
private:
    QJsonObject m_object;
};

class DeviceInfo : public JsonObjectData
{
public:
    DeviceInfo() {}
    DeviceInfo(const QJsonObject &object);
    QString id;             // 设备ID
    QString name;           // 设备名称
    QString agentName;      // 代理商名称
    QString hospitalName;   // 医院名称
    QString secret;         // 登入密码（通过MD5加密）
    QString place1Id;       // 一级场所ID
    QString place1Name;     // 一级场所名称
    QString place2Id;       // 二级场所ID
    QString place2Name;     // 二级场所名称
    QString signSecret;     // 签名秘钥
};

struct ConsumableUsedData
{
    void append(const QString &consumableId, const int &usedCount, const int &usedTag);
    QString toJsonStr() const;
    QJsonArray m_array;
};

class DataList : public JsonObjectData
{
public:
    DataList() {}
    DataList(const QJsonObject &object);
    void print() const;
    // HTTP获取
    QString createTime;         // 创建时间
    QString typeName;           // 类型名称
    int isReceived = 0;         // 是否被接收：1未接收，2已接收
    QString id;                 // 耗材id
    QString consumableTypeId;   // 耗材类型id
    QString descript;           // 耗材描述
    int totalCount = 0;         // 总数量
    int usedCount = 0;          // 可用数量
    QString deviceId;           // 设备id
    QString deviceName;         // 设备名称
};

QString ArgsNameToHttp(const QString &argsName);

class PostHttpMultiPart : public QHttpMultiPart
{
    Q_OBJECT
public:
    explicit PostHttpMultiPart(QHttpMultiPart::ContentType contentType, QObject *parent = nullptr);
    explicit PostHttpMultiPart(QObject *parent = nullptr);
    virtual ~PostHttpMultiPart();
    void appendTxt(const QString &key, const QString &value);
    void appendPic(const QString &filePath);
    void appendPic(const QPixmap &pixmap, const QString &fileName);
    void appendJson(QByteArray jsonData);
};

const int RequestGetConsumableList = -886677;
class HttpPost : public QObject
{
    Q_OBJECT
public:
    explicit HttpPost(QObject *parent = nullptr);

    virtual ~HttpPost();

    static QPixmap jsonToPixmap(const QJsonValue &value);
public slots:
    // 设置设备参数
    void setDeviceInfo(const DeviceInfo &dInfo);
    // 激活设备
    void activeDevice(const QString &mac);
    // 设备在线通知
    bool deviceOnlineNotice(const QString &deviceId);
    // 接收耗材
    void receiveConsumable(const DataList &dataList);
    // 使用耗材
    void useConsumable(const QString &deviceId, const QString &consumableUsedData);
    // 获取耗材
    void getConsumableList(const QString &pageNum, const QString &pageSize, const QString &deviceId,
                           const QString &id, const QString &consumableTypeId);
    // 报告上传
    void reportUpload(const qint64 &dtime, const QString &jsonStr);
    // 创建设备
    void createDevice(const QString &mac, const QString &deviceName);
public:
    // return image url path
    QString picUpload(const QPixmap &pixmap, const QString &fileName);
private:
    QNetworkRequest getRequest(const QJsonObject &target, const QString &url, bool authorization = true) const;
    QByteArray toPostData(const QJsonObject &target);
    QJsonObject returnValueProcessing(QNetworkAccessManager *manager, QNetworkReply *reply);
    void addJsonObject(QJsonObject &target, const QJsonObject &jsonObject);
    void addJsonArray(QJsonObject &target, const QJsonArray &jsonArray, const QString &fpDzUrl, const QString &spDzUrl);
    void addDeviceString(QJsonObject &target, const Type &type, QString fValue, QString sValue);
    void addDeviceString(QJsonObject &target, const Type &type, qreal fValue, qreal sValue, int digit);
    void addDeviceString(QJsonObject &target, const char &index, const QJsonObject &fObject, const QJsonObject &sObject);
    void addBpDeviceString(QJsonObject &target, const Type &type, qreal fValue, qreal sValue, int digit);
    int getData(const QJsonObject &data, const Type &type);
signals:
    // 激活设备获取的信息
    void deviceInfo(const DeviceInfo &deviceInfo);
    // 接收的耗材数据
    void quantitReceived(const DataList &dataList, const int &noUploadCount = RequestGetConsumableList);
    // 获取到新的批次
    void getNewBatch(const int &increase);
    // 使用耗材
    void used();
    // 获取到耗材
    void acquired(const QString &id, const QString &deviceId, const int &totalCount);
    // 重复接收
    void repeatedReceived();
    // 报告上传完成
    void finished(const qint64 &time);
    // 设备已创建
    void deviceCreated(const QString &tip);
private:
    const QString m_urlApiRequestHeader;    // api接口请求头
    const QString m_urlFileServices;        // 文件服务
    const QString m_urlActiveDevice;        // 激活设备
    const QString m_urlDeviceOnlineNotice;  // 设备在线通知
    const QString m_urlReceiveConsumable;   // 接收耗材
    const QString m_urlUseConsumable;       // 使用耗材
    const QString m_urlGetConsumableList;   // 获取耗材
    const QString m_urlSendPationReport;    // 发送患者报告
    const QString m_urlUploadFile;          // 上传文件
    const QString m_urlCreateDevice;        // 创建设备
    // 设备信息
    DeviceInfo m_deviceInfo;
private:
    int sex;                    // 性别: 0男,1女
    int age;                    // 年龄
    int height;                 // 身高
    int weight;                 // 体重
    int fposture;
    int fCvp;
    int fLap;
    int sposture;
    int sCvp;
    int sLap;
};

#endif // HTTPPOST_H
