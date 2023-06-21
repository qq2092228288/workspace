#include "tebcoparamcalculation.h"
#include <qmath.h>

const qreal TebcoParamCalculation::invalid()
{
    return 0xFFFFFFFF;
}

qreal TebcoParamCalculation::cHr(const qreal &value)
{
    return checkValue(value);
}

qreal TebcoParamCalculation::cVet(const qreal &value)
{
    return checkValue(value);
}

qreal TebcoParamCalculation::cPep(const qreal &value)
{
    return checkValue(value);
}

qreal TebcoParamCalculation::cStr(const qreal &pep, const qreal &vet)
{
    return (isInvalid(pep, vet) ? invalid() : pep/vet);
}

qreal TebcoParamCalculation::cTfc(const qreal &value)
{
    return checkValue(value/1000.0);
}

qreal TebcoParamCalculation::cEpci(const qreal &value)
{
    return checkValue(value/1000.0);
}

qreal TebcoParamCalculation::cIsi(const qreal &value)
{
    return checkValue(value/100.0);
}

qreal TebcoParamCalculation::cIno(const qreal &isi, const int &sex, const int &age)
{
    return percent(isi, age < 70 ? (sex == 1 ? 1.35 : 1.15) : (sex == 1 ? 1.20 : 1.10));
}

qreal TebcoParamCalculation::cEf(const qreal &value)
{
    return checkValue(value);
}

qreal TebcoParamCalculation::cSi(const qreal &value, const qreal &bsa, const qreal &vept)
{
    return (isInvalid(value, bsa) ? invalid() : 2/bsa*vept/6800*value);
}

qreal TebcoParamCalculation::cSv(const qreal &si, const qreal &bsa)
{
    return (isInvalid(si, bsa) ? invalid() : si*bsa);
}

qreal TebcoParamCalculation::cEdi(const qreal &si, const qreal &ef)
{
    return (isInvalid(si, ef) ? invalid() : si/ef*100);
}

qreal TebcoParamCalculation::cLsw(const qreal &sv, const qreal &map, const qreal &lap)
{
    return (isInvalid(sv, map) ? invalid() : 0.0144*sv*(map - lap));
}

qreal TebcoParamCalculation::cLswi(const qreal &si, const qreal &map, const qreal &lap)
{
    return (isInvalid(si, map) ? invalid() : 0.0144*si*(map - lap));
}

qreal TebcoParamCalculation::cVol(const qreal &lswi, const qreal &ino)
{
    return (isInvalid(lswi) || invalid() == ino ? invalid() : percent(lswi, 52.8) - ino);
}

qreal TebcoParamCalculation::cSsvr(const qreal &sv, const qreal &map, const qreal &cvp)
{
    return (isInvalid(sv, map) ? invalid() : 80*(map - cvp)/sv);
}

qreal TebcoParamCalculation::cSsvri(const qreal &si, const qreal &map, const qreal &cvp)
{
    return (isInvalid(si, map) ? invalid() : 80*(map - cvp)/si);
}

qreal TebcoParamCalculation::cVas(const qreal &ssvri)
{
    return (isInvalid(ssvri) ? invalid() : percent(ssvri, 137.8));
}

qreal TebcoParamCalculation::cSvv(const qreal &sv, QList<qreal> svList)
{
    if (svList.size() < 6 || isInvalid(sv)) return invalid();
    svList.removeOne(*std::min_element(svList.begin(), svList.end()));
    svList.removeOne(*std::max_element(svList.begin(), svList.end()));
    qreal svAvg = std::accumulate(svList.begin(), svList.end(), 0.0)/svList.size();
    return qAbs(sv - svAvg)/svAvg;
}

qreal TebcoParamCalculation::cCi(const qreal &value, const qreal &bsa, const qreal &vept)
{
    return (isInvalid(value, bsa) ? invalid() : 2/bsa*vept/6800*(value/10.0));
}

qreal TebcoParamCalculation::cCo(const qreal &ci, const qreal &bsa)
{
    return (isInvalid(ci, bsa) ? invalid() : ci*bsa);
}

qreal TebcoParamCalculation::cHrv(const qreal &ci)
{
    return percent(ci, 4.5);
}

qreal TebcoParamCalculation::cSvr(const qreal &co, const qreal &map, const qreal &cvp)
{
    return (isInvalid(co, map) ? invalid() : (map - cvp)/co*80);
}

