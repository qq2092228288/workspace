#include "httppost.h"
#include "datamanagement.h"
#include "datacalculation.h"
#include <iostream>
using namespace std;


DeviceInfo::DeviceInfo(const QJsonObject &object)
    : m_object{object}
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
}

QString DeviceInfo::find(const QString &key)
{
    return m_object.value(key).toString();
}

void DeviceInfo::print() const {
    qDebug()<<"id"<<id;
    qDebug()<<"name"<<name;
    qDebug()<<"agentName"<<agentName;
    qDebug()<<"hospitalName"<<hospitalName;
    qDebug()<<"secret"<<secret;
    qDebug()<<"place1Id"<<place1Id;
    qDebug()<<"place1Name"<<place1Name;
    qDebug()<<"place2Id"<<place2Id;
    qDebug()<<"place2Name"<<place2Name;
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
      m_urlCreateDevice{"/qk-wcxl-business/dDevice/createDevice"}
{
    m_pManager = new QNetworkAccessManager(this);
}

QPixmap HttpPost::jsonToPixmap(const QJsonValue &value) {
    if (value.toString().isEmpty()) return QPixmap();
    auto const encoded = value.toString().toLatin1();
    QPixmap pixmap;
    pixmap.loadFromData(QByteArray::fromBase64(encoded), "PNG");
    return pixmap;
}

void HttpPost::activeDevice(const QString &mac)
{
    m_urlQuery.clear();

    m_urlQuery.addQueryItem("code", mac);

    QNetworkRequest request = getRequest(m_urlApiRequestHeader + m_urlActiveDevice);
    QNetworkReply *reply = m_pManager->post(request, urlQueryToByteArray());

    auto func = [](QJsonObject object) {
        QJsonObject data = object.find("data").value().toObject();
        return QString(QJsonDocument(data).toJson(QJsonDocument::Compact));
    };
    QString retStr = returnValueProcessing(reply, func);
    if (QJsonDocument::fromJson(retStr.toUtf8()).isObject()) {
        emit deviceInfo(DeviceInfo(QJsonDocument::fromJson(retStr.toUtf8()).object()));
    }
}

bool HttpPost::deviceOnlineNotice(const QString &deviceId)
{
    m_urlQuery.clear();

    m_urlQuery.addQueryItem("deviceId", deviceId);

    QNetworkRequest request = getRequest(m_urlApiRequestHeader + m_urlDeviceOnlineNotice);
    QNetworkReply *reply = m_pManager->post(request, urlQueryToByteArray());

    auto func = [](QJsonObject object) {
        return QString::number(object.find("code").value().toInt(-1));
    };
    if (0 == returnValueProcessing(reply, func).toInt()) {
        return true;
    }
    return false;
}

void HttpPost::receiveConsumable(const QString &id, const QString &deviceId, const int &totalCount)
{
    m_urlQuery.clear();

    m_urlQuery.addQueryItem("id", id);
    m_urlQuery.addQueryItem("deviceId", deviceId);

    QNetworkRequest request = getRequest(m_urlApiRequestHeader + m_urlReceiveConsumable);
    QNetworkReply *reply = m_pManager->post(request, urlQueryToByteArray());

    auto func = [](QJsonObject object) {
        return QString::number(object.find("code").value().toInt(-1));
    };
    if (0 == returnValueProcessing(reply, func).toInt()) {
        emit quantitReceived(id, totalCount);
    }
}

void HttpPost::useConsumable(const QString &deviceId, const QString &consumableUsedData,
                             const QString &consumableId, const int &usedCount)
{
    qDebug()<<consumableUsedData;
    m_urlQuery.clear();

    m_urlQuery.addQueryItem("deviceId", deviceId);
    m_urlQuery.addQueryItem("consumableUsedData", consumableUsedData);

    QNetworkRequest request = getRequest(m_urlApiRequestHeader + m_urlUseConsumable);
    QNetworkReply *reply = m_pManager->post(request, urlQueryToByteArray());

    auto func = [](QJsonObject object) {
        return QString::number(object.find("code").value().toInt(-1));
    };
    if (0 == returnValueProcessing(reply, func).toInt()) {
        emit used(consumableId, usedCount);
    }
}

void HttpPost::getConsumableList(const QString &pageNum, const QString &pageSize, const QString &deviceId,
                                 const QString &id, const QString &consumableTypeId)
{
    m_urlQuery.clear();

    m_urlQuery.addQueryItem("pageNum", pageNum);
    m_urlQuery.addQueryItem("pageSize", pageSize);
    if (!deviceId.isEmpty())
        m_urlQuery.addQueryItem("deviceId", deviceId);
    if (!id.isEmpty())
        m_urlQuery.addQueryItem("id", id);
    if (!consumableTypeId.isEmpty())
        m_urlQuery.addQueryItem("consumableTypeId", consumableTypeId);

    QNetworkRequest request = getRequest(m_urlApiRequestHeader + m_urlGetConsumableList);
    QNetworkReply *reply = m_pManager->post(request, urlQueryToByteArray());

    auto func = [](QJsonObject object) {
        QJsonObject data = object.value("data").toObject();
        QJsonArray list = data.value("list").toArray();
        if (!list.isEmpty()) {
            return QString(QJsonDocument(list.at(0).toObject()).toJson(QJsonDocument::Compact));
        }
        return QString();
    };
    QString list = returnValueProcessing(reply, func);
    if (QJsonDocument::fromJson(list.toUtf8()).isObject()) {
        QJsonObject object = QJsonDocument::fromJson(list.toUtf8()).object();
        int isReceived = object.value("isReceived").toInt();
        if (isReceived == 1) {
            QString id = object.value("id").toString();
            QString deviceId = object.value("deviceId").toString();
            int totalCount = object.value("totalCount").toInt();
            receiveConsumable(id, deviceId, totalCount);
        }
        else if (isReceived == 2) {
            emit repeatedReceived();
        }
    }
}

void HttpPost::reportUpload(const qint64 &dtime, const QString &jsonStr)
{
    m_urlQuery.clear();
    QString sudokuUrl, fpDzUrl, spDzUrl;
    bool spExists = false;
    QJsonObject object = QJsonDocument::fromJson(jsonStr.toUtf8()).object();
    // 上传图片，并获取图片返回的网络地址
    for (auto it = object.begin(); it != object.end(); ++it) {
        if (it.key() == "pAnalyse") {
            QPixmap pixmap = jsonToPixmap(it.value());
            sudokuUrl = picUpload(pixmap, QString("pAnalyse%1.png").arg(QDateTime::currentMSecsSinceEpoch()));
        }
        else if (it.key() == "position") {
            QJsonArray array = it.value().toArray();
            for (auto iterator = array.begin(); iterator != array.end(); ++iterator) {
                QPixmap pixmap = jsonToPixmap(iterator->toObject().find("pDz").value());
                QString fileName = picUpload(pixmap, QString("pDz%1.png").arg(QDateTime::currentMSecsSinceEpoch()));
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
    for (auto it = object.begin(); it != object.end(); ++it) {
        if (it.key() == "place") {  // 场所数据
            addJsonObject(it.value().toObject());
        }
        else if (it.key() == "patientInfo") {  // 患者信息
            QJsonObject temp = it.value().toObject();
            sex = temp.find("sex")->toString().toInt();
            age = temp.find("age")->toString().toInt();
            height = temp.find("height")->toString().toInt();
            weight = temp.find("weight")->toString().toInt();
            addJsonObject(temp);
        }
        else if (it.key() == "position") {      // 数据
            addJsonArray(it.value().toArray(), fpDzUrl, spDzUrl);
        }
        else if (it.key() == "pAnalyse") {      // 九宫格图
            m_urlQuery.addQueryItem(it.key(), sudokuUrl);
        }
        else if (it.key() == "reportConclusion") {      // 九宫格图
            m_urlQuery.addQueryItem(it.key(), it.value().toString());
        }
    }
    QNetworkRequest request = getRequest(m_urlApiRequestHeader + m_urlSendPationReport);
    QNetworkReply *reply = m_pManager->post(request, urlQueryToByteArray());

    auto func = [](QJsonObject object) {
        return QString::number(object.value("code").toInt(-1));
    };
    if (0 == returnValueProcessing(reply, func).toInt()) {
        emit finished(dtime);
    }
}

void HttpPost::createDevice(const QString &mac, const QString &deviceName)
{
    m_urlQuery.clear();

    m_urlQuery.addQueryItem("code", mac);
    m_urlQuery.addQueryItem("name", deviceName);

    QNetworkRequest request = getRequest(m_urlApiRequestHeader + m_urlCreateDevice);
    QNetworkReply *reply = m_pManager->post(request, urlQueryToByteArray());

    auto func = [](QJsonObject object) {
        return QString::number(object.value("code").toInt(-1));
    };
    int code = returnValueProcessing(reply, func).toInt();
    if (0 == code) {
        emit deviceCreated(tr("设备创建成功，请联系厂家进行激活！！！"));
    }
    else if (1 == code) {
        emit deviceCreated(tr("设备已经创建，请勿重复创建！！！"));
    }
    else {
        emit deviceCreated(QString("create error, code = %1").arg(code));
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
    QNetworkRequest request;
    request.setUrl(QUrl(m_urlFileServices + m_urlUploadFile));
    QNetworkReply *reply = m_pManager->post(request, multiPart);
    multiPart->setParent(reply);
    // processing function
    auto func = [](QJsonObject object) {
        QJsonObject data = object.value("data").toObject();
        return data.value("path").toString();
    };
    return returnValueProcessing(reply, func);
}

QNetworkRequest HttpPost::getRequest(const QString &url) const
{
    QNetworkRequest request;
    request.setUrl(QUrl(url));
    request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/x-www-form-urlencoded"));
    request.setRawHeader("Connection", "Keep-Alive");
    return request;
}

QByteArray HttpPost::urlQueryToByteArray()
{
    string headers = string(m_urlQuery.toString().toUtf8());
    return QByteArray(headers.c_str(), int(headers.length()));
}

QString HttpPost::returnValueProcessing(QNetworkReply *reply, QString (*func)(QJsonObject object))
{
    // 超时处理定时器
    QTimer timer;
    timer.setInterval(1000);    // 超时时间
    timer.setSingleShot(true);  // 单次触发
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
                QString ret = QString(reply->readAll());
                qDebug()<<ret;
                return func(QJsonDocument::fromJson(ret.toUtf8()).object());
            }
        }
    }
    else {
        disconnect(m_pManager, &QNetworkAccessManager::finished, &eventLoop, &QEventLoop::quit);
        reply->abort();
        qDebug()<<"请求超时";
    }
    return QString::number(-1);
}

QUrlQuery HttpPost::addJsonObject(const QJsonObject &jsonObject)
{
    for(auto it = jsonObject.begin(); it != jsonObject.end(); ++it) {
        m_urlQuery.addQueryItem(it.key(), it.value().toString());
    }
    return m_urlQuery;
}

QUrlQuery HttpPost::addJsonArray(const QJsonArray &jsonArray, const QString &fpDzUrl, const QString &spDzUrl)
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
    fCvp = sPos.find(QString::number(Type::CVP))->toInt(4);
    sLap = sPos.find(QString::number(Type::LAP))->toInt(9);

    m_urlQuery.addQueryItem("bodySurfaceArea", QString::number(DatCa::cBsa(height, weight), 'f', 2));
    m_urlQuery.addQueryItem("reportTime", reportTime);
    addDeviceString(Type::Dz, fpDzUrl, spDzUrl);
    addBpDeviceString(Type::SBP, getData(fPos, Type::SBP), getData(sPos, Type::SBP), 0);
    addBpDeviceString(Type::DBP, getData(fPos, Type::DBP), getData(sPos, Type::DBP), 0);
    addBpDeviceString(Type::MAP,
                    DatCa::cMap(getData(fPos, Type::SBP), getData(fPos, Type::DBP)),
                    DatCa::cMap(getData(sPos, Type::SBP), getData(sPos, Type::DBP)), 0);
    for (char index = 0; index < 10; ++index) {
        addDeviceString(index, fPos, sPos);
    }

    return m_urlQuery;
}

QUrlQuery HttpPost::addDeviceString(const Type &type, QString fValue, QString sValue)
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

    m_urlQuery.addQueryItem(ArgsNameToHttp(typeName(type)), QJsonDocument(jsonArray).toJson(QJsonDocument::Compact));
    return m_urlQuery;
}

