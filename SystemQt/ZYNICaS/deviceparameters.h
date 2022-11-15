#ifndef DEVICEPARAMETERS_H
#define DEVICEPARAMETERS_H

#include <QObject>
#include <QJsonValue>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QDebug>
#include <QUrlQuery>

namespace DeviceParameters {

//template<class T>
//static QJsonObject JsonObject(QString argName, T argValue)
//{
//    QJsonObject jsonObject;
//    jsonObject.insert(argName, QJsonValue::fromVariant(argValue));
//    return jsonObject;
//}

QString SinglePos(QString name, QString posture, QString referenceRange,
                             QString unit, QString value)
{
    QJsonObject jsonObject;
    jsonObject.insert("name", QJsonValue::fromVariant(name));
    jsonObject.insert("posture", QJsonValue::fromVariant(posture));
    jsonObject.insert("referenceRange", QJsonValue::fromVariant(referenceRange));
    jsonObject.insert("unit", QJsonValue::fromVariant(unit));
    jsonObject.insert("value", QJsonValue::fromVariant(value));
    return QString(QJsonDocument(jsonObject).toJson(QJsonDocument::Compact));

//    QUrlQuery urlQuery;
//    urlQuery.addQueryItem("name", name);
//    urlQuery.addQueryItem("posture", posture);
//    urlQuery.addQueryItem("referenceRange", referenceRange);
//    urlQuery.addQueryItem("unit", unit);
//    urlQuery.addQueryItem("value", value);
//    return urlQuery.toString(QUrl::FullyDecoded);
}

QString DoublePos(QString name, QString fPosture, QString sPosture, QString referenceRange,
                            QString unit, QString fValue, QString sValue)
{
//    QJsonArray fJsonArray;
//    fJsonArray.append(SinglePos(name, fPosture, referenceRange, unit, fValue));
//    fJsonArray.append(SinglePos(name, sPosture, referenceRange, unit, sValue));
    return QString("[%1,%2]").arg(SinglePos(name, fPosture, referenceRange, unit, fValue),
                                  SinglePos(name, sPosture, referenceRange, unit, sValue));

//    QUrlQuery urlQuery;
//    urlQuery.addQueryItem("name", name);
//    urlQuery.addQueryItem("posture", fPosture);
//    urlQuery.addQueryItem("referenceRange", referenceRange);
//    urlQuery.addQueryItem("unit", unit);
//    urlQuery.addQueryItem("value", fValue);
//    urlQuery.addQueryItem("name", name);
//    urlQuery.addQueryItem("posture", sPosture);
//    urlQuery.addQueryItem("referenceRange", referenceRange);
//    urlQuery.addQueryItem("unit", unit);
//    urlQuery.addQueryItem("value", sValue);
//    return urlQuery.toString(QUrl::FullyDecoded);
}

}

#endif // DEVICEPARAMETERS_H
