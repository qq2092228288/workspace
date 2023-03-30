#include "httppost.h"
#include "datamanagement.h"
#include "datacalculation.h"
#include <iostream>
using namespace std;

JsonObjectData::JsonObjectData(const QJsonObject &object)
    : m_object{object}
{

}

void JsonObjectData::print() const
{
    for (auto it = m_object.begin(); it != m_object.end(); ++it) {
        qDebug()<<it.key()<<it.value();
    }
}

QJsonObject JsonObjectData::object()
{
    return m_object;
}

QString JsonObjectData::find(const QString &key)
{
    return m_object.value(key).toString();
}

DeviceInfo::DeviceInfo(const QJsonObject &object)
    : JsonObjectData{object}
{
    id = find("id");
    name = find("name");
    agentName = find("agentName");
    hospitalName = find("hospitalName");
    secret = find("secret");
    place1Id = find("place1Id");
    place1Name = find("place1Name");
    place2Id = find("place2Id");
    place2Name = find("place2Name");
    signSecret = find("signSecret");
}

void ConsumableUsedData::append(const QString &consumableId, const int &usedCount, const int &usedTag)
{
    QJsonObject object;
    object.insert("consumableId", consumableId);
    object.insert("usedCount", usedCount);
    object.insert("usedTag", QString::number(usedTag));
    object.insert("usedTime", QDateTime::currentDateTime().toString("yyyyMMddhhmmss"));
    m_array.append(object);
}

QString ConsumableUsedData::toJsonStr() const
{
    return QString(QJsonDocument(m_array).toJson(QJsonDocument::Compact));
}

DataList::DataList(const QJsonObject &object)
    : JsonObjectData{object}
{
    createTime = find("createTime");
    typeName = find("typeName");
    isReceived = object.value("isReceived").toInt();
    id = find("id");
    consumableTypeId = find("consumableTypeId");
    descript = find("descript");
    totalCount = object.value("totalCount").toInt();
    usedCount = object.value("usedCount").toInt();
    deviceId = find("deviceId");
    deviceName = find("deviceName");
}

void DataList::print() const
{
    qDebug()<<createTime<<"createTime";
    qDebug()<<typeName<<"typeName";
    qDebug()<<isReceived<<"isReceived";
    qDebug()<<id<<"id";
    qDebug()<<consumableTypeId<<"consumableTypeId";
    qDebug()<<descript<<"descript";
    qDebug()<<totalCount<<"totalCount";
    qDebug()<<usedCount<<"usedCount";
    qDebug()<<deviceId<<"deviceId";
    qDebug()<<deviceName<<"deviceName";
//    qDebug()<<noUploadCount<<"noUploadCount";
}

SoftwareVersionInfo::SoftwareVersionInfo(const QJsonObject &object)
    : JsonObjectData{object}
{
    versionId = find("versionId");
    newVersionCode = find("newVersionCode");
    createTime = find("createTime");
    isForceUpgrade = find("isForceUpgrade");
    actionCommon = find("actionCommon");
    descript = find("descript");
    programFilePath = find("programFilePath");
}

