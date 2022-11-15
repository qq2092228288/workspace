#include "httppost.h"


PostHttpMultiPart::PostHttpMultiPart(ContentType contentType, QObject *parent)
    : QHttpMultiPart{contentType, parent}
{

}

PostHttpMultiPart::PostHttpMultiPart(QObject *parent)
    : QHttpMultiPart{parent}
{

}

PostHttpMultiPart::~PostHttpMultiPart()
{
    qDebug()<<"~PostHttpMultiPart()";
}

void PostHttpMultiPart::appendTxt(const QString &key, const QString &value)
{
    QHttpPart httpPart;
    httpPart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("text/plain"));
    httpPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant(QString("form-data; name=\"%1\"").arg(key)));
    httpPart.setBody(value.toUtf8());
    this->append(httpPart);
}

void PostHttpMultiPart::appendPic(const QString &filePath)
{
    if(filePath.isEmpty()) return;
    QFileInfo info(filePath);
    QString fileName=info.fileName();
    // 图片
    QHttpPart filePart;
    QString header=QString("form-data; name=\"file\";filename=\"%1\"").arg(fileName);
    filePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("image/png"));
    filePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant(header));
    QFile *file = new QFile(filePath);
    file->open(QIODevice::ReadOnly);
    filePart.setBodyDevice(file);
    file->setParent(this);
    // we cannot delete the file now, so delete it with the multiPart
    this->append(filePart);
}

void PostHttpMultiPart::appendPic(const QPixmap &pixmap, const QString &fileName)
{
    QHttpPart filePart;
    QString header=QString("form-data; name=\"file\";filename=\"%1\"").arg(fileName);
    filePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("image/png"));
    filePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant(header));
    QBuffer *buffer = new QBuffer(this);
    buffer->open(QIODevice::WriteOnly);
    pixmap.save(buffer, "PNG");
    filePart.setBodyDevice(buffer);
    buffer->setParent(this);
    this->append(filePart);
}

void PostHttpMultiPart::appendJson(QByteArray jsonData)
{
    QHttpPart httpPart;
    httpPart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json"));
    httpPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant(QString("form-data;")));
    httpPart.setBody(jsonData);
    this->append(httpPart);
}

HttpPost::HttpPost(QObject *parent)
    : QObject{parent}
{
    m_pManager = new QNetworkAccessManager(this);

    m_dataRequest.setUrl(QUrl("http://43.142.236.90/qk-wcxl-business/dDevice/sendPationReport"));
    m_dataRequest.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/x-www-form-urlencoded"));
    m_dataRequest.setRawHeader("Connection", "Keep-Alive");

    m_picRequest.setUrl(QUrl("http://43.142.236.90:22021/group1/upload"));

//    connect(m_pManager, &QNetworkAccessManager::finished, this, &HttpPost::requestFinished);
}

QPixmap HttpPost::jsonToPixmap(const QJsonValue &value) {
    auto const encoded = value.toString().toLatin1();
    QPixmap pixmap;
    pixmap.loadFromData(QByteArray::fromBase64(encoded), "PNG");
    return pixmap;
}

void HttpPost::reportUpload(const QString &jsonStr)
{
    qDebug()<<jsonStr.size();
    QString sudokuUrl, fpDzUrl, spDzUrl;
    QJsonObject object = QJsonDocument::fromJson(jsonStr.toLocal8Bit()).object();
    qDebug()<<QJsonDocument(object);
    for (auto it = object.begin(); it != object.end(); ++it) {
        if (it.key() == "pAnalyse") {
            qDebug()<<it.value();
            QPixmap pixmap = jsonToPixmap(it.value());
            sudokuUrl = picUpload(pixmap, QString("sudoku%1.png").arg(QDateTime::currentSecsSinceEpoch()));
        }
        else if (it.key() == "firstPosition") {
            QJsonObject temp = it.value().toObject();
            for (auto ie = temp.begin(); ie != temp.end(); ++ie) {
                if (ie.key() == "pDz") {
                    QPixmap pixmap = jsonToPixmap(it.value());
                    fpDzUrl = picUpload(pixmap, QString("fDz%1.png").arg(QDateTime::currentSecsSinceEpoch()));
                }
            }
        }
        else if (it.key() == "secondPosition") {
            QJsonObject temp = it.value().toObject();
            for (auto ie = temp.begin(); ie != temp.end(); ++ie) {
                if (ie.key() == "pDz") {
                    QPixmap pixmap = jsonToPixmap(it.value());
                    spDzUrl = picUpload(pixmap, QString("sDz%1.png").arg(QDateTime::currentSecsSinceEpoch()));
                }
            }
        }
    }
    qDebug()<<sudokuUrl<<fpDzUrl<<spDzUrl;
}