QUrlQuery HttpPost::addDeviceString(const Type &type, qreal fValue, qreal sValue, int digit)
{
    return addDeviceString(type, DatCa::invalid() != fValue ? QString::number(fValue, 'f', digit) : "-",
                           DatCa::invalid() != sValue ? QString::number(sValue, 'f', digit) : "-");
}

QUrlQuery HttpPost::addDeviceString(const char &index, const QJsonObject &fObject, const QJsonObject &sObject)
{
    qreal bsa = DatCa::cBsa(height, weight);
    qreal vept = DatCa::cVept(height, weight, sex);
    qreal fMap = DatCa::cMap(getData(fObject, Type::SBP), getData(fObject, Type::DBP));
    qreal sMap = DatCa::cMap(getData(sObject, Type::SBP), getData(sObject, Type::DBP));
    switch (index) {
    case Type::HR:
        addDeviceString(Type::HR, DatCa::cHr(getData(fObject, Type::HR)), DatCa::cHr(getData(sObject, Type::HR)), 0);
        break;
    case Type::VET:
        addDeviceString(Type::LVET, DatCa::cVet(getData(fObject, Type::VET)), DatCa::cVet(getData(sObject, Type::VET)), 0);
        break;
    case Type::PEP:
    {
        qreal fPep = DatCa::cPep(getData(fObject, Type::PEP));
        qreal sPep = DatCa::cPep(getData(sObject, Type::PEP));
        addDeviceString(Type::PEP, fPep, sPep, 0);
        addDeviceString(Type::STR, DatCa::cStr(fPep, DatCa::cVet(getData(fObject, Type::VET))),
                        DatCa::cStr(sPep, DatCa::cVet(getData(sObject, Type::VET))), 1);
    }
        break;
    case Type::TFC:
        addDeviceString(Type::TFC, DatCa::cTfc(getData(fObject, Type::TFC)), DatCa::cTfc(getData(sObject, Type::TFC)), 3);
        break;
    case Type::EPCI:
        addDeviceString(Type::EPCI, DatCa::cEpci(getData(fObject, Type::EPCI)), DatCa::cEpci(getData(sObject, Type::EPCI)), 3);
        break;
    case Type::ISI:
    {
        qreal fIsi = DatCa::cIsi(getData(fObject, Type::ISI));
        qreal sIsi = DatCa::cIsi(getData(sObject, Type::ISI));
        addDeviceString(Type::ISI, fIsi, sIsi, 2);
        addDeviceString(Type::Ino, DatCa::cIno(fIsi, sex, age), DatCa::cIno(sIsi, sex, age), 0);
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
        addDeviceString(Type::SI, fSi, sSi, 0);
        addDeviceString(Type::SV, fSv, sSv, 1);
        addDeviceString(Type::EDI, DatCa::cEdi(fSi, DatCa::cEf(getData(fObject, Type::EF))),
                        DatCa::cEdi(sSi, DatCa::cEf(getData(sObject, Type::EF))), 0);
        addDeviceString(Type::LSW, DatCa::cLsw(fSv, fMap, fLap), DatCa::cLsw(sSv, sMap, sLap), 0);
        addDeviceString(Type::LSWI, fLswi, sLswi, 1);
        addDeviceString(Type::Vol, DatCa::cVol(fLswi, DatCa::cIno(DatCa::cIsi(getData(fObject, Type::ISI)), sex, age)),
                        DatCa::cVol(sLswi, DatCa::cIno(DatCa::cIsi(getData(sObject, Type::ISI)), sex, age)), 0);
        addDeviceString(Type::SSVR, DatCa::cSsvr(fSv, fMap, fCvp), DatCa::cSsvr(sSv, sMap, sCvp), 1);
        addDeviceString(Type::SSVRI, fSsvri, sSsvri, 1);
        addDeviceString(Type::Vas, DatCa::cVas(fSsvri), DatCa::cVas(sSsvri), 0);
    }
        break;
    case Type::CI:
    {
        qreal fCi = DatCa::cCi(getData(fObject, Type::CI), bsa, vept);
        qreal sCi = DatCa::cCi(getData(sObject, Type::CI), bsa, vept);
        qreal fCo = DatCa::cCo(fCi, bsa);
        qreal sCo = DatCa::cCo(sCi, bsa);
        addDeviceString(Type::CI, fCi, sCi, 1);
        addDeviceString(Type::CO, fCo, sCo, 1);
        addDeviceString(Type::HRV, DatCa::cHrv(fCi), DatCa::cHrv(sCi), 0);
        addDeviceString(Type::SVR, DatCa::cSvr(fCo, fMap, fCvp), DatCa::cSvr(sCo, sMap, sCvp), 0);
        addDeviceString(Type::SVRI, DatCa::cSvri(fCi, fMap, fCvp), DatCa::cSvri(sCi, sMap, sCvp), 0);
        addDeviceString(Type::LCW, DatCa::cLcw(fCo, fMap, fLap), DatCa::cLcw(sCo, sMap, sLap), 1);
        addDeviceString(Type::LCWI, DatCa::cLcwi(fCi, fMap, fLap), DatCa::cLcwi(sCi, sMap, sLap), 2);
    }
        break;
//    case Type::RR:
//        break;
    default:
        break;
    }
    return m_urlQuery;
}

QUrlQuery HttpPost::addBpDeviceString(const Type &type, qreal fValue, qreal sValue, int digit)
{
    return addDeviceString(type, 0 != fValue ? fValue : DatCa::invalid(), 0 != sValue ? sValue : DatCa::invalid(), digit);
}

int HttpPost::getData(const QJsonObject &data, const Type &type)
{
    return data.find(QString::number(type)).value().toInt();
}
