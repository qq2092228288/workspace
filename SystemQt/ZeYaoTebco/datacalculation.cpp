#include "datacalculation.h"

double DataCalculation::invalid()
{
    return 0xFFFFFFFF;
}

double DataCalculation::cHr(const double &value)
{
    return checkValue(value);
}

double DataCalculation::cVet(const double &value)
{
    return checkValue(value);
}

double DataCalculation::cPep(const double &value)
{
    return checkValue(value);
}

double DataCalculation::cStr(const double &pep, const double &vet)
{
    return (isInvalid(pep, vet) ? invalid() : pep/vet);
}

double DataCalculation::cTfc(const double &value)
{
    return checkValue(value/1000.0);
}

double DataCalculation::cEpci(const double &value)
{
    return checkValue(value/1000.0);
}

double DataCalculation::cIsi(const double &value)
{
    return checkValue(value/100.0);
}

double DataCalculation::cIno(const double &isi, const int &sex, const int &age)
{
    return percent(isi, age < 70 ? (sex == 1 ? 1.35 : 1.15) : (sex == 1 ? 1.20 : 1.10));
}

double DataCalculation::cEf(const double &value)
{
    return checkValue(value);
}

double DataCalculation::cSi(const double &value, const double &bsa, const double &vept)
{
    return (isInvalid(value, bsa) ? invalid() : 2/bsa*vept/6800*value);
}

double DataCalculation::cSv(const double &si, const double &bsa)
{
    return (isInvalid(si, bsa) ? invalid() : si*bsa);
}

double DataCalculation::cEdi(const double &si, const double &ef)
{
    return (isInvalid(si, ef) ? invalid() : si/ef*100);
}

double DataCalculation::cLsw(const double &sv, const double &map, const double &lap)
{
    return (isInvalid(sv, map) ? invalid() : 0.0144*sv*(map - lap));
}

double DataCalculation::cLswi(const double &si, const double &map, const double &lap)
{
    return (isInvalid(si, map) ? invalid() : 0.0144*si*(map - lap));
}

double DataCalculation::cVol(const double &lswi, const double &ino)
{
    return (isInvalid(lswi) || invalid() == ino ? invalid() : percent(lswi, 52.8) - ino);
}

double DataCalculation::cSsvr(const double &sv, const double &map, const double &cvp)
{
    return (isInvalid(sv, map) ? invalid() : 80*(map - cvp)/sv);
}

double DataCalculation::cSsvri(const double &si, const double &map, const double &cvp)
{
    return (isInvalid(si, map) ? invalid() : 80*(map - cvp)/si);
}

double DataCalculation::cVas(const double &ssvri)
{
    return (isInvalid(ssvri) ? invalid() : percent(ssvri, 137.8));
}

double DataCalculation::cCi(const double &value, const double &bsa, const double &vept)
{
    return (isInvalid(value, bsa) ? invalid() : 2/bsa*vept/6800*(value/10.0));
}

double DataCalculation::cCo(const double &ci, const double &bsa)
{
    return (isInvalid(ci, bsa) ? invalid() : ci*bsa);
}

double DataCalculation::cHrv(const double &ci)
{
    return percent(ci, 4.5);
}

double DataCalculation::cSvr(const double &co, const double &map, const double &cvp)
{
    return (isInvalid(co, map) ? invalid() : (map - cvp)/co*80);
}

double DataCalculation::cSvri(const double &ci, const double &map, const double &cvp)
{
    return (isInvalid(ci, map) ? invalid() : 80*(map - cvp)/ci);
}

double DataCalculation::cLcw(const double &co, const double &map, const double &lap)
{
    return (isInvalid(co, map) ? invalid() : 0.0144*(map - lap - 2)*co);
}

double DataCalculation::cLcwi(const double &ci, const double &map, const double &lap)
{
    return (isInvalid(ci, map) ? invalid() : 0.0144*(map - lap)*ci);
}

double DataCalculation::cDo2(const double &co, const double &hb)
{
    return (isInvalid(co, hb) ? invalid() : 1.34*hb*co*10);
}

double DataCalculation::cRr(const double &value)
{
    return checkValue(value/10);
}

double DataCalculation::cBsa(const double &height, const double &weight)
{
    return (std::pow(weight, 0.425)*std::pow(height, 0.725)*0.007184);
}

double DataCalculation::cVept(const double &height, const double &weight, const double &sex)
{
    //    double bmi = weight/qPow(height/100.0, 2);
    //    double increase = (bmi <= 23.9 ? 1.1 : 1.1 + (bmi - 23.9)*0.1625);
    //    return ((qPow(0.17*height, 3)/4.25)*(1 + 0.65*(weight/(0.524*height - (sex == 0 ? 16.58 : 26.58)) - 1))*increase);
    return ((std::pow(0.17*height, 3)/4.25)*(1 + 0.65*(weight/(0.524*height - (sex == 0 ? 16.58 : 26.58)) - 1)));
}

double DataCalculation::cIdealW(const double &height, const double &sex)
{
    return (0.524*height - (sex == 0 ? 16.58 : 26.58));
}

double DataCalculation::cMap(const double &sbp, const double &dbp)
{
    return ((sbp + dbp*2)/3);
}

double DataCalculation::percent(const double &value, const double &ideal)
{
    return (isInvalid(value) ? invalid() : perPNJ(value, ideal));
}

double DataCalculation::perPNJ(const double &value1, const double &value2)
{
    return (value1 >= value2 ? (value1/value2 - 1)*100 : (1 - value2/value1)*100);
}

double DataCalculation::checkValue(const double &value)
{
    return (0 != value ? value : invalid());
}

bool DataCalculation::isInvalid(const double &value)
{
    return ((0 == value || invalid() == value) ? true : false);
}

bool DataCalculation::isInvalid(const double &value1, const double &value2)
{
    return (isInvalid(value1) || isInvalid(value2));
}