QString ArgsNameToHttp(const QString &argsName)
{
    if (argsName == "CO")           return "cCo";
    else if (argsName == "CI")      return "cCi";
    else if (argsName == "SV")      return "cSv";
    else if (argsName == "SI")      return "cSi";
    else if (argsName == "HRV")     return "cHrv";

    else if (argsName == "SVR")     return "bSvr";
    else if (argsName == "SSVR")    return "bSsvr";
    else if (argsName == "SSVRI")   return "bSsvri";
    else if (argsName == "SVRI")    return "bSvri";
    else if (argsName == "Vas")     return "bVas";

    else if (argsName == "TFC")     return "fTfc";
    else if (argsName == "EDI")     return "fEdi";
    else if (argsName == "Vol")     return "fVol";

    else if (argsName == "PEP")     return "mPep";
    else if (argsName == "LVET")    return "mLvet";
    else if (argsName == "LSW")     return "mLsw";
    else if (argsName == "LSWI")    return "mLswi";
    else if (argsName == "LCW")     return "mLcw";
    else if (argsName == "LCWI")    return "mLcwi";
    else if (argsName == "STR")     return "mStr";
    else if (argsName == "EPCI")    return "mEpci";
    else if (argsName == "ISI")     return "mIsi";
    else if (argsName == "Ino")     return "mIno";

    else if (argsName == "HR")      return "rHr";
    else if (argsName == "SBP")     return "rSbp";
    else if (argsName == "DBP")     return "rDbp";
    else if (argsName == "MAP")     return "rMap";
    else if (argsName == "Dz")      return "pDz";
    return nullptr;
}

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
//    qDebug()<<"~PostHttpMultiPart()";
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
    QFile *file = new QFile(filePath, this);
    file->open(QIODevice::ReadOnly);
    filePart.setBodyDevice(file);

    // we cannot delete the file now, so delete it with the multiPart
    this->append(filePart);
}

void PostHttpMultiPart::appendPic(const QPixmap &pixmap, const QString &fileName)
{
    QString filePath = DataManagement::getInstance().getPaths().tempDir() + fileName;
    if (!pixmap.save(filePath, "PNG")) return;
    QHttpPart filePart;
    QString header=QString("form-data; name=\"file\";filename=\"%1\"").arg(fileName);
    filePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("image/png"));
    filePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant(header));
    QFile *file = new QFile(filePath, this);
    file->open(QIODevice::ReadOnly);
    filePart.setBodyDevice(file);
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
    : QObject{parent},
      m_urlApiRequestHeader{"https://xldl.zeyaotebco.com"},
      m_urlFileServices{"https://file.zeyaotebco.com:29101"},
      m_urlActiveDevice{"/qk-wcxl-business/dDevice/activeDevice"},
      m_urlDeviceOnlineNotice{"/qk-wcxl-business/dDevice/deviceOnlineNotice"},
      m_urlReceiveConsumable{"/qk-wcxl-business/dDevice/receiveConsumable"},
      m_urlUseConsumable{"/qk-wcxl-business/dDevice/useConsumable"},
      m_urlGetConsumableList{"/qk-wcxl-business/dDevice/getConsumableList"},
      m_urlSendPationReport{"/qk-wcxl-business/dDevice/sendPationReport"},
      m_urlUploadFile{"/group1/upload"},
      m_urlCreateDevice{"/qk-wcxl-business/dDevice/createDevice"},
      m_urlCheckDeviceUpdateVersion{"/qk-wcxl-business/dDevice/checkDeviceUpdateVersion"},
      m_urlDoDeviceUpadateVersion{"/qk-wcxl-business/dDevice/doDeviceUpdateVersion"}
{

}

HttpPost::~HttpPost()
{

}

QPixmap HttpPost::jsonToPixmap(const QJsonValue &value) {
    if (value.toString().isEmpty()) return QPixmap();
    auto const encoded = value.toString().toLatin1();
    QPixmap pixmap;
    pixmap.loadFromData(QByteArray::fromBase64(encoded), "PNG");
    return pixmap;
}

void HttpPost::setDeviceInfo(const DeviceInfo &dInfo)
{
    this->m_deviceInfo = dInfo;
}

void HttpPost::activeDevice(const QString &mac)
{
    QJsonObject target;
    target.insert("code", mac);

    QScopedPointer<QNetworkAccessManager> manager_ptr(new QNetworkAccessManager);
    auto manager = manager_ptr.data();
    QNetworkRequest request = getRequest(target, m_urlApiRequestHeader + m_urlActiveDevice, false);
    QNetworkReply *reply = manager->post(request, toPostData(target));

    QJsonObject retValue = returnValueProcessing(manager, reply);
    if (retValue.isEmpty())
        return;
    if (retValue.find("code").value().toInt(-1) == 1) {
        emit deviceNotExist();
        return;
    }
    QJsonObject data = retValue.find("data").value().toObject();
    if (!data.isEmpty()) {
        m_deviceInfo = DeviceInfo(data);
        emit deviceInfo(m_deviceInfo);
    }
}

