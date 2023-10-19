#ifndef DATATYPE_H
#define DATATYPE_H

#include <QString>

enum Type {
    HR = 0,
    VET = 1,
    PEP = 2,
    TFC = 3,
    EPCI = 4,
    ISI = 5,
    EF = 6,
    SI = 7,
    CI = 8,
    RR = 9,
    BEEP = 10,
    Reserved = 15,
    CO = 16,
    SV = 17,
    HRV = 18,
    EDI = 19,
    Vol = 20,
    SVR = 21,
    SSVR = 22,
    SSVRI = 23,
    SVRI = 24,
    Vas = 25,
    LVET = 26,
    LSW = 27,
    LSWI = 28,
    LCW = 29,
    LCWI = 30,
    STR = 31,
    Ino = 32,
    SBP = 33,
    DBP = 34,
    MAP = 35,
    CVP = 36,
    LAP = 37,
    Dz = 38,
    Pos = 39,
    DO2 = 40,
    SVV = 41
};

QString typeName(const uchar &type);

QString typeName(const Type &type);

#endif // DATATYPE_H
