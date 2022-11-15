#ifndef MYSTRUCT_H
#define MYSTRUCT_H

#include <QObject>
#include <QPixmap>
#include <QDateTime>
#include <QMap>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QBuffer>
#include <QUrlQuery>
#include <iostream>
using namespace std;
#include "httppost.h"

struct BpAndAuxArgs
{
    int sbp;                    // 收缩压
    int dbp;                    // 舒张压
    int cvp;                    // 中心静脉压
    int lap;                    // 左房压
};

struct PatientInfo
{
    int sex;                    // 性别: 0男,1女
    int age;                    // 年龄
    int height;                 // 身高
    int weight;                 // 体重
    QString name;               // 姓名
    QString medicalRecordNumber;// 病历号
};

struct PlaceInfo
{
    QString placeId;            // 场所ID
    QString place1Id;           // 一级场所
    QString place2Id;           // 二级场所
    QString deviceId;           // 设备ID
    QString inspector;          // 检察人员
};

struct TebcoData
{
    int posture = -1;           // (-1表示无数据)体位：1半卧，2平躺，3抬腿
    QMap<uchar, short> data;    // 原始数据
    QDateTime cTime;            // 记录数据时的时间
    QPixmap dzPix;              // 心阻抗图
    BpAndAuxArgs bpAuxArgs;     // 血压和辅助参数
};

struct BaseData
{
    PlaceInfo place;            // 场所信息
    PatientInfo patient;        // 患者信息
    TebcoData firstPosture;     // 第一体位数据
    TebcoData secondPosture;    // 第二体位数据
    QPixmap sudokuPix;          // 九宫格图
    QList<QPixmap> trendCharts; // 趋势图
    QString structToJson()
    {
        QJsonObject jBaseData;

        QJsonObject jPlace;
        jPlace.insert("placeId", place.placeId);
        jPlace.insert("place1Id", place.place1Id);
        jPlace.insert("place2Id", place.place2Id);
        jPlace.insert("deviceId", place.deviceId);
        jPlace.insert("inspector", place.inspector);
        jBaseData.insert("place", jPlace);

        QJsonObject jPatient;
        jPatient.insert("sex", patient.sex);
        jPatient.insert("age", patient.age);
        jPatient.insert("height", patient.height);
        jPatient.insert("weight", patient.weight);
        jPatient.insert("name", patient.name);
        jPatient.insert("medicalRecordNumber", patient.medicalRecordNumber);
        jBaseData.insert("patientInfo", jPatient);

        QJsonObject jFirstPosture;
        insertBpAndArgs(jFirstPosture, firstPosture);
        jBaseData.insert("firstPosition", jFirstPosture);
        if (secondPosture.posture != -1) {
            QJsonObject jSecondPosture;
            insertBpAndArgs(jSecondPosture, secondPosture);
            jBaseData.insert("secondPosition", jSecondPosture);
        }

        jBaseData.insert("pAnalyse", pixmapToJson(sudokuPix));

        if (!trendCharts.isEmpty()) {
            QJsonObject jTrendCharts;
            for (int index = 0; index < trendCharts.size(); ++index) {
                jTrendCharts.insert("trendCharts" + QString::number(index), pixmapToJson(trendCharts.at(index)));
            }
            jBaseData.insert("trendCharts", jTrendCharts);
        }
        return QString(QJsonDocument(jBaseData).toJson(QJsonDocument::Compact));
    }
    void insertBpAndArgs(QJsonObject &jobject, const TebcoData &tebcoData)
    {
        jobject.insert("posture", tebcoData.posture);
        jobject.insert("data", dataToJson(tebcoData.data));
        jobject.insert("reportTime", tebcoData.cTime.toSecsSinceEpoch());
        jobject.insert("pDz", pixmapToJson(tebcoData.dzPix));
        jobject.insert("rSbp", tebcoData.bpAuxArgs.sbp);
        jobject.insert("rDbp", tebcoData.bpAuxArgs.dbp);
        jobject.insert("cvp", tebcoData.bpAuxArgs.cvp);
        jobject.insert("lap", tebcoData.bpAuxArgs.lap);
    }
    static QJsonObject dataToJson(const QMap<uchar, short> &data)
    {
        QJsonObject jData;
        for (auto it = data.begin(); it != data.end(); ++it) {
            jData.insert("data" + QString::number(it.key()), it.value());
        }
        return jData;
    }
    static QJsonValue pixmapToJson(const QPixmap &pixmap)
    {
        QBuffer buffer;
        buffer.open(QIODevice::WriteOnly);
        pixmap.save(&buffer, "PNG");
        auto const encoded = buffer.data().toBase64();
        return {QLatin1String(encoded)};
    }
    static QPixmap jsonToPixmap(const QJsonValue &val) {
        auto const encoded = val.toString().toLatin1();
        QPixmap pixmap;
        pixmap.loadFromData(QByteArray::fromBase64(encoded), "PNG");
        return pixmap;
    }
    static QJsonObject strToJson(const QString &str)
    {
        return QJsonDocument::fromJson(str.toLocal8Bit().data()).object();
    }
//    static QByteArray jsonStrToUrlByteArray(const QString &str)
//    {
//        QUrlQuery urlQuery;
//        QJsonObject firstPosition, secondPosition;
//        QJsonObject object = QJsonDocument::fromJson(str.toLocal8Bit().data()).object();
//        for (auto it = object.begin(); it != object.end(); ++it) {
//            if (it.key() == "pAnalyse") {
//                QPixmap pixmap = jsonToPixmap(it.value());
//            }
//        }
//        for (auto it = object.begin(); it != object.end(); ++it) {
//            if (it.key() == "place" || it.key() == "patientInfo") {
//                urlQuerayInsertItems(urlQuery, it.value().toObject());
//            }
//            else if (it.key() == "firstPosition") {
//                firstPosition = it.value().toObject();
//            }
//            else if (it.key() == "secondPosition") {
//                secondPosition = it.value().toObject();
//            }
//            else if (it.key() == "pAnalyse") {

//            }
//        }


//        string headers = string(urlQuery.toString().toUtf8());
//        return QByteArray(headers.c_str(), int(headers.length()));
//    }
//    static void urlQuerayInsertItems(QUrlQuery &urlQuery, const QJsonObject &object)
//    {
//        for (auto it = object.begin(); it != object.end(); ++it) {
//            urlQuery.addQueryItem(it.key(), it.value().toString());
//        }
//    }
};

#endif // MYSTRUCT_H