void HttpPost::deviceOnlineNotice(const QString &deviceId)
{
    if (deviceId.isEmpty())
        return;
    QJsonObject target;
    target.insert("deviceId", deviceId);

    QScopedPointer<QNetworkAccessManager> manager_ptr(new QNetworkAccessManager);
    auto manager = manager_ptr.data();
    QNetworkRequest request = getRequest(target, m_urlApiRequestHeader + m_urlDeviceOnlineNotice);
    QNetworkReply *reply = manager->post(request, toPostData(target));

    QJsonObject retValue = returnValueProcessing(manager, reply);
    bool isOnline = false;
    if (0 == retValue.value("code").toInt(-1)) {
        isOnline = true;
    }
    emit deviceOnline(isOnline);
}

void HttpPost::receiveConsumable(const DataList &dataList)
{
    QJsonObject target;
    target.insert("id", dataList.id);
    target.insert("deviceId", dataList.deviceId);

    QScopedPointer<QNetworkAccessManager> manager_ptr(new QNetworkAccessManager);
    auto manager = manager_ptr.data();
    QNetworkRequest request = getRequest(target, m_urlApiRequestHeader + m_urlReceiveConsumable);
    QNetworkReply *reply = manager->post(request, toPostData(target));

    QJsonObject retValue = returnValueProcessing(manager, reply);
    if (retValue.isEmpty())
        return;
    if (0 == retValue.value("code").toInt(-1)) {
        emit quantitReceived(dataList);
        emit getNewBatch(dataList.totalCount);
    }
}

void HttpPost::useConsumable(const QString &deviceId, const QString &consumableUsedData)
{
    if (deviceId.isEmpty())
        return;
    QJsonObject target;
    target.insert("deviceId", deviceId);
    target.insert("consumableUsedData", consumableUsedData);

    QScopedPointer<QNetworkAccessManager> manager_ptr(new QNetworkAccessManager);
    auto manager = manager_ptr.data();
    QNetworkRequest request = getRequest(target, m_urlApiRequestHeader + m_urlUseConsumable);
    QNetworkReply *reply = manager->post(request, toPostData(target));

    QJsonObject retValue = returnValueProcessing(manager, reply);
    if (retValue.isEmpty())
        return;
    if (0 == retValue.value("code").toInt(-1)) {
        emit used();
    }
}

void HttpPost::getConsumableList(const QString &pageNum, const QString &pageSize, const QString &deviceId,
                                 const QString &id, const QString &consumableTypeId)
{
    if (deviceId.isEmpty())
        return;
    QJsonObject target;
    target.insert("pageNum", pageNum);
    target.insert("pageSize", pageSize);
    target.insert("deviceId", deviceId);
    if (!id.isEmpty())
        target.insert("id", id);
    if (!consumableTypeId.isEmpty())
        target.insert("consumableTypeId", consumableTypeId);

    QScopedPointer<QNetworkAccessManager> manager_ptr(new QNetworkAccessManager);
    auto manager = manager_ptr.data();
    QNetworkRequest request = getRequest(target, m_urlApiRequestHeader + m_urlGetConsumableList);
    QNetworkReply *reply = manager->post(request, toPostData(target));

    QJsonObject retValue = returnValueProcessing(manager, reply);
    if (retValue.isEmpty())
        return;
    QJsonObject data = retValue.value("data").toObject();
    QJsonArray listArray = data.value("list").toArray();
    QString str = QString(QJsonDocument(listArray).toJson());
    if (QJsonDocument::fromJson(str.toUtf8()).isArray()) {
        QJsonArray array = QJsonDocument::fromJson(str.toUtf8()).array();
        for (auto it = array.begin(); it != array.end(); ++it) {
            DataList dataList(it->toObject());
            if (dataList.isReceived == 2) {
                emit quantitReceived(dataList);
            }
            else if (dataList.isReceived == 1) {
                receiveConsumable(dataList);
            }
        }
    }
}

