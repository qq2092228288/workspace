#include "httppost.h"
#include "datamanagement.h"
#include "datacalculation.h"
#include <iostream>
using namespace std;

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
    : QObject{parent}
{
    m_pManager = new QNetworkAccessManager(this);

    m_dataRequest.setUrl(QUrl("https://xldl.zeyaotebco.com/qk-wcxl-business/dDevice/sendPationReport"));
    m_dataRequest.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/x-www-form-urlencoded"));
    m_dataRequest.setRawHeader("Connection", "Keep-Alive");

    m_picRequest.setUrl(QUrl("https://file.zeyaotebco.com:29101/group1/upload"));
}

QPixmap HttpPost::jsonToPixmap(const QJsonValue &value) {
    if (value.toString().isEmpty()) return QPixmap();
    auto const encoded = value.toString().toLatin1();
    QPixmap pixmap;
    pixmap.loadFromData(QByteArray::fromBase64(encoded), "PNG");
    return pixmap;
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
            sudokuUrl = picUpload(pixmap, QString("pAnalyse%1.png").arg(QDateTime::currentMSecsSinceEpoch()%60000));
        }
        else if (it.key() == "position") {
            QJsonArray array = it.value().toArray();
            for (auto iterator = array.begin(); iterator != array.end(); ++iterator) {
                QPixmap pixmap = jsonToPixmap(iterator->toObject().find("pDz").value());
                QString fileName = picUpload(pixmap, QString("pDz%1.png").arg(QDateTime::currentMSecsSinceEpoch()%60000));
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
    if (!(sudokuUrl.isEmpty() || fpDzUrl.isEmpty()) && (spExists ? !spDzUrl.isEmpty() : true)) {
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
//        foreach (auto test, m_urlQuery.queryItems()) {
//            qDebug()<<test.first<<test.second;
//        }
        // 超时处理定时器
        QTimer timer;
        timer.setInterval(5000);    // 超时时间
        timer.setSingleShot(true);  // 单次触发
        // 开启事件循环
        QEventLoop eventLoop;
        connect(&timer, &QTimer::timeout, &eventLoop, &QEventLoop::quit);
        connect(m_pManager, &QNetworkAccessManager::finished, &eventLoop, &QEventLoop::quit);
        string headers = string(m_urlQuery.toString().toUtf8());
        QNetworkReply *reply = m_pManager->post(m_dataRequest, QByteArray(headers.c_str(), int(headers.length())));
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
                    QString retData = QString(reply->readAll());
                    qDebug()<<QJsonDocument::fromJson(retData.toUtf8());
                    if (QJsonDocument::fromJson(retData.toUtf8()).object().find("code").value().toInt() != 0) {    // 上传失败
                        emit finished(0);
                        return;
                    }
                }
            }
        }
        else {
            disconnect(m_pManager, &QNetworkAccessManager::finished, &eventLoop, &QEventLoop::quit);
            reply->abort();
            qDebug()<<"请求超时";
        }
    }
    emit finished(dtime);
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
    if (pixmap.isNull()) return nullptr;
    QString picUrlPath;
    // 上传的图片信息
    PostHttpMultiPart *multiPart = new PostHttpMultiPart(QHttpMultiPart::FormDataType);
    multiPart->appendPic(pixmap, fileName);
    multiPart->appendTxt("scene", "wcReport");
    multiPart->appendTxt("output", "json2");
    // 超时处理定时器
    QTimer timer;
    timer.setInterval(5000);    // 超时时间
    timer.setSingleShot(true);  // 单次触发
    // 开启事件循环
    QEventLoop eventLoop;
    connect(&timer, &QTimer::timeout, &eventLoop, &QEventLoop::quit);
    connect(m_pManager, &QNetworkAccessManager::finished, &eventLoop, &QEventLoop::quit);
    // POST 请求
    QNetworkReply *reply = m_pManager->post(m_picRequest, multiPart);
    multiPart->setParent(reply);
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
                QJsonObject object = QJsonDocument::fromJson(QString(reply->readAll()).toLocal8Bit()).object();
                for (auto it = object.begin(); it != object.end(); ++it) {
                    if (it.key() == "data") {
                        QJsonObject dobject = it.value().toObject();
                        for (auto dit = dobject.begin(); dit != dobject.end(); ++dit){
                            if (dit.key() == "path") {
                                picUrlPath = dit.value().toString();
                                break;
                            }
                        }
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
    return picUrlPath;
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
    addDeviceString(Type::SBP, getData(fPos, Type::SBP), getData(sPos, Type::SBP), 0);
    addDeviceString(Type::DBP, getData(fPos, Type::DBP), getData(sPos, Type::DBP), 0);
    addDeviceString(Type::MAP,
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
    qDebug()<<typeName(type)<<fValue<<sValue
           <<QString::number(fValue, 'f', digit)
           <<QString::number(sValue, 'f', digit);
    return addDeviceString(type, QString::number(fValue, 'f', digit), QString::number(sValue, 'f', digit));
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

int HttpPost::getData(const QJsonObject &data, const Type &type)
{
    return data.find(QString::number(type)).value().toInt();
}
