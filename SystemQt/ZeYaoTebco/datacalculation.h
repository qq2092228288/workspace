#ifndef DATACALCULATION_H
#define DATACALCULATION_H

#include <math.h>
#include <algorithm>
#include <numeric>

class DataCalculation
{
public:
    static double invalid();
    // device data
    static double cHr     (const double &value);
    static double cVet    (const double &value);
    static double cPep    (const double &value);
    static double cStr    (const double &pep, const double &vet);
    static double cTfc    (const double &value);
    static double cEpci   (const double &value);
    static double cIsi    (const double &value);
    static double cIno    (const double &isi, const int &sex, const int &age);
    static double cEf     (const double &value);
    static double cSi     (const double &value, const double &bsa, const double &vept);
    static double cSv     (const double &si, const double &bsa);
    static double cEdi    (const double &si, const double &ef);
    static double cLsw    (const double &sv, const double &map, const double &lap);
    static double cLswi   (const double &si, const double &map, const double &lap);
    static double cVol    (const double &lswi, const double &ino);
    static double cSsvr   (const double &sv, const double &map, const double &cvp);
    static double cSsvri  (const double &si, const double &map, const double &cvp);
    static double cVas    (const double &ssvri);
    static double cCi     (const double &value, const double &bsa, const double &vept);
    static double cCo     (const double &ci, const double &bsa);
    static double cHrv    (const double &ci);
    static double cSvr    (const double &co, const double &map, const double &cvp);
    static double cSvri   (const double &ci, const double &map, const double &cvp);
    static double cLcw    (const double &co, const double &map, const double &lap);
    static double cLcwi   (const double &ci, const double &map, const double &lap);
    static double cDo2    (const double &co, const double &hb);
    static double cRr     (const double &value);
    // body data
    static double cBsa    (const double &height, const double &weight);
    static double cVept   (const double &height, const double &weight, const double &sex);
    static double cIdealW (const double &height, const double &sex);
    static double cMap    (const double &sbp, const double &dbp);
private:
    static double percent (const double &value, const double &ideal);
    static double perPNJ  (const double &value1, const double &value2);
    static double checkValue(const double &value);
    static bool  isInvalid(const double &value);
    static bool  isInvalid(const double &value1, const double &value2);
};
typedef DataCalculation DatCa;

#endif // DATACALCULATION_H