void HttpPost::reportUpload(const qint64 &dtime, const QString &jsonStr)
{
    QString sudokuUrl, fpDzUrl, spDzUrl;
    bool spExists = false;
    QJsonObject object = QJsonDocument::fromJson(jsonStr.toUtf8()).object();
    // 上传图片，并获取图片返回的网络地址
    for (auto it = object.begin(); it != object.end(); ++it) {
        if (it.key() == "pAnalyse") {
            QPixmap pixmap = jsonToPixmap(it.value());
            sudokuUrl = picUpload(pixmap, QString("pAnalyse%1.png").arg(QDateTime::currentSecsSinceEpoch()%10));
        }
        else if (it.key() == "position") {
            QJsonArray array = it.value().toArray();
            for (auto iterator = array.begin(); iterator != array.end(); ++iterator) {
                QPixmap pixmap = jsonToPixmap(iterator->toObject().find("pDz").value());
                QString fileName = picUpload(pixmap, QString("pDz%1.png").arg(QDateTime::currentSecsSinceEpoch()%10));
                if (fpDzUrl.isEmpty()) {
                    fpDzUrl = fileName;
                }
                else if (!pixmap.isNull()) {
                    spExists = true;
                    spDzUrl = fileName;
                }
            }
        }
    }
    // 上传数据
    if (sudokuUrl.isEmpty() || fpDzUrl.isEmpty() || (spExists ? spDzUrl.isEmpty() : false)) {
        emit finished(0);
        return;
    }
    QJsonObject target;
    for (auto it = object.begin(); it != object.end(); ++it) {
        if (it.key() == "place") {  // 场所数据
            addJsonObject(target, it.value().toObject());
        }
        else if (it.key() == "patientInfo") {  // 患者信息
            QJsonObject temp = it.value().toObject();
            sex = temp.find("sex")->toString().toInt();
            age = temp.find("age")->toString().toInt();
            height = temp.find("height")->toString().toInt();
            weight = temp.find("weight")->toString().toInt();
            addJsonObject(target, temp);
        }
        else if (it.key() == "position") {      // 数据
            addJsonArray(target, it.value().toArray(), fpDzUrl, spDzUrl);

        }
        else if (it.key() == "pAnalyse") {      // 九宫格图
            target.insert(it.key(), sudokuUrl);
        }
        else if (it.key() == "reportConclusion") {      // 九宫格图
            target.insert(it.key(), it.value().toString());
        }
    }
    QScopedPointer<QNetworkAccessManager> manager_ptr(new QNetworkAccessManager);
    auto manager = manager_ptr.data();
    QNetworkRequest request = getRequest(target, m_urlApiRequestHeader + m_urlSendPationReport);
    QNetworkReply *reply = manager->post(request, toPostData(target));

    QJsonObject retValue = returnValueProcessing(manager, reply);
    if (retValue.isEmpty())
        return;
    int code = retValue.value("code").toInt(-1);
    if (0 == code) {
        emit finished(dtime);
    }
}

void HttpPost::createDevice(const QString &mac, const QString &deviceName)
{
    QJsonObject target;
    target.insert("code", mac);
    target.insert("name", deviceName);

    QScopedPointer<QNetworkAccessManager> manager_ptr(new QNetworkAccessManager);
    auto manager = manager_ptr.data();
    QNetworkRequest request = getRequest(target, m_urlApiRequestHeader + m_urlCreateDevice, false);
    QNetworkReply *reply = manager->post(request, toPostData(target));

    QString tip = QString("failed to create device!");
    QJsonObject retValue = returnValueProcessing(manager, reply);
    if (!retValue.isEmpty()) {
        tip = QString(QJsonDocument(retValue).toJson());
    }
    emit deviceCreated(tip);
}

