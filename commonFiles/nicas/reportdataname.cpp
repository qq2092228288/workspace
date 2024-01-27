#include "reportdataname.h"
#include "reportdatajson.h"
#include "datacalculation.h"
#include "reportset.h"

QMap<int, int> ReportDataName::digitMap(const QJsonArray &array)
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

PatientInfo::PatientInfo()
{

}

PatientInfo::PatientInfo(const QJsonObject &object)
{
    auto info = object.value(ReportDataName::ekey(ReportDataName::patientInfo)).toObject();
    age = info.value(ReportDataName::ekey(ReportDataName::age)).toString().toInt();
    height = info.value(ReportDataName::ekey(ReportDataName::height)).toString().toInt();
    weight = info.value(ReportDataName::ekey(ReportDataName::weight)).toString().toInt();
    hb = info.value(ReportDataName::ekey(ReportDataName::hb)).toString().toDouble();
    bsa = static_cast<int>(DatCa::cBsa(height, weight) * 100) / 100.0;
    medicalRecordNumber = info.value(ReportDataName::ekey(ReportDataName::medicalRecordNumber)).toString();
    patientName = info.value(ReportDataName::ekey(ReportDataName::patientName)).toString();
    sex = info.value(ReportDataName::ekey(ReportDataName::sex)).toString();
    male = sex == QString::fromUtf8("男") ? true : false;
    // auto reportConclusion = object.value(ReportDataName::ekey(ReportDataName::reportConclusion)).toString();
}

Place::Place()
{

}

Place::Place(const QJsonObject &object)
{
    auto place = object.value(ReportDataName::ekey(ReportDataName::place)).toObject();
    consultationPlace = place.value(ReportDataName::ekey(ReportDataName::consultationPlace)).toString();
    inspector = place.value(ReportDataName::ekey(ReportDataName::inspector)).toString();
    if ("unknown" == inspector) inspector.clear();
    mac = place.value(ReportDataName::ekey(ReportDataName::mac)).toString();
    primaryPlace = place.value(ReportDataName::ekey(ReportDataName::primaryPlace)).toString();
    secondaryPlace = place.value(ReportDataName::ekey(ReportDataName::secondaryPlace)).toString();
}

Position::Position()
{

}

Position::Position(const QJsonObject &position, const QJsonObject &info)
{
    allData = position.value(ReportDataName::ekey(ReportDataName::allData)).toArray();
    data = position.value(ReportDataName::ekey(ReportDataName::data)).toObject();
    pos = data.value(QString::number(Type::Pos)).toInt(0);
    auto otime = position.value(ReportDataName::ekey(ReportDataName::reportTime)).toString();
    auto time  = QDateTime::fromString(otime, "yyyyMMddhhmmsszzz");
    if (!time.isValid()) {
        time = QDateTime::fromString(otime, "yyyyMMddhhmmss");
    }
    reportTime = time.toString("yyyy-MM-dd hh:mm:ss");
    waveform = position.value(ReportDataName::ekey(ReportDataName::waveform)).toArray();
    valueMap = ReportDataJson::valueMap(info, data, allData, ReportParameters::array(Check_Mode::IntensiveCareUnit));
}
