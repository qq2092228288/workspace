#ifndef DATACALCULATION_H
#define DATACALCULATION_H

#include <QObject>


class DataCalculation
{
public:
    static qreal cHr     (const qreal &value);
    static qreal cVet    (const qreal &value);
    static qreal cPep    (const qreal &value);
    static qreal cStr    (const qreal &pep, const qreal &vet);
    static qreal cTfc    (const qreal &value);
    static qreal cEpci   (const qreal &value);
    static qreal cIsi    (const qreal &value);
    static qreal cIno    (const qreal &isi, const int &sex, const int &age);
    static qreal cEf     (const qreal &value);
    static qreal cSi     (const qreal &value, const qreal &bsa, const qreal &vept);
    static qreal cSv     (const qreal &si, const qreal &bsa);
    static qreal cEdi    (const qreal &si, const qreal &ef);
    static qreal cLsw    (const qreal &sv, const qreal &map, const qreal &lap);
    static qreal cLswi   (const qreal &si, const qreal &map, const qreal &lap);
    static qreal cVol    (const qreal &lswi, const qreal &ino);
    static qreal cSsvr   (const qreal &sv, const qreal &map, const qreal &cvp);
    static qreal cSsvri  (const qreal &si, const qreal &map, const qreal &cvp);
    static qreal cVas    (const qreal &ssvri);
    static qreal cCi     (const qreal &value, const qreal &bsa, const qreal &vept);
    static qreal cCo     (const qreal &ci, const qreal &bsa);
    static qreal cHrv    (const qreal &ci);
    static qreal cSvr    (const qreal &co, const qreal &map, const qreal &cvp);
    static qreal cSvri   (const qreal &ci, const qreal &map, const qreal &lap);
    static qreal cLcw    (const qreal &co, const qreal &map, const qreal &lap);
    static qreal cLcwi   (const qreal &ci, const qreal &map, const qreal &lap);
    static qreal cRr     (const qreal &value);
private:
    static qreal percent(const qreal &value, const qreal &ideal);
};
typedef DataCalculation DatCa;

#endif // DATACALCULATION_H