void HttpPost::checkDeviceUpdateVersion(const QString &deviceId, const QString &oldVerionId)
{
    QJsonObject target;
    target.insert("deviceId", deviceId);
    target.insert("oldVerionId", oldVerionId);

    QScopedPointer<QNetworkAccessManager> manager_ptr(new QNetworkAccessManager);
    auto manager = manager_ptr.data();
    QNetworkRequest request = getRequest(target, m_urlApiRequestHeader + m_urlCheckDeviceUpdateVersion);
    QNetworkReply *reply = manager->post(request, toPostData(target));

    QJsonObject retValue = returnValueProcessing(manager, reply);
    if (retValue.isEmpty())
        return;
    int code = retValue.value("code").toInt(-1);
    if (0 == code) {
//        QJsonObject data = retValue.find("data").value().toObject();
//        SoftwareVersionInfo versionInfo(data);
    }
}

void HttpPost::doDeviceUpdateVersion(const QString &id, const QString &oldVerionId, const QString &status)
{
    QJsonObject target;
    target.insert("id", id);
    target.insert("oldVerionId", oldVerionId);
    target.insert("status", status);


    QScopedPointer<QNetworkAccessManager> manager_ptr(new QNetworkAccessManager);
    auto manager = manager_ptr.data();
    QNetworkRequest request = getRequest(target, m_urlApiRequestHeader + m_urlDoDeviceUpadateVersion);
    QNetworkReply *reply = manager->post(request, toPostData(target));

    QJsonObject retValue = returnValueProcessing(manager, reply);
    if (retValue.isEmpty())
        return;
    int code = retValue.value("code").toInt(-1);
    if (0 == code) {

    }
}

QString HttpPost::picUpload(const QPixmap &pixmap, const QString &fileName)
{
    if (pixmap.isNull()) return nullptr;
    // 上传的图片信息
    PostHttpMultiPart *multiPart = new PostHttpMultiPart(QHttpMultiPart::FormDataType);
    multiPart->appendPic(pixmap, fileName);
    multiPart->appendTxt("scene", "wcReport");
    multiPart->appendTxt("output", "json2");
    // POST 请求
    QScopedPointer<QNetworkAccessManager> manager_ptr(new QNetworkAccessManager);
    auto manager = manager_ptr.data();
    QNetworkRequest request;
    request.setUrl(QUrl(m_urlFileServices + m_urlUploadFile));
    QNetworkReply *reply = manager->post(request, multiPart);
    multiPart->setParent(reply);

    QJsonObject retValue = returnValueProcessing(manager, reply);
    if (retValue.isEmpty())
        return nullptr;
    return retValue.value("data").toObject().value("path").toString();
}

QNetworkRequest HttpPost::getRequest(const QJsonObject &target, const QString &url, bool authorization) const
{
    QNetworkRequest request;
    request.setUrl(QUrl(url));
    request.setRawHeader(QByteArray("Content-Type"), QByteArray("application/x-www-form-urlencoded"));
    request.setRawHeader(QByteArray("Connection"), QByteArray("Keep-Alive"));
    if (authorization) {    // 授权认证请求头
        QJsonObject jsonObject = target; // signature json object
        QString dKey = m_deviceInfo.id;
        QString timestamp = QString::number(QDateTime::currentMSecsSinceEpoch());
        QString noncestr = QUuid::createUuid().toString().remove("-").remove("{").remove("}");
        jsonObject.insert("dKey", dKey);
        jsonObject.insert("timestamp", timestamp);
        jsonObject.insert("noncestr", noncestr);
        QString signatureStr = QString(QJsonDocument(jsonObject).toJson(QJsonDocument::Compact)) + m_deviceInfo.signSecret;
        QByteArray md5 = QCryptographicHash::hash(signatureStr.toUtf8(), QCryptographicHash::Md5);
        QString signature = QString(md5.toHex());
        request.setRawHeader(QByteArray("signature"), signature.toUtf8());
        request.setRawHeader(QByteArray("dKey"), dKey.toUtf8());
        request.setRawHeader(QByteArray("timestamp"), timestamp.toUtf8());
        request.setRawHeader(QByteArray("noncestr"), noncestr.toUtf8());
//        std::cout<<signatureStr.toLocal8Bit().data()<<std::endl;
//        std::cout<<"signature "<<signature.toStdString()<<std::endl;
//        std::cout<<"dKey "<<dKey.toStdString()<<std::endl;
//        std::cout<<"timestamp "<<timestamp.toStdString()<<std::endl;
//        std::cout<<"noncestr "<<noncestr.toStdString()<<std::endl;
    }

    return request;
}

