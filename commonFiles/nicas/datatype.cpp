#include "datatype.h"

QString typeName(const uchar &type)
{
    switch (type) {
    case Type::HR:
        return "HR";
        break;
    case Type::VET:
        return "VET";
        break;
    case Type::PEP:
        return "PEP";
        break;
    case Type::TFC:
        return "TFC";
        break;
    case Type::EPCI:
        return "EPCI";
        break;
    case Type::ISI:
        return "ISI";
        break;
    case Type::EF:
        return "EF";
        break;
    case Type::SI:
        return "SI";
        break;
    case Type::CI:
        return "CI";
        break;
    case Type::RR:
        return "RR";
        break;
    case Type::BEEP:
        return "BEEP";
        break;
    case Type::Reserved:
        return "Reserved";
        break;
    case Type::CO:
        return "CO";
        break;
    case Type::SV:
        return "SV";
        break;
    case Type::HRV:
        return "HRV";
        break;
    case Type::EDI:
        return "EDI";
        break;
    case Type::Vol:
        return "Vol";
        break;
    case Type::SVR:
        return "SVR";
        break;
    case Type::SSVR:
        return "SSVR";
        break;
    case Type::SSVRI:
        return "SSVRI";
        break;
    case Type::SVRI:
        return "SVRI";
        break;
    case Type::Vas:
        return "Vas";
        break;
    case Type::LVET:
        return "LVET";
        break;
    case Type::LSW:
        return "LSW";
        break;
    case Type::LSWI:
        return "LSWI";
        break;
    case Type::LCW:
        return "LCW";
        break;
    case Type::LCWI:
        return "LCWI";
        break;
    case Type::STR:
        return "STR";
        break;
    case Type::Ino:
        return "Ino";
        break;
    case Type::SBP:
        return "SBP";
        break;
    case Type::DBP:
        return "DBP";
        break;
    case Type::MAP:
        return "MAP";
        break;
    case Type::CVP:
        return "CVP";
        break;
    case Type::LAP:
        return "LAP";
        break;
    case Type::Dz:
        return "Dz";
        break;
    case Type::DO2:
        return "DO2";
        break;
    case Type::SVV:
        return "SVV";
        break;
    default:
        break;
    }
    return nullptr;
}
QString typeName(const Type &type)
{
    return typeName((uchar)type);
}
