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
    void reportUpload(const QString &jsonStr);
    void dataUpload(QByteArray data);
    void picUpload(const QString &filePath);
    // return image url path
    QString picUpload(const QPixmap &pixmap, const QString &fileName);
private slots:
    void requestFinished(QNetworkReply *reply);
signals:
    void finished();
private:
    QNetworkAccessManager *m_pManager;
    QNetworkRequest m_dataRequest;
    QNetworkRequest m_picRequest;
};

#endif // HTTPPOST_H
