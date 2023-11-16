#include "reportdatajson.h"
#include <chrono>
#include <sstream>
#include <iomanip>

using std::chrono::system_clock;

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

ReportDataJson::ReportDataJson(const std::string &mac_)
    : mac(mac_)
{

}

Json::Value ReportDataJson::getJson()
{
    return json;
}

void ReportDataJson::clear()
{
    json.clear();
    data.clear();
    tempData.clear();
    allData.clear();
    waveform.clear();
    rtime.clear();
}

void ReportDataJson::setPatientInfo(const std::string &name, const std::string &id,
                                    int gender, int age, int height, int weight)
{
    clear();

    json["startTime"] = currentTime();

    Json::Value patientInfo;
    patientInfo["age"] = std::to_string(age);
    patientInfo["height"] = std::to_string(height);
    patientInfo["weight"] = std::to_string(weight);
    patientInfo["sex"] = (0 == gender ? std::string("男") : std::string("女"));
    patientInfo["patientName"] = name;
    patientInfo["medicalRecordNumber"] = id;

    json["patientInfo"] = patientInfo;

    Json::Value place;
    place["inspector"] = "";
    place["primaryPlace"] = "";
    place["secondaryPlace"] = "";
    place["mac"] = mac;
    place["consultationPlace"] = "";

    json["place"] = place;

    json["reportConclusion"] = "";
}

void ReportDataJson::appendPosition(int sbp, int dbp, int pos)
{
    data[std::to_string(Type::SBP)] = sbp;
    data[std::to_string(Type::DBP)] = dbp;
    data[std::to_string(Type::CVP)] = 4;
    data[std::to_string(Type::LAP)] = 9;
    data[std::to_string(Type::Pos)] = pos;
    if (!dataIntegrity()) {
        return;
    }

    Json::Value position = positionArray();
    if (position.size() == 2) {
        return;
    }

    Json::Value temp;
    temp["reportTime"] = currentTime();
    temp["data"] = data;
    temp["allData"] = allData;
    temp["waveform"] = waveform;

    position.append(temp);

    json["position"] = position;

    allData.clear();
    waveform.clear();
}

void ReportDataJson::appendDefault()
{
    appendPosition(90, 60, 1);
    json["tip"] = std::string("Default values for blood pressure and posture.");
}

void ReportDataJson::appendData(ushort type, ushort value)
{
    if (Type::BEEP != type) {
        data[std::to_string(type)] = value;
        tempData[std::to_string(type)] = value;
        if (10 == tempData.size()) {
            allData.append(tempData);
            tempData.clear();
        }
    }
}

void ReportDataJson::appendWaveformData(const std::string &data)
{
    if (waveform.size() >= 500) {
        Json::Value removed;
        waveform.removeIndex(0, &removed);
    }
    waveform.append(data);
}

bool ReportDataJson::dataIntegrity(bool all) const
{
    if (all)
        return (data.size() >= 15);
    return (data.size() >= 10);
}

Json::Value ReportDataJson::positionArray()
{
    return json["position"];
}

std::string ReportDataJson::reportTime()
{
    return rtime;
}

std::string ReportDataJson::currentTime()
{
    auto tp = system_clock::now();
    auto raw_time = system_clock::to_time_t(tp);
    auto timeinfo = std::localtime(&raw_time);
    std::stringstream ss;
    ss<<std::put_time(timeinfo, "%Y%m%d%H%M%S");
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch());
    auto milliseconds_str = std::to_string(ms.count() % 1000);
    if (milliseconds_str.length() < 3) {
        milliseconds_str = std::string(3 - milliseconds_str.length(), '0') + milliseconds_str;
    }
    std::stringstream rtime_ss;
    rtime_ss<<std::put_time(timeinfo, "%Y-%m-%d %H:%M:%S.");
    rtime = rtime_ss.str() + milliseconds_str;
    return ss.str() + milliseconds_str;
}