QByteArray HttpPost::toPostData(const QJsonObject &target)
{
    QUrlQuery urlQuery;
    for (auto it = target.begin(); it < target.end(); ++it) {
        urlQuery.addQueryItem(it.key(), it.value().toString());
    }
    string headers = string(urlQuery.toString().toUtf8());
    return QByteArray(headers.c_str(), int(headers.length()));
}

QJsonObject HttpPost::returnValueProcessing(QNetworkAccessManager *manager, QNetworkReply *reply)
{
    // 超时处理定时器
    QTimer timer;
    timer.setInterval(5000);    // 超时时间
    timer.setSingleShot(true);  // 单次触发
    // 开启事件循环
    QEventLoop eventLoop;
    connect(&timer, &QTimer::timeout, &eventLoop, &QEventLoop::quit);
    connect(manager, &QNetworkAccessManager::finished, &eventLoop, &QEventLoop::quit);
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
                QString ret = QString(reply->readAll());
//                std::cout<<ret.toLocal8Bit().data()<<std::endl;
                return QJsonDocument::fromJson(ret.toUtf8()).object();
            }
        }
    }
    else {
        disconnect(manager, &QNetworkAccessManager::finished, &eventLoop, &QEventLoop::quit);
        reply->abort();
        qDebug()<<"请求超时"<<reply->url();
    }
    return QJsonObject();
}

void HttpPost::addJsonObject(QJsonObject &target, const QJsonObject &jsonObject)
{
    for(auto it = jsonObject.begin(); it != jsonObject.end(); ++it) {
        target.insert(it.key(), it.value());
    }
}

void HttpPost::addJsonArray(QJsonObject &target, const QJsonArray &jsonArray, const QString &fpDzUrl, const QString &spDzUrl)
{
    QString reportTime;
    QJsonObject fPos, sPos;
    for(auto iterator = jsonArray.begin(); iterator != jsonArray.end(); ++iterator) {
        QJsonObject jsonObject = iterator->toObject();
        for(auto it = jsonObject.begin(); it != jsonObject.end(); ++it) {
            if (it.key() == "data") {
                fPos.isEmpty() ? fPos = it.value().toObject() : sPos = it.value().toObject();
            }
            else if (it.key() == "reportTime") {
                it.value().toString().isEmpty() ? nullptr : reportTime = it.value().toString();
            }
        }
    }

    fposture = getData(fPos, Type::Pos);
    fCvp = fPos.find(QString::number(Type::CVP))->toInt(4);
    fLap = fPos.find(QString::number(Type::LAP))->toInt(9);
    sposture = getData(sPos, Type::Pos);
    sCvp = sPos.find(QString::number(Type::CVP))->toInt(4);
    sLap = sPos.find(QString::number(Type::LAP))->toInt(9);

    target.insert("bodySurfaceArea", QString::number(DatCa::cBsa(height, weight), 'f', 2));
    target.insert("reportTime", reportTime);

    addDeviceString(target, Type::Dz, fpDzUrl, spDzUrl);
    addBpDeviceString(target, Type::SBP, getData(fPos, Type::SBP), getData(sPos, Type::SBP), 0);
    addBpDeviceString(target, Type::DBP, getData(fPos, Type::DBP), getData(sPos, Type::DBP), 0);
    addBpDeviceString(target, Type::MAP,
                    DatCa::cMap(getData(fPos, Type::SBP), getData(fPos, Type::DBP)),
                    DatCa::cMap(getData(sPos, Type::SBP), getData(sPos, Type::DBP)), 0);
    for (char index = 0; index < 10; ++index) {
        addDeviceString(target, index, fPos, sPos);
    }
}

