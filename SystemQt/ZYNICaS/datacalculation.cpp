#include "datacalculation.h"
#include <qmath.h>

const qreal DataCalculation::invalid()
{
    return 0xFFFFFFFF;
}

qreal DataCalculation::cHr(const qreal &value)
{
    return checkValue(value);
}

qreal DataCalculation::cVet(const qreal &value)
{
    return checkValue(value);
}

qreal DataCalculation::cPep(const qreal &value)
{
    return checkValue(value);
}

qreal DataCalculation::cStr(const qreal &pep, const qreal &vet)
{
    return (isInvalid(pep, vet) ? invalid() : pep/vet);
}

qreal DataCalculation::cTfc(const qreal &value)
{
    return checkValue(value/1000.0);
}

qreal DataCalculation::cEpci(const qreal &value)
{
    return checkValue(value/1000.0);
}

qreal DataCalculation::cIsi(const qreal &value)
{
    return checkValue(value/100.0);
}

qreal DataCalculation::cIno(const qreal &isi, const int &sex, const int &age)
{
    return percent(isi, age < 70 ? (sex == 1 ? 1.35 : 1.15) : (sex == 1 ? 1.20 : 1.10));
}

qreal DataCalculation::cEf(const qreal &value)
{
    return checkValue(value);
}

qreal DataCalculation::cSi(const qreal &value, const qreal &bsa, const qreal &vept)
{
    return (isInvalid(value, bsa) ? invalid() : 2/bsa*vept/6800*value);
}

qreal DataCalculation::cSv(const qreal &si, const qreal &bsa)
{
    return (isInvalid(si, bsa) ? invalid() : si*bsa);
}

qreal DataCalculation::cEdi(const qreal &si, const qreal &ef)
{
    return (isInvalid(si, ef) ? invalid() : si/ef*100);
}

qreal DataCalculation::cLsw(const qreal &sv, const qreal &map, const qreal &lap)
{
    return (isInvalid(sv, map) ? invalid() : 0.0144*sv*(map - lap));
}

qreal DataCalculation::cLswi(const qreal &si, const qreal &map, const qreal &lap)
{
    return (isInvalid(si, map) ? invalid() : 0.0144*si*(map - lap));
}

qreal DataCalculation::cVol(const qreal &lswi, const qreal &ino)
{
    return (isInvalid(lswi) || invalid() == ino ? invalid() : percent(lswi, 52.8) - ino);
}

qreal DataCalculation::cSsvr(const qreal &sv, const qreal &map, const qreal &cvp)
{
    return (isInvalid(sv, map) ? invalid() : 80*(map - cvp)/sv);
}

qreal DataCalculation::cSsvri(const qreal &si, const qreal &map, const qreal &cvp)
{
    return (isInvalid(si, map) ? invalid() : 80*(map - cvp)/si);
}

qreal DataCalculation::cVas(const qreal &ssvri)
{
    return (isInvalid(ssvri) ? invalid() : percent(ssvri, 137.8));
}

qreal DataCalculation::cSvv(const qreal &sv, QList<qreal> svList)
{
    if (svList.size() < 6 || isInvalid(sv)) return invalid();
    svList.removeOne(*std::min_element(svList.begin(), svList.end()));
    svList.removeOne(*std::max_element(svList.begin(), svList.end()));
    qreal svAvg = std::accumulate(svList.begin(), svList.end(), 0.0)/svList.size();
    return qAbs(sv - svAvg)/svAvg;
}

qreal DataCalculation::cCi(const qreal &value, const qreal &bsa, const qreal &vept)
{
    return (isInvalid(value, bsa) ? invalid() : 2/bsa*vept/6800*(value/10.0));
}

qreal DataCalculation::cCo(const qreal &ci, const qreal &bsa)
{
    return (isInvalid(ci, bsa) ? invalid() : ci*bsa);
}

qreal DataCalculation::cHrv(const qreal &ci)
{
    return percent(ci, 4.5);
}

qreal DataCalculation::cSvr(const qreal &co, const qreal &map, const qreal &cvp)
{
    return (isInvalid(co, map) ? invalid() : (map - cvp)/co*80);
}

qreal DataCalculation::cSvri(const qreal &ci, const qreal &map, const qreal &cvp)
{
    return (isInvalid(ci, map) ? invalid() : 80*(map - cvp)/ci);
}

qreal DataCalculation::cLcw(const qreal &co, const qreal &map, const qreal &lap)
{
    return (isInvalid(co, map) ? invalid() : 0.0144*(map - lap - 2)*co);
}

qreal DataCalculation::cLcwi(const qreal &ci, const qreal &map, const qreal &lap)
{
    return (isInvalid(ci, map) ? invalid() : 0.0144*(map - lap)*ci);
}

qreal DataCalculation::cDo2(const qreal &co, const qreal &hb)
{
    return (isInvalid(co) ? invalid() : 1.34*hb*co*10);
}

qreal DataCalculation::cRr(const qreal &value)
{
    return checkValue(value/10);
}

qreal DataCalculation::cBsa(const qreal &height, const qreal &weight)
{
    return (qPow(weight, 0.425)*qPow(height, 0.725)*0.007184);
}

qreal DataCalculation::cVept(const qreal &height, const qreal &weight, const qreal &sex)
{
//    qreal bmi = weight/qPow(height/100.0, 2);
//    qreal increase = (bmi <= 23.9 ? 1.1 : 1.1 + (bmi - 23.9)*0.1625);
//    return ((qPow(0.17*height, 3)/4.25)*(1 + 0.65*(weight/(0.524*height - (sex == 0 ? 16.58 : 26.58)) - 1))*increase);
    return ((qPow(0.17*height, 3)/4.25)*(1 + 0.65*(weight/(0.524*height - (sex == 0 ? 16.58 : 26.58)) - 1)));
}

qreal DataCalculation::cIdealW(const qreal &height, const qreal &sex)
{
    return (0.524*height - (sex == 0 ? 16.58 : 26.58));
}

qreal DataCalculation::cMap(const qreal &sbp, const qreal &dbp)
{
    return ((sbp + dbp*2)/3);
}

qreal DataCalculation::percent(const qreal &value, const qreal &ideal)
{
    return (isInvalid(value) ? invalid() : perPNJ(value, ideal));
}

qreal DataCalculation::perPNJ(const qreal &value1, const qreal &value2)
{
    return (value1 >= value2 ? (value1/value2 - 1)*100 : (1 - value2/value1)*100);
}

qreal DataCalculation::checkValue(const qreal &value)
{
    return (0 != value ? value : invalid());
}

bool DataCalculation::isInvalid(const qreal &value)
{
    return ((0 == value || invalid() == value) ? true : false);
}

bool DataCalculation::isInvalid(const qreal &value1, const qreal &value2)
{
    return (isInvalid(value1) || isInvalid(value2));
}
