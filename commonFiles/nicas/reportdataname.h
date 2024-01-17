#ifndef REPORTDATANAME_H
#define REPORTDATANAME_H

#include <QObject>
#include <QJsonObject>
#include <QJsonArray>
#include <QMetaEnum>

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
    static QMap<int, int> digitMap(const QJsonArray &array)
    {
        QMap<int, int> map;
        foreach (auto group, array) {
            auto parameters = group.toObject().value("parameters").toArray();
            foreach (auto value, parameters) {
                auto parameter = value.toObject();
                map.insert(parameter.value("type").toInt(), parameter.value("digit").toInt());
            }
        }
        return map;
    }
    template <class T>
    static QString ekey(const T &t)
    {
        return QString(QMetaEnum::fromType<T>().valueToKey(static_cast<int>(t)));
    }
};

#endif // REPORTDATANAME_H