qreal TebcoParamCalculation::cSvri(const qreal &ci, const qreal &map, const qreal &cvp)
{
    return (isInvalid(ci, map) ? invalid() : 80*(map - cvp)/ci);
}

qreal TebcoParamCalculation::cLcw(const qreal &co, const qreal &map, const qreal &lap)
{
    return (isInvalid(co, map) ? invalid() : 0.0144*(map - lap - 2)*co);
}

qreal TebcoParamCalculation::cLcwi(const qreal &ci, const qreal &map, const qreal &lap)
{
    return (isInvalid(ci, map) ? invalid() : 0.0144*(map - lap)*ci);
}

qreal TebcoParamCalculation::cDo2(const qreal &co, const qreal &hb)
{
    return (isInvalid(co) ? invalid() : 1.34*hb*co*10);
}

qreal TebcoParamCalculation::cRr(const qreal &value)
{
    return checkValue(value/10);
}

qreal TebcoParamCalculation::cBsa(const qreal &height, const qreal &weight)
{
    return (qPow(weight, 0.425)*qPow(height, 0.725)*0.007184);
}

qreal TebcoParamCalculation::cVept(const qreal &height, const qreal &weight, const qreal &sex)
{
    return ((qPow(0.17*height, 3)/4.25)*(1 + 0.65*(weight/(0.524*height - (sex == 0 ? 16.58 : 26.58)) - 1)));
}

qreal TebcoParamCalculation::cIdealW(const qreal &height, const qreal &sex)
{
    return (0.524*height - (sex == 0 ? 16.58 : 26.58));
}

qreal TebcoParamCalculation::cMap(const qreal &sbp, const qreal &dbp)
{
    return ((sbp + dbp*2)/3);
}

qreal TebcoParamCalculation::cNnvgr(const QList<qreal> &hrList)
{
    if (hrList.count() == 0) return invalid();
    QList<qreal> rrList = getRrList(hrList);
    return (std::accumulate(rrList.begin(), rrList.end(), 0.0)/rrList.size());
}

qreal TebcoParamCalculation::cSdnn(const QList<qreal> &hrList)
{
    if (hrList.count() == 0) return invalid();
    QList<qreal> rrList = getRrList(hrList);
    double avg = std::accumulate(rrList.begin(), rrList.end(), 0.0)/rrList.size();
    double accum = 0.0;
    foreach (auto rr, rrList) {
        accum += qPow(rr - avg, 2);
    }
    return (qSqrt(accum/rrList.size()));
}

qreal TebcoParamCalculation::cPnn50(const QList<qreal> &hrList)
{
    QList<qreal> rrList = getRrList(hrList);
    int mT50ms = 0;
    for (int index = 1; index < rrList.size() - 1; ++index) {
        if (qAbs(rrList.at(index) - rrList.at(index - 1)) > 0.05) {
            ++mT50ms;
        }
    }
    return (mT50ms/rrList.size());
}

qreal TebcoParamCalculation::cRmssd(const QList<qreal> &hrList)
{
    QList<qreal> rrList = getRrList(hrList);
    if (rrList.size() < 2) return 0;
    double accum = 0.0;
    for (int index = 1; index < rrList.size() - 1; ++index) {
        accum += qPow(rrList.at(index) - rrList.at(index - 1), 2);
    }
    return (qSqrt(accum/(rrList.size() - 1)));
}

qreal TebcoParamCalculation::percent(const qreal &value, const qreal &ideal)
{
    return (isInvalid(value) ? invalid() : perPNJ(value, ideal));
}

qreal TebcoParamCalculation::perPNJ(const qreal &value1, const qreal &value2)
{
    return (value1 >= value2 ? (value1/value2 - 1)*100 : (1 - value2/value1)*100);
}

QList<qreal> TebcoParamCalculation::getRrList(const QList<qreal> &hrList)
{
    QList<qreal> rrList;
    foreach (auto hr, hrList) {
        rrList<<(60000.0/hr);
    }
    return rrList;
}

qreal TebcoParamCalculation::checkValue(const qreal &value)
{
    return (0 != value ? value : invalid());
}

bool TebcoParamCalculation::isInvalid(const qreal &value)
{
    return ((0 == value || invalid() == value) ? true : false);
}

bool TebcoParamCalculation::isInvalid(const qreal &value1, const qreal &value2)
{
    return (isInvalid(value1) || isInvalid(value2));
}
