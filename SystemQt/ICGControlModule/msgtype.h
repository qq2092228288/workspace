#ifndef MSGTYPE_H
#define MSGTYPE_H

#include <QObject>
#include <QDebug>

constexpr char invalid_value = char(0xfd);
constexpr int len_9 = 9;
constexpr int len_10 = 10;
constexpr double factor_1[len_9] = {1, 0.02, 50, 0.5, 0.1, 1, 16, 1, 10};
constexpr double factor_2[len_9] = {1, 1, 50, 0.5, 0.1, 1, 16, 1, 10};
constexpr double ext_factor[len_10] = {2, 1, 2, 2, 1, 0.01, 0.1, 0.1, 0.1, 1};

struct PatientData
{
    PatientData(const uchar &_height, const uchar &_weight, const bool &_gender,
                const uchar &_age, const bool &_pacemaker)
        : height(_height)
        , weight(_weight)
        , gender(_gender)
        , age(_age)
        , pacemaker(_pacemaker)
        , options(0x31)
        , mainsFrequency(0x00)
        , protocolVersion(0x02)
    {}
    uchar height;
    uchar weight;
    uchar gender;
    uchar age;
    uchar pacemaker;
    uchar options;
    uchar mainsFrequency;
    uchar protocolVersion;
};

struct BloodPressure
{
    BloodPressure(const uchar &_syst, const uchar &_diast)
        : syst(_syst)
        , diast(_diast)
        , mean(static_cast<uchar>((_syst + _diast * 2) / 3))
    {}
    uchar syst;
    uchar diast;
    uchar mean;
};

struct ExternalValues
{
    ExternalValues(const uchar &_cvp, const uchar &_pawp, const uchar &_hb, const uchar &_spO2)
        : cvp(_cvp)
        , pawp(_pawp)
        , hb(_hb)
        , spO2(_spO2)
    {}
    uchar cvp;
    uchar pawp;
    uchar hb;
    uchar spO2;
};

class MsgType
{
public:
    static int16_t hiLo(const char &byte0, const char &byte1)
    {
        return (byte0 << 7 | byte1);
    }
    static QVector<float> standard(const QByteArray &data)
    {
        QVector<float> vect;
        if (data.length() == len_10) {
            for (int i = 0; i < len_9; ++i) {
                if (i == 0) {
                    vect<<evaluation(data.at(i), factor_1[i]);
                }
                else if (i == 1) {
                    if (data.at(i) != invalid_value) {
                        vect<<(hiLo(data.at(i), data.at(i + 1)) * factor_1[i]);
                    }
                    else {
                        vect<<invalid_value;
                    }
                }
                else {
                    vect<<evaluation(data.at(i + 1), factor_1[i]);
                }
            }
        }
        else if (data.length() == len_9) {
            for (int i = 0; i < len_9; ++i) {
                vect<<evaluation(data.at(i), factor_2[i]);
            }
        }
        else {
            for (int i = 0; i < len_9; ++i) {
                vect<<invalid_value;
            }
        }
        return vect;
    }
    static QVector<float> extended(const QByteArray &data)
    {
        QVector<float> vect;
        if (data.length() == len_10) {
            for (int i = 0; i < len_10; ++i) {
                vect<<evaluation(data.at(i), ext_factor[i]);
            }
        }
        else {
            for (int i = 0; i < len_10; ++i) {
                vect<<invalid_value;
            }
        }
        return vect;
    }
private:
    static float evaluation(const char &value, const double &factor)
    {
        if (value != invalid_value) {
            return value * factor;
        }
        return invalid_value;
    }
};




#endif // MSGTYPE_H
