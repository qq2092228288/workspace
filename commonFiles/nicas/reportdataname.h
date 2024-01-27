#ifndef REPORTDATANAME_H
#define REPORTDATANAME_H

#include <QObject>
#include <QJsonObject>
#include <QJsonArray>
#include <QMetaEnum>
#include <QDateTime>

#include "datatype.h"

class ReportDataName : public QObject
{
    Q_OBJECT
public:
    enum ReportJson
    {
        module,
        parameters,
        cn,
        en,
        unit,
        min,
        max,
        digit,
        type,
        desc
    };
    Q_ENUM(ReportJson)
    enum ReportData
    {
        consultation,
        inquiry,
        patientInfo,
        place,
        position,
        reportConclusion,
        startTime
    };
    Q_ENUM(ReportData)
    enum PatientInfoStruct
    {
        age,
        hb,
        height,
        medicalRecordNumber,
        patientName,
        sex,
        weight
    };
    Q_ENUM(PatientInfoStruct)
    enum ConsultationStruct
    {
        signature,
        suggestion,
        updateTime
    };
    Q_ENUM(ConsultationStruct)
    enum InquiryStruct
    {
        fhh,
        edh,
        ltsh,
        lthms,
        ptm,
        al
    };
    Q_ENUM(InquiryStruct)
    enum PlaceStruct
    {
        inspector,
        primaryPlace,
        secondaryPlace,
        mac,
        consultationPlace
    };
    Q_ENUM(PlaceStruct)
    enum PositionStruct
    {
        data,
        reportTime,
        allData,
        waveform
    };
    Q_ENUM(PositionStruct)
    static QMap<int, int> digitMap(const QJsonArray &array);
    template <class T>
    static QString ekey(const T &t)
    {
        return QString(QMetaEnum::fromType<T>().valueToKey(static_cast<int>(t)));
    }
};

struct PatientInfo
{
    PatientInfo();
    PatientInfo(const QJsonObject &object);
    bool male;
    int age;
    int height;
    int weight;
    double hb;
    double bsa;
    QString medicalRecordNumber;
    QString patientName;
    QString sex;
};

struct Place
{
    Place();
    Place(const QJsonObject &object);
    QString consultationPlace;
    QString inspector;
    QString mac;
    QString primaryPlace;
    QString secondaryPlace;
};

struct Position
{
    Position();
    Position(const QJsonObject &position, const QJsonObject &info);
    int pos;
    QJsonArray allData;
    QJsonObject data;
    QString reportTime;
    QJsonArray waveform;
    QMap<Type, qreal> valueMap;
};

#endif // REPORTDATANAME_H
