#include "reportdatajson.h"
#include <QDateTime>
#include "datamanagement.h"
#include "datacalculation.h"

ReportDataJson::ReportDataJson(QObject *parent)
    : QObject{parent}
{

}

QJsonObject ReportDataJson::getJson()
{
    // 场所信息
    auto info = DataManagement::getInstance().getHospitalInfo();
    json.insert(ReportDataName::ekey(ReportDataName::place), QJsonObject
                {
                    { ReportDataName::ekey(ReportDataName::primaryPlace), info->hospitalName },
                    { ReportDataName::ekey(ReportDataName::secondaryPlace), info->roomName },
                    { ReportDataName::ekey(ReportDataName::inspector), info->doctorName },
                    { ReportDataName::ekey(ReportDataName::consultationPlace), info->consultationHospitalName },
                    { ReportDataName::ekey(ReportDataName::mac), DataManagement::getInstance().getMac() }
                });
    return json;
}

QString ReportDataJson::getJsonString()
{
    return QString(QJsonDocument(getJson()).toJson(QJsonDocument::Compact));
}

QString ReportDataJson::getReportTime()
{
    auto array = getPosition();
    auto reportTime = array.at(1).toObject().value(ReportDataName::ekey(ReportDataName::reportTime)).toString();
    if (reportTime.isEmpty()) {
        reportTime = array.at(0).toObject().value(ReportDataName::ekey(ReportDataName::reportTime)).toString();
    }
    return reportTime;
}

QMap<Type, qreal> ReportDataJson::valueMap(const QJsonObject &info, const QJsonObject &data, const QJsonArray &alldata)
{
    // 基本数据
    auto age = info.value(ReportDataName::ekey(ReportDataName::age)).toString().toInt();
    auto height = info.value(ReportDataName::ekey(ReportDataName::height)).toString().toInt();
    auto weight = info.value(ReportDataName::ekey(ReportDataName::weight)).toString().toInt();
    auto sex = info.value(ReportDataName::ekey(ReportDataName::sex)).toString() == "男" ? 0 : 1;
    auto hb = info.value(ReportDataName::ekey(ReportDataName::hb)).toString().toInt();
    auto sbp = data.value(QString::number(Type::SBP)).toInt();
    auto dbp = data.value(QString::number(Type::DBP)).toInt();
    auto lap = data.value(QString::number(Type::LAP)).toInt();
    auto cvp = data.value(QString::number(Type::CVP)).toInt();
    // 计算值
    auto bsa = DatCa::cBsa(height, weight);
    auto vept = DatCa::cVept(height, weight, sex);
    auto map = DatCa::cMap(sbp, dbp);

    QMap<Type, qreal> vMap;
    vMap.insert(Type::Pos, data.value(QString::number(Type::Pos)).toInt());
    vMap.insert(Type::SBP, sbp);
    vMap.insert(Type::DBP, dbp);
    vMap.insert(Type::LAP, lap);
    vMap.insert(Type::CVP, cvp);
    vMap.insert(Type::MAP, map);

    auto lvet = DatCa::cVet(data.value(QString::number(Type::VET)).toInt());
    auto pep = DatCa::cPep(data.value(QString::number(Type::PEP)).toInt());
    auto isi = DatCa::cIsi(data.value(QString::number(Type::ISI)).toInt());
    auto ef = DatCa::cEf(data.value(QString::number(Type::EF)).toInt());
    auto ino = DatCa::cIno(isi, sex, age);
    auto si = DatCa::cSi(data.value(QString::number(Type::SI)).toInt(), bsa, vept);
    auto sv = DatCa::cSv(si, bsa);
    auto lswi = DatCa::cLswi(si, map, lap);
    auto ssvri = DatCa::cSsvri(si, map, cvp);
    auto ci = DatCa::cCi(data.value(QString::number(Type::CI)).toInt(), bsa, vept);
    auto co = DatCa::cCo(ci, bsa);
    QVector<qreal> svs;
    auto osis = allValue(Type::SI, alldata);
    if (osis.size() > 25) {
        osis = osis.mid(osis.size() - 25);
    }
    foreach (auto value, osis) {
        svs<<(vept * value / 3400);
    }

    vMap.insert(Type::HR, DatCa::cHr(data.value(QString::number(Type::HR)).toInt()));
    vMap.insert(Type::LVET, lvet);
    vMap.insert(Type::PEP, pep);
    vMap.insert(Type::STR, DatCa::cStr(pep, lvet));
    vMap.insert(Type::TFC, DatCa::cTfc(data.value(QString::number(Type::TFC)).toInt()));
    vMap.insert(Type::EPCI, DatCa::cEpci(data.value(QString::number(Type::EPCI)).toInt()));
    vMap.insert(Type::ISI, isi);
    vMap.insert(Type::Ino, ino);
    vMap.insert(Type::SI, si);
    vMap.insert(Type::SV, sv);
    vMap.insert(Type::EDI, DatCa::cEdi(si, ef));
    vMap.insert(Type::LSW, DatCa::cLsw(sv, map, lap));
    vMap.insert(Type::LSWI, lswi);
    vMap.insert(Type::Vol, DatCa::cVol(lswi, ino));
    vMap.insert(Type::SSVR, DatCa::cSsvr(sv, map, cvp));
    vMap.insert(Type::SSVRI, DatCa::cSsvri(si, map, cvp));
    vMap.insert(Type::Vas, DatCa::cVas(ssvri));
    vMap.insert(Type::SVV, DatCa::cSvv(sv, svs));
    vMap.insert(Type::CI, ci);
    vMap.insert(Type::CO, co);
    vMap.insert(Type::HRV, DatCa::cHrv(ci));
    vMap.insert(Type::SVR, DatCa::cSvr(co, map, cvp));
    vMap.insert(Type::SVRI, DatCa::cSvri(ci, map, cvp));
    vMap.insert(Type::LCW, DatCa::cLcw(co, map, lap));
    vMap.insert(Type::LCWI, DatCa::cLcwi(ci, map, lap));
    vMap.insert(Type::DO2, DatCa::cDo2(co, hb));

    return vMap;
}

