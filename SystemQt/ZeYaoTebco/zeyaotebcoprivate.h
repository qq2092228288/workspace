#ifndef ZEYAOTEBCOPRIVATE_H
#define ZEYAOTEBCOPRIVATE_H

#include <iostream>
#include <cstring>

#include "mqtt/async_client.h"
#include "CSerialPort/SerialPort.h"
#include "json/value.h"
#include "json/reader.h"
#include "datacalculation.h"
#include "reportdatajson.h"

const int	QOS = 2;

class action_listener : public virtual mqtt::iaction_listener
{
    std::string name_;

    void on_failure(const mqtt::token& tok) override {

    }

    void on_success(const mqtt::token& tok) override {
    }

public:
    action_listener(const std::string& name) : name_(name) {}
};

class callback : public virtual mqtt::callback,
                 public virtual mqtt::iaction_listener

{
    mqtt::async_client& client_;

    mqtt::connect_options& connOpts_;

    action_listener subListener_;

    std::string subTopic_;

    Json::Value &deviceInfo_;

    void reconnect() {
        std::this_thread::sleep_for(std::chrono::milliseconds(2500));
        try {
            client_.connect(connOpts_, nullptr, *this);
        }
        catch (const mqtt::exception& exc) {
        }
    }

    void on_failure(const mqtt::token& tok) override {
        reconnect();
    }

    void on_success(const mqtt::token& tok) override {}

    void connected(const std::string& cause) override {
        client_.subscribe(subTopic_, QOS, nullptr, subListener_);
    }

    void connection_lost(const std::string& cause) override {
        reconnect();
    }

    void message_arrived(mqtt::const_message_ptr msg) override {
        Json::Reader reader;
        reader.parse(msg->to_string(), deviceInfo_);
    }

    void delivery_complete(mqtt::delivery_token_ptr token) override {}

public:
    callback(mqtt::async_client& client, mqtt::connect_options& connOpts,
             const std::string& subTopic, Json::Value &deviceInfo)
        : client_(client), connOpts_(connOpts), subListener_("Subscription"),
        subTopic_(subTopic), deviceInfo_(deviceInfo){}
};

static double intercept(double value, int digit)
{
    return DatCa::invalid() != value ?
               (static_cast<int>(value * std::pow(10, digit))) / std::pow(10, digit) : value;
}

struct BaseData
{
    BaseData(std::string _name = std::string(), std::string _id = std::string(),
             int _gender = 0, int _age = 0, int _height = 0, int _weight = 0)
        : name(_name),id(_id),gender(_gender),age(_age),height(_height),weight(_weight)
    {
        bsa = intercept(DatCa::cBsa(height, weight), 2);
        vept = intercept(DatCa::cVept(height, weight, gender), 0);
    }
    std::string name;
    std::string id;
    int gender;
    int age;
    int height;
    int weight;

    double bsa;
    double vept;

};

struct Value
{
    ushort data : 12;
    ushort type : 4;
};

class ZeYaoTebcoPrivate : public itas109::CSerialPortListener
{
public:
    ZeYaoTebcoPrivate();
    ~ZeYaoTebcoPrivate();
    enum Error
    {
        NoError = 0,
        InputError = 1,
        ServerConnectionFailed = 2,
        SerialPortNotOpened = 3,
        Detecting = 4,
        InsufficientQuantity = 5,
        NotDetected = 6,
        NoDataDetected = 7
    };
public:
    int login(std::string deviceId, std::string password);
    bool openSerialPort(std::string portName);
    int start(const std::string &name, const std::string &id,
              int gender, int age, int height, int weight);
    int appendBpAndPosition(int sbp, int dbp, int position);
    int end(int sbp, int dbp, int postiion);
    void closeSerialPort();
    std::string readAll();
    bool serverIsConnected();
    bool serialPortIsOpen();
    bool isDetecting();
    int availableQuantity();
    std::string availablePorts();
    std::string serialPortErrorMsg();
private:
    void onReadEvent(const char *portName, unsigned int readBufferLen);
    void publishReportData();
    void serialPortReconnect(itas109::CSerialPort *serialPort);
    bool dataIntegrity() const;
    std::string toString(const double &d);
private:
    static int init_hotplug_sock();
    static std::string login_publish_topic();
    static std::string upload_data_publish_topic();
    static std::string subscribe_topic();
    static std::string server_address();
    static std::string mac_address();
private:
    mqtt::connect_options m_connOpts;
    callback m_cb;
    mqtt::async_client m_client;
    itas109::CSerialPort *m_pSerialPort;
    bool m_checking;
    Json::Value m_jsonObject;
    Json::Value m_deviceInfo;
    std::string m_deviceId;
    BaseData m_data;
    ReportDataJson m_reportData;
};

#endif // ZEYAOTEBCOPRIVATE_H
