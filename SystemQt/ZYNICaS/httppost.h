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
public:
    static QPixmap jsonToPixmap(const QJsonValue &value);
public slots:
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
    void finished(const qint64 &time);
private:
    QMutex mutex;
    QNetworkAccessManager *m_pManager;
    QNetworkRequest m_dataRequest;
    QNetworkRequest m_picRequest;
    QUrlQuery m_urlQuery;
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
