#include "reportdatajson.h"
#include <QDateTime>
#include <qmath.h>
#include "datamanagement.h"
#include "datacalculation.h"
#include "reportset.h"


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

    auto dMap = ReportParameters::digitMap();
    // 计算值
    auto bsa = intercept(DatCa::cBsa(height, weight), 2);
    auto vept = intercept(DatCa::cVept(height, weight, sex), 0);
    auto map = intercept(DatCa::cMap(sbp, dbp), dMap.value(Type::MAP));

    QMap<Type, qreal> vMap;
    vMap.insert(Type::Pos, data.value(QString::number(Type::Pos)).toInt());
    vMap.insert(Type::SBP, sbp);
    vMap.insert(Type::DBP, dbp);
    vMap.insert(Type::LAP, lap);
    vMap.insert(Type::CVP, cvp);
    vMap.insert(Type::MAP, map);

    auto lvet = intercept(DatCa::cVet(data.value(QString::number(Type::VET)).toInt()), dMap.value(Type::LVET));
    auto pep = intercept(DatCa::cPep(data.value(QString::number(Type::PEP)).toInt()), dMap.value(Type::PEP));
    auto isi = intercept(DatCa::cIsi(data.value(QString::number(Type::ISI)).toInt()), dMap.value(Type::ISI));
    auto ef = DatCa::cEf(data.value(QString::number(Type::EF)).toInt());
    auto ino = intercept(DatCa::cIno(isi, sex, age), dMap.value(Type::Ino));
    auto si = intercept(DatCa::cSi(data.value(QString::number(Type::SI)).toInt(), bsa, vept), dMap.value(Type::SI));
    auto sv = intercept(DatCa::cSv(si, bsa), dMap.value(Type::SV));
    auto lswi = intercept(DatCa::cLswi(si, map, lap), dMap.value(Type::LSWI));
    auto ssvri = intercept(DatCa::cSsvri(si, map, cvp), dMap.value(Type::SSVRI));
    auto ci = intercept(DatCa::cCi(data.value(QString::number(Type::CI)).toInt(), bsa, vept), dMap.value(Type::CI));
    auto co = intercept(DatCa::cCo(ci, bsa), dMap.value(Type::CO));
    QVector<qreal> svs;
    auto osis = allValue(Type::SI, alldata);
    if (osis.size() > 25) {
        osis = osis.mid(osis.size() - 25);
    }
    foreach (auto value, osis) {
        svs<<(vept * value / 3400);
    }

    vMap.insert(Type::HR, intercept(DatCa::cHr(data.value(QString::number(Type::HR)).toInt()), dMap.value(Type::HR)));
    vMap.insert(Type::LVET, lvet);
    vMap.insert(Type::PEP, pep);
    vMap.insert(Type::STR, intercept(DatCa::cStr(pep, lvet), dMap.value(Type::STR)));
    vMap.insert(Type::TFC, intercept(DatCa::cTfc(data.value(QString::number(Type::TFC)).toInt()), dMap.value(Type::TFC)));
    vMap.insert(Type::EPCI, intercept(DatCa::cEpci(data.value(QString::number(Type::EPCI)).toInt()), dMap.value(Type::EPCI)));
    vMap.insert(Type::ISI, isi);
    vMap.insert(Type::Ino, ino);
    vMap.insert(Type::SI, si);
    vMap.insert(Type::SV, sv);
    vMap.insert(Type::EDI, intercept(DatCa::cEdi(si, ef), dMap.value(Type::EDI)));
    vMap.insert(Type::LSW, intercept(DatCa::cLsw(sv, map, lap), dMap.value(Type::LSW)));
    vMap.insert(Type::LSWI, lswi);
    vMap.insert(Type::Vol, intercept(DatCa::cVol(lswi, ino), dMap.value(Type::Vol)));
    vMap.insert(Type::SSVR, intercept(DatCa::cSsvr(sv, map, cvp), dMap.value(Type::SSVR)));
    vMap.insert(Type::SSVRI, ssvri);
    vMap.insert(Type::Vas, intercept(DatCa::cVas(ssvri), dMap.value(Type::Vas)));
    vMap.insert(Type::SVV, intercept(DatCa::cSvv(sv, svs), dMap.value(Type::SVV)));
    vMap.insert(Type::CI, ci);
    vMap.insert(Type::CO, co);
    vMap.insert(Type::HRV, intercept(DatCa::cHrv(ci), dMap.value(Type::HRV)));
    vMap.insert(Type::SVR, intercept(DatCa::cSvr(co, map, cvp), dMap.value(Type::SVR)));
    vMap.insert(Type::SVRI, intercept(DatCa::cSvri(ci, map, cvp), dMap.value(Type::SVRI)));
    vMap.insert(Type::LCW, intercept(DatCa::cLcw(co, map, lap), dMap.value(Type::LCW)));
    vMap.insert(Type::LCWI, intercept(DatCa::cLcwi(ci, map, lap), dMap.value(Type::LCWI)));
    vMap.insert(Type::DO2, intercept(DatCa::cDo2(co, hb), dMap.value(Type::DO2)));
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

qreal ReportDataJson::intercept(qreal value, int digit)
{
    return DatCa::invalid() != value ? (static_cast<int>(value * qPow(10, digit)))/qPow(10, digit) : value;
}