void HttpPost::dataUpload(QByteArray data)
{
//    QNetworkRequest request;
//    request.setUrl(QUrl("http://43.142.236.90/qk-wcxl-business/dDevice/sendPationReport"));
//    request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/x-www-form-urlencoded"));
//    request.setRawHeader("Connection", "Keep-Alive");
    m_pManager->post(m_dataRequest, data);
}

void HttpPost::picUpload(const QString &filePath)
{
    PostHttpMultiPart *multiPart = new PostHttpMultiPart(QHttpMultiPart::FormDataType);
    multiPart->appendPic(filePath);
    multiPart->appendTxt("scene", "wcReport");
    multiPart->appendTxt("output", "json2");

    QNetworkReply *reply = m_pManager->post(m_picRequest, multiPart);
    multiPart->setParent(reply);
}

QString HttpPost::picUpload(const QPixmap &pixmap, const QString &fileName)
{
    QString picUrlPath;
    // 超时处理定时器
    QTimer timer;
    timer.setInterval(10000);   // 超时时间
    timer.setSingleShot(true);  // 单次触发
    // 上传的图片信息
    PostHttpMultiPart *multiPart = new PostHttpMultiPart(QHttpMultiPart::FormDataType);
    multiPart->appendPic(pixmap, fileName);
    multiPart->appendTxt("scene", "wcReport");
    multiPart->appendTxt("output", "json2");
    // POST 请求
    QNetworkReply *reply = m_pManager->post(m_picRequest, multiPart);
    multiPart->setParent(reply);
    // 开启事件循环
    QEventLoop eventLoop;
    connect(&timer, &QTimer::timeout, &eventLoop, &QEventLoop::quit);
    connect(m_pManager, &QNetworkAccessManager::finished, &eventLoop, &QEventLoop::quit);
    timer.start();
    eventLoop.exec();
    // 响应处理
    if (timer.isActive()) {
        timer.stop();
        if (reply->error() != QNetworkReply::NoError) {
            qDebug() << "Failed : " << reply->errorString();
        }
        else {
            if (reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() == 200) {
                QJsonObject object = QJsonDocument::fromJson(QString(reply->readAll()).toLocal8Bit().data()).object();
                qDebug()<<object;
                for (auto it = object.begin(); it != object.end(); ++it) {
                    if (it.key() == "path") {
                        picUrlPath = it.value().toString();
                        break;
                    }
                }
            }
        }
    }
    else {
        disconnect(m_pManager, &QNetworkAccessManager::finished, &eventLoop, &QEventLoop::quit);
        reply->abort();
        qDebug()<<"请求超时";
    }
//    reply->deleteLater();
    return picUrlPath;
}

void HttpPost::requestFinished(QNetworkReply *reply)
{
    // 获取http状态码
    QVariant statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    if(statusCode.isValid())
        qDebug() << "status code = " << statusCode.toInt();

    QVariant reason = reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString();
    if(reason.isValid())
        qDebug() << "reason = " << reason.toString();

    QNetworkReply::NetworkError err = reply->error();
    if(err != QNetworkReply::NoError) {
        qDebug() << "Failed : " << reply->errorString();
    }
    else {
        // 获取返回内容
//        qDebug() << QString(reply->readAll());
        QJsonObject object = QJsonDocument::fromJson(QString(reply->readAll()).toLocal8Bit().data()).object();
        for (auto it = object.begin(); it != object.end(); ++it) {
            if (it.key() == "path") {
                break;
            }
        }
    }
    emit finished();
}
