#ifndef REPORTDATAJSON_H
#define REPORTDATAJSON_H

#include <cstring>
#include <ctime>
#include "json/value.h"

class ReportDataJson
{
public:
    ReportDataJson(const std::string &mac_);
    Json::Value getJson();
    void clear();
    void setPatientInfo(const std::string &name, const std::string &id,
                        int gender, int age, int height, int weight);
    void appendPosition(int sbp, int dbp, int pos);
    void appendDefault();
    void appendData(ushort type, ushort value);
    void appendWaveformData(const std::string &data);
    bool dataIntegrity(bool all = true) const;
    Json::Value positionArray();
    std::string reportTime();
private:
    std::string currentTime();
private:
    std::string mac;
    Json::Value json;
    Json::Value data;
    Json::Value tempData;
    Json::Value allData;
    Json::Value waveform;
    std::string rtime;
};

#endif // REPORTDATAJSON_H