void HttpPost::addDeviceString(QJsonObject &target, const Type &type, QString fValue, QString sValue)
{
    Argument argument = Args().findArgument(typeName(type));
    QString name = QString("%1/%2").arg(argument.cn, argument.en);
    QString referenceRange = QString("%1~%2").arg(argument.min).arg(argument.max);
    if (Type::DBP == type) {
        name = QString("%1/%2").arg(argument.dbpcn, argument.en);
        referenceRange = QString("%1~%2").arg(argument.dbpmin).arg(argument.dbpmax);
    }

    QJsonObject fObject;
    fObject.insert("name", name);
    fObject.insert("posture", QString::number(fposture));
    fObject.insert("referenceRange", referenceRange);
    fObject.insert("unit", argument.unit);
    fObject.insert("value", fValue);

    QJsonObject sObject;
    sObject.insert("name", name);
    sObject.insert("posture", QString::number(sposture));
    sObject.insert("referenceRange", referenceRange);
    sObject.insert("unit", argument.unit);
    sObject.insert("value", sValue);

    QJsonArray jsonArray;
    jsonArray.append(fObject);
    jsonArray.append(sObject);

    target.insert(ArgsNameToHttp(typeName(type)), QString(QJsonDocument(jsonArray).toJson(QJsonDocument::Compact)));
}

void HttpPost::addDeviceString(QJsonObject &target, const Type &type, qreal fValue, qreal sValue, int digit)
{
    return addDeviceString(target, type, DatCa::invalid() != fValue ? QString::number(fValue, 'f', digit) : "-",
                           DatCa::invalid() != sValue ? QString::number(sValue, 'f', digit) : "-");
}

