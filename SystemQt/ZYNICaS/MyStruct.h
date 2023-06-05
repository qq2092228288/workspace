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


struct PatientInfo
{
    QString sex;                // 性别
    QString age;                // 年龄
    QString height;             // 身高
    QString weight;             // 体重
    QString hb;                 // 血红蛋白
    QString patientName;        // 姓名
    QString medicalRecordNumber;// 病历号
};

struct PlaceInfo
{
//    QString placeId;            // 场所ID
//    QString place1Id;           // 一级场所
//    QString place2Id;           // 二级场所
//    QString deviceId;           // 设备ID
    QString primaryPlace;
    QString secondaryPlace;
    QString inspector;          // 检查人员
    QString mac;                // MAC地址
};

struct TebcoData
{
    QMap<uchar, short> data;    // 原始数据
    QDateTime cTime;            // 记录数据时的时间
    QPixmap dzPix;              // 心阻抗图
};

struct BaseData
{
    PlaceInfo place;            // 场所信息
    PatientInfo patient;        // 患者信息
    TebcoData firstPosture;     // 第一体位数据
    TebcoData secondPosture;    // 第二体位数据
    QPixmap sudokuPix;          // 九宫格图
    QString reportConclusion;   // 报告结论
//    QList<QPixmap> trendCharts; // 趋势图
    QString structToJsonString()
    {
        QJsonObject jBaseData;

        QJsonObject jPlace;
//        jPlace.insert("placeId", place.placeId);
//        jPlace.insert("place1Id", place.place1Id);
//        jPlace.insert("place2Id", place.place2Id);
//        jPlace.insert("deviceId", place.deviceId);
        jPlace.insert("primaryPlace", place.primaryPlace);
        jPlace.insert("secondaryPlace", place.secondaryPlace);
        jPlace.insert("inspector", place.inspector);
        jBaseData.insert("place", jPlace);

        QJsonObject jPatient;
        jPatient.insert("sex", patient.sex);
        jPatient.insert("age", patient.age);
        jPatient.insert("height", patient.height);
        jPatient.insert("weight", patient.weight);
        jPatient.insert("hb", patient.hb);
        jPatient.insert("patientName", patient.patientName);
        jPatient.insert("medicalRecordNumber", patient.medicalRecordNumber);
        jBaseData.insert("patientInfo", jPatient);

        QJsonArray jPosition;
        jPosition.append(getPostureJsonObject(firstPosture));
        jPosition.append(getPostureJsonObject(secondPosture));
        jBaseData.insert("position", jPosition);

        jBaseData.insert("pAnalyse", pixmapToJson(sudokuPix));
        jBaseData.insert("reportConclusion", reportConclusion);

//        if (!trendCharts.isEmpty()) {
//            QJsonObject jTrendCharts;
//            for (int index = 0; index < trendCharts.size(); ++index) {
//                jTrendCharts.insert("trendCharts" + QString::number(index), pixmapToJson(trendCharts.at(index)));
//            }
//            jBaseData.insert("trendCharts", jTrendCharts);
//        }
        return QString(QJsonDocument(jBaseData).toJson(QJsonDocument::Compact));
    }
    QJsonObject getPostureJsonObject(const TebcoData &tebcoData)
    {
        QJsonObject jobject;
//        jobject.insert("posture", tebcoData.posture);
        jobject.insert("data", dataToJson(tebcoData.data));
        jobject.insert("reportTime", tebcoData.cTime.toString("yyyyMMddHHmmss"));
        jobject.insert("pDz", pixmapToJson(tebcoData.dzPix));
//        jobject.insert("rSbp", tebcoData.bpAuxArgs.sbp);
//        jobject.insert("rDbp", tebcoData.bpAuxArgs.dbp);
//        jobject.insert("cvp", tebcoData.bpAuxArgs.cvp);
//        jobject.insert("lap", tebcoData.bpAuxArgs.lap);
        return jobject;
    }
    static QJsonObject dataToJson(const QMap<uchar, short> &data)
    {
        QJsonObject jData;
        for (auto it = data.begin(); it != data.end(); ++it) {
            jData.insert(QString::number(it.key()), it.value());
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
};

#endif // MYSTRUCT_H
