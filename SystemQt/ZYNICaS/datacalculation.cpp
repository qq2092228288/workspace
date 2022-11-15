#include "datacalculation.h"


qreal DataCalculation::cHr(const qreal &value)
{
    return value;
}

qreal DataCalculation::cVet(const qreal &value)
{
    return value;
}

qreal DataCalculation::cPep(const qreal &value)
{
    return value;
}

qreal DataCalculation::cStr(const qreal &pep, const qreal &vet)
{
    if (0 == vet)
        return 0;
    return pep/vet;
}

qreal DataCalculation::cTfc(const qreal &value)
{
    return value/1000.0;
}

qreal DataCalculation::cEpci(const qreal &value)
{
    return value/1000.0;
}

qreal DataCalculation::cIsi(const qreal &value)
{
    return value/100.0;
}

qreal DataCalculation::cIno(const qreal &isi, const int &sex, const int &age)
{
    return percent(isi, age < 70 ? (sex == 1 ? 1.35 : 1.15) : (sex == 1 ? 1.20 : 1.10));
}

qreal DataCalculation::cEf(const qreal &value)
{
    return value;
}

qreal DataCalculation::cSi(const qreal &value, const qreal &bsa, const qreal &vept)
{
    return 2/bsa*vept/6800*value;
}

qreal DataCalculation::cSv(const qreal &si, const qreal &bsa)
{
    return si*bsa;
}

qreal DataCalculation::cEdi(const qreal &si, const qreal &ef)
{
    if (0 == ef)
        return 0;
    return si/ef*100;
}

qreal DataCalculation::cLsw(const qreal &sv, const qreal &map, const qreal &lap)
{
    return 0.0144*sv*(map - lap);
}

qreal DataCalculation::cLswi(const qreal &si, const qreal &map, const qreal &lap)
{
    return 0.0144*si*(map - lap);
}

qreal DataCalculation::cVol(const qreal &lswi, const qreal &ino)
{
    return (percent(lswi, 52.8) - ino);
}

qreal DataCalculation::cSsvr(const qreal &sv, const qreal &map, const qreal &cvp)
{
    if (0 == sv)
        return 0;
    return 80*(map - cvp)/sv;
}

qreal DataCalculation::cSsvri(const qreal &si, const qreal &map, const qreal &cvp)
{
    if (0 == si)
        return 0;
    return 80*(map - cvp)/si;
}

qreal DataCalculation::cVas(const qreal &ssvri)
{
    return percent(ssvri, 137.8);
}

qreal DataCalculation::cCi(const qreal &value, const qreal &bsa, const qreal &vept)
{
    return 2/bsa*vept/6800*(value/10.0);
}

qreal DataCalculation::cCo(const qreal &ci, const qreal &bsa)
{
    return ci*bsa;
}

qreal DataCalculation::cHrv(const qreal &ci)
{
    return percent(ci, 4.5);
}

qreal DataCalculation::cSvr(const qreal &co, const qreal &map, const qreal &cvp)
{
    if (0 == co)
        return 0;
    return (map - cvp)/co*80;
}

qreal DataCalculation::cSvri(const qreal &ci, const qreal &map, const qreal &lap)
{
    if (0 == ci)
        return 0;
    return 80*(map - lap)/ci;
}

qreal DataCalculation::cLcw(const qreal &co, const qreal &map, const qreal &lap)
{
    return 0.0144*(map - lap - 2)*co;
}

qreal DataCalculation::cLcwi(const qreal &ci, const qreal &map, const qreal &lap)
{
    return 0.0144*(map - lap)*ci;
}

qreal DataCalculation::cRr(const qreal &value)
{
    return value/10;
}

qreal DataCalculation::percent(const qreal &value, const qreal &ideal)
{
    if (value >= ideal)
        return (value/ideal - 1)*100;
    return (1 - ideal/value)*100;
}