void HttpPost::addDeviceString(QJsonObject &target, const char &index, const QJsonObject &fObject, const QJsonObject &sObject)
{
    qreal bsa = DatCa::cBsa(height, weight);
    qreal vept = DatCa::cVept(height, weight, sex);
    qreal fMap = DatCa::cMap(getData(fObject, Type::SBP), getData(fObject, Type::DBP));
    qreal sMap = DatCa::cMap(getData(sObject, Type::SBP), getData(sObject, Type::DBP));
    switch (index) {
    case Type::HR:
        addDeviceString(target, Type::HR, DatCa::cHr(getData(fObject, Type::HR)), DatCa::cHr(getData(sObject, Type::HR)), 0);
        break;
    case Type::VET:
        addDeviceString(target, Type::LVET, DatCa::cVet(getData(fObject, Type::VET)), DatCa::cVet(getData(sObject, Type::VET)), 0);
        break;
    case Type::PEP:
    {
        qreal fPep = DatCa::cPep(getData(fObject, Type::PEP));
        qreal sPep = DatCa::cPep(getData(sObject, Type::PEP));
        addDeviceString(target, Type::PEP, fPep, sPep, 0);
        addDeviceString(target, Type::STR, DatCa::cStr(fPep, DatCa::cVet(getData(fObject, Type::VET))),
                        DatCa::cStr(sPep, DatCa::cVet(getData(sObject, Type::VET))), 1);
    }
        break;
    case Type::TFC:
        addDeviceString(target, Type::TFC, DatCa::cTfc(getData(fObject, Type::TFC)), DatCa::cTfc(getData(sObject, Type::TFC)), 3);
        break;
    case Type::EPCI:
        addDeviceString(target, Type::EPCI, DatCa::cEpci(getData(fObject, Type::EPCI)), DatCa::cEpci(getData(sObject, Type::EPCI)), 3);
        break;
    case Type::ISI:
    {
        qreal fIsi = DatCa::cIsi(getData(fObject, Type::ISI));
        qreal sIsi = DatCa::cIsi(getData(sObject, Type::ISI));
        addDeviceString(target, Type::ISI, fIsi, sIsi, 2);
        addDeviceString(target, Type::Ino, DatCa::cIno(fIsi, sex, age), DatCa::cIno(sIsi, sex, age), 0);
    }
        break;
//    case Type::EF:
//        break;
    case Type::SI:
    {
        qreal fSi = DatCa::cSi(getData(fObject, Type::SI), bsa, vept);
        qreal sSi = DatCa::cSi(getData(sObject, Type::SI), bsa, vept);
        qreal fSv = DatCa::cSv(fSi, bsa);
        qreal sSv = DatCa::cSv(sSi, bsa);
        qreal fLswi = DatCa::cLswi(fSi, fMap, fLap);
        qreal sLswi = DatCa::cLswi(sSi, sMap, sLap);
        qreal fSsvri = DatCa::cSsvri(fSi, fMap, fCvp);
        qreal sSsvri = DatCa::cSsvri(sSi, sMap, sCvp);
        addDeviceString(target, Type::SI, fSi, sSi, 0);
        addDeviceString(target, Type::SV, fSv, sSv, 1);
        addDeviceString(target, Type::EDI, DatCa::cEdi(fSi, DatCa::cEf(getData(fObject, Type::EF))),
                        DatCa::cEdi(sSi, DatCa::cEf(getData(sObject, Type::EF))), 0);
        addDeviceString(target, Type::LSW, DatCa::cLsw(fSv, fMap, fLap), DatCa::cLsw(sSv, sMap, sLap), 0);
        addDeviceString(target, Type::LSWI, fLswi, sLswi, 1);
        addDeviceString(target, Type::Vol, DatCa::cVol(fLswi, DatCa::cIno(DatCa::cIsi(getData(fObject, Type::ISI)), sex, age)),
                        DatCa::cVol(sLswi, DatCa::cIno(DatCa::cIsi(getData(sObject, Type::ISI)), sex, age)), 0);
        addDeviceString(target, Type::SSVR, DatCa::cSsvr(fSv, fMap, fCvp), DatCa::cSsvr(sSv, sMap, sCvp), 1);
        addDeviceString(target, Type::SSVRI, fSsvri, sSsvri, 1);
        addDeviceString(target, Type::Vas, DatCa::cVas(fSsvri), DatCa::cVas(sSsvri), 0);
    }
        break;
    case Type::CI:
    {
        qreal fCi = DatCa::cCi(getData(fObject, Type::CI), bsa, vept);
        qreal sCi = DatCa::cCi(getData(sObject, Type::CI), bsa, vept);
        qreal fCo = DatCa::cCo(fCi, bsa);
        qreal sCo = DatCa::cCo(sCi, bsa);
        addDeviceString(target, Type::CI, fCi, sCi, 1);
        addDeviceString(target, Type::CO, fCo, sCo, 1);
        addDeviceString(target, Type::HRV, DatCa::cHrv(fCi), DatCa::cHrv(sCi), 0);
        addDeviceString(target, Type::SVR, DatCa::cSvr(fCo, fMap, fCvp), DatCa::cSvr(sCo, sMap, sCvp), 0);
        addDeviceString(target, Type::SVRI, DatCa::cSvri(fCi, fMap, fCvp), DatCa::cSvri(sCi, sMap, sCvp), 0);
        addDeviceString(target, Type::LCW, DatCa::cLcw(fCo, fMap, fLap), DatCa::cLcw(sCo, sMap, sLap), 1);
        addDeviceString(target, Type::LCWI, DatCa::cLcwi(fCi, fMap, fLap), DatCa::cLcwi(sCi, sMap, sLap), 2);
    }
        break;
//    case Type::RR:
//        break;
    default:
        break;
    }
}

void HttpPost::addBpDeviceString(QJsonObject &target, const Type &type, qreal fValue, qreal sValue, int digit)
{
    return addDeviceString(target, type, 0 != fValue ? fValue : DatCa::invalid(), 0 != sValue ? sValue : DatCa::invalid(), digit);
}

int HttpPost::getData(const QJsonObject &data, const Type &type)
{
    return data.find(QString::number(type)).value().toInt();
}