QVector<qreal> ReportDataJson::allValue(Type type, const QJsonArray &alldata)
{
    QVector<qreal> vector;
    foreach (auto value, alldata) {
        auto v = value.toObject().value(QString::number(type)).toDouble(-1);
        if (-1 != v) {
            vector.append(v);
        }
    }
    return vector;
}

void ReportDataJson::clear()
{
    json = QJsonObject();
    data = QJsonObject();
    allData = QJsonArray();
    waveform = QJsonArray();
}

void ReportDataJson::clearCheckData()
{
    json.remove(ReportDataName::ekey(ReportDataName::position));
}

void ReportDataJson::setPatientInfo(int age, float hb, int height, int weight, int sex,
                                    const QString &patientName, const QString &medicalRecordNumber,
                                    int fhh, int edh, int ltsh, int lthms, int ptm, int al)
{
    clear();
    json.insert(ReportDataName::ekey(ReportDataName::startTime), time());
    json.insert(ReportDataName::ekey(ReportDataName::patientInfo), QJsonObject
                {
                    { ReportDataName::ekey(ReportDataName::age), QString::number(age) },
                    { ReportDataName::ekey(ReportDataName::hb), QString::number(hb) },
                    { ReportDataName::ekey(ReportDataName::height), QString::number(height) },
                    { ReportDataName::ekey(ReportDataName::weight), QString::number(weight) },
                    { ReportDataName::ekey(ReportDataName::sex), (sex == 0 ? QString("男") : QString("女")) },
                    { ReportDataName::ekey(ReportDataName::patientName), patientName },
                    { ReportDataName::ekey(ReportDataName::medicalRecordNumber), medicalRecordNumber }
                });
    json.insert(ReportDataName::ekey(ReportDataName::inquiry), QJsonObject
                {
                    { ReportDataName::ekey(ReportDataName::fhh), fhh },
                    { ReportDataName::ekey(ReportDataName::edh), edh },
                    { ReportDataName::ekey(ReportDataName::ltsh), ltsh },
                    { ReportDataName::ekey(ReportDataName::lthms), lthms },
                    { ReportDataName::ekey(ReportDataName::ptm), ptm },
                    { ReportDataName::ekey(ReportDataName::al), al }
                });
}

void ReportDataJson::appendPosition(int sbp, int dbp, int cvp, int lap, int pos)
{
    if (0 != pos) {
        data.insert(QString::number(Type::SBP), sbp);
        data.insert(QString::number(Type::DBP), dbp);
        data.insert(QString::number(Type::CVP), cvp);
        data.insert(QString::number(Type::LAP), lap);
        data.insert(QString::number(Type::Pos), pos);
    }
    auto position = getPosition();
    position.append(QJsonObject
                    {
                        { ReportDataName::ekey(ReportDataName::reportTime), (0 != pos ? time() : "") },
                        { ReportDataName::ekey(ReportDataName::data), data },
                        { ReportDataName::ekey(ReportDataName::allData), allData },
                        { ReportDataName::ekey(ReportDataName::waveform), waveform }
                    });
    json.insert(ReportDataName::ekey(ReportDataName::position), position);
    allData = QJsonArray();
    waveform = QJsonArray();
}

void ReportDataJson::setReportConclusion(const QString &reportConclusion)
{
    json.insert(ReportDataName::ekey(ReportDataName::reportConclusion), reportConclusion);
}

void ReportDataJson::appendData(uchar type, short value)
{
    if (Type::BEEP != type) {
        data.insert(QString::number(type), value);
        allData.append(QJsonObject{{QString::number(type), value}});
    }
}

void ReportDataJson::appendWaveformData(const QByteArray &data)
{
    if (waveform.size() >= 500) waveform.pop_front();
    waveform.append(QString(data.toHex()));
}

QJsonArray ReportDataJson::getPosition()
{
    return json.value(ReportDataName::ekey(ReportDataName::position)).toArray();
}

QString ReportDataJson::time() const
{
    return QDateTime::currentDateTime().toString("yyyyMMddhhmmsszzz");
}

