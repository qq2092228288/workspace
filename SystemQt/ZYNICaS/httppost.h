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
#include <QMutexLocker>
#include "customctrl.h"

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

class HttpPost : public QObject
{
    Q_OBJECT
public:
    explicit HttpPost(QObject *parent = nullptr);
    static QPixmap jsonToPixmap(const QJsonValue &value);

public slots:
    // 获取有效验证码
//    void getValidCode();
    // 设备在线通知
    void deviceOnlineNotice(const QString &deviceId);
    // 报告上传
    void reportUpload(const qint64 &dtime, const QString &jsonStr);
private slots:
    void picUpload(const QString &filePath);
    // return image url path
    QString picUpload(const QPixmap &pixmap, const QString &fileName);
private:
    QUrlQuery addJsonObject(const QJsonObject &jsonObject);
    QUrlQuery addJsonArray(const QJsonArray &jsonArray, const QString &fpDzUrl, const QString &spDzUrl);
    QUrlQuery addDeviceString(const Type &type, QString fValue, QString sValue);
    QUrlQuery addDeviceString(const Type &type, qreal fValue, qreal sValue, int digit);
    QUrlQuery addDeviceString(const char &index, const QJsonObject &fObject, const QJsonObject &sObject);
    QUrlQuery addBpDeviceString(const Type &type, qreal fValue, qreal sValue, int digit);
    int getData(const QJsonObject &data, const Type &type);
signals:
//    void qrCodeValue(const QString &url);
    void online();
    void finished(const qint64 &time);
private:
    QMutex mutex;
    QNetworkAccessManager *m_pManager;
    QUrlQuery m_urlQuery;
    const QString m_urlApiRequestHeader;    // api接口请求头
    const QString m_urlFileServices;        // 文件服务
    const QString m_urlActiveDevice;        // 激活设备
    const QString m_urlDeviceOnlineNotice;  // 设备在线通知
    const QString m_urlReceiveConsumable;   // 接收耗材
    const QString m_urlUseConsumable;       // 使用耗材
    const QString m_urlGetConsumableList;   // 获取耗材
    const QString m_urlSendPationReport;    // 发送患者报告
    const QString m_urlUploadFile;          // 上传文件
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
