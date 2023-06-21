#ifndef TEBCOPARAMCALCULATION_H
#define TEBCOPARAMCALCULATION_H

#include <QtCore/qglobal.h>
#include <QObject>

#if defined(TEBCOPARAMCALCULATION_LIBRARY)
#  define TEBCOPARAMCALCULATION_EXPORT Q_DECL_EXPORT
#else
#  define TEBCOPARAMCALCULATION_EXPORT Q_DECL_IMPORT
#endif

class TEBCOPARAMCALCULATION_EXPORT TebcoParamCalculation
{
public:
    static const qreal invalid();
    // device data
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
    static qreal cSvv    (const qreal &sv, QList<qreal> svList);
    static qreal cCi     (const qreal &value, const qreal &bsa, const qreal &vept);
    static qreal cCo     (const qreal &ci, const qreal &bsa);
    static qreal cHrv    (const qreal &ci);
    static qreal cSvr    (const qreal &co, const qreal &map, const qreal &cvp);
    static qreal cSvri   (const qreal &ci, const qreal &map, const qreal &cvp);
    static qreal cLcw    (const qreal &co, const qreal &map, const qreal &lap);
    static qreal cLcwi   (const qreal &ci, const qreal &map, const qreal &lap);
    static qreal cDo2    (const qreal &co, const qreal &hb);
    static qreal cRr     (const qreal &value);
    // body data
    static qreal cBsa    (const qreal &height, const qreal &weight);
    static qreal cVept   (const qreal &height, const qreal &weight, const qreal &sex);
    static qreal cIdealW (const qreal &height, const qreal &sex);
    static qreal cMap    (const qreal &sbp, const qreal &dbp);
    // hrv analyse rr = 60/hr
    static qreal cNnvgr  (const QList<qreal> &hrList);
    static qreal cSdnn   (const QList<qreal> &hrList);
    static qreal cPnn50  (const QList<qreal> &hrList);
    static qreal cRmssd  (const QList<qreal> &hrList);
private:
    static qreal percent (const qreal &value, const qreal &ideal);
    static qreal perPNJ  (const qreal &value1, const qreal &value2);
    static QList<qreal> getRrList(const QList<qreal> &hrList);
    static qreal checkValue(const qreal &value);
    static bool  isInvalid(const qreal &value);
    static bool  isInvalid(const qreal &value1, const qreal &value2);
};
typedef TebcoParamCalculation DatCa;

#endif // TEBCOPARAMCALCULATION_H
