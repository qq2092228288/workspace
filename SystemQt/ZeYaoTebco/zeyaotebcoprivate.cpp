#include "zeyaotebcoprivate.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/un.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#if _WIN32
#include <windows.h>
#elif __linux__
#include <linux/types.h>
#include <linux/netlink.h>
#elif __APPLE__
#include <mach-o/dyld.h>
#endif

#include "CSerialPort/SerialPortInfo.h"


ZeYaoTebcoPrivate::ZeYaoTebcoPrivate()
    : m_cb(m_client, m_connOpts, subscribe_topic(), m_deviceInfo)
    , m_client(server_address(), mac_address())
    , m_reportData(mac_address())
    , m_checking(false)
{
    m_pSerialPort = new itas109::CSerialPort();
    m_connOpts.set_clean_session(true);
    m_client.set_callback(m_cb);
    std::thread t(&ZeYaoTebcoPrivate::serialPortReconnect, this, m_pSerialPort);
    t.detach();
}

ZeYaoTebcoPrivate::~ZeYaoTebcoPrivate()
{
    closeSerialPort();
    m_pSerialPort->disconnectReadEvent();
    delete m_pSerialPort;
    m_pSerialPort = nullptr;
}

int ZeYaoTebcoPrivate::login(std::string deviceId, std::string password)
{
    try {
        if (!m_client.is_connected()) {
            auto token = m_client.connect(m_connOpts, nullptr, m_cb);
            token->wait();
        }
    }
    catch (...) {
        return Error::ServerConnectionFailed;
    }
    if (deviceId.empty() || password.empty() || deviceId.length() > 32 || password.length() > 32) {
        return Error::InputError;
    }
    if (!m_deviceInfo.empty()) {
        m_deviceInfo.clear();
    }
    m_deviceId = deviceId;
    Json::Value value;
    value["deviceid"] = deviceId;
    value["password"] = password;
    m_client.publish(login_publish_topic(), value.toStyledString(), QOS, false);
    return Error::NoError;
}

bool ZeYaoTebcoPrivate::openSerialPort(std::string portName)
{
    if (m_pSerialPort->isOpen()) {
        m_pSerialPort->close();
    }
    m_pSerialPort->init(portName.data());
    m_pSerialPort->open();
    return m_pSerialPort->isOpen();
}

int ZeYaoTebcoPrivate::start(const std::string &name, const std::string &id,
                             int gender, int age, int height, int weight)
{
    // 串口未打开
    if (!m_pSerialPort->isOpen())
        return Error::SerialPortNotOpened;
    // 有效验证码不足
    if (availableQuantity() <= 0) {
        return Error::InsufficientQuantity;
    }
    // 正在检测
    if (m_checking)
        return Error::Detecting;
    // 信息错误
    if (name.empty() || name.length() > 32 ||
        id.empty() || id.length() > 32 ||
        gender < 0 || gender > 1 ||
        age < 10 || age > 120 ||
        height < 50 || height > 250 ||
        weight < 20 || weight > 300)
        return Error::InputError;
    // 设置基本数据
    m_data = BaseData(name, id, gender, age, height, weight);
    // 报告
    m_reportData.setPatientInfo(name, id, gender, age, height, weight);
    // 读取串口数据
    m_pSerialPort->connectReadEvent(this);
    // 设置为正在检测
    m_checking = true;
    return Error::NoError;
}

int ZeYaoTebcoPrivate::appendBpAndPosition(int sbp, int dbp, int position)
{
    // 未进行检测
    if (!m_checking)
        return Error::NotDetected;
    // 输入错误
    if (sbp <= dbp || sbp < 0 || sbp > 300 || dbp < 0 || dbp > 300 || position < 1 || position > 3)
        return Error::InputError;
    if (!m_reportData.dataIntegrity(false)) {
        return Error::NoDataDetected;
    }
    // 报告
    m_reportData.appendPosition(sbp, dbp, position);
    return Error::NoError;
}

int ZeYaoTebcoPrivate::end(int sbp, int dbp, int postiion)
{
    auto ret = appendBpAndPosition(sbp, dbp, postiion);
    if (0 != ret) {
        return ret;
    }
    // 推送数据
    publishReportData();
    // 设为未检测状态
    m_checking = false;
    return Error::NoError;
}

void ZeYaoTebcoPrivate::closeSerialPort()
{
    if (m_pSerialPort->isOpen()) {
        if (m_reportData.dataIntegrity(false)) {
            m_reportData.appendDefault();
            publishReportData();
        }
        m_pSerialPort->close();
    }
    // 设为未检测状态
    m_checking = false;
}

std::string ZeYaoTebcoPrivate::readAll()
{
    if (!dataIntegrity()) return std::string();
    auto str = m_jsonObject.toStyledString();
    m_jsonObject.clear();
    return str;
}

bool ZeYaoTebcoPrivate::serverIsConnected()
{
    return m_client.is_connected();
}

bool ZeYaoTebcoPrivate::serialPortIsOpen()
{
    return m_pSerialPort->isOpen();
}

bool ZeYaoTebcoPrivate::isDetecting()
{
    return m_checking;
}

int ZeYaoTebcoPrivate::availableQuantity()
{
    try {
        if (m_deviceInfo["status"].asInt() == 0) {
            return 0;
        }
        return (std::atoi(m_deviceInfo["totalcount"].asString().c_str())
                - std::atoi(m_deviceInfo["usedcount"].asString().c_str()));
    } catch (...) {
        return 0;
    }
    return 0;
}

std::string ZeYaoTebcoPrivate::availablePorts()
{
    Json::Value portNames;
    for(auto info: itas109::CSerialPortInfo::availablePortInfos()) {
        portNames.append(info.portName);
    }
    return portNames.toStyledString();
}

std::string ZeYaoTebcoPrivate::serialPortErrorMsg()
{
    return std::string(m_pSerialPort->getLastErrorMsg());
}

void ZeYaoTebcoPrivate::onReadEvent(const char *portName, unsigned int readBufferLen)
{
    uint8_t *data = new uint8_t[readBufferLen];
    m_pSerialPort->readAllData(data);
    if (4 == readBufferLen) {
        std::string str(8, 0);
        snprintf(str.data(), str.max_size(), "%02x%02x%02x%02x",
                 data[0], data[1], data[2], data[3]);
        m_reportData.appendWaveformData(str);
    }
    else if (6 == readBufferLen) {
        std::string str(8, 0);
        snprintf(str.data(), str.max_size(), "%02x%02x%02x%02x",
                 data[0], data[1], data[2], data[3]);
        m_reportData.appendWaveformData(str);
        Value value;
        uint16_t d = data[4] << 8 | data[5];
        std::memcpy(&value, &d, 2);
        m_reportData.appendData(value.type, value.data);
        switch (value.type) {
        case 0:
            m_jsonObject["hr"] = toString(intercept(DatCa::cHr(value.data), 0));
            break;
        case 7:
        {
            auto si = intercept(DatCa::cSi(value.data, m_data.bsa, m_data.vept), 0);
            m_jsonObject["si"] = toString(si);
            m_jsonObject["sv"] = toString(intercept(DatCa::cSv(si, m_data.bsa), 1));
        }
        break;
        case 8:
        {
            auto ci = intercept(DatCa::cCi(value.data, m_data.bsa, m_data.vept), 1);
            m_jsonObject["ci"] = toString(ci);
            m_jsonObject["co"] = toString(intercept(DatCa::cCo(ci, m_data.bsa), 1));
            m_jsonObject["hrv"] = toString(intercept(DatCa::cHrv(ci), 0));
        }
        break;
        default:
            break;
        }
    }
    delete[] data;
    data = nullptr;
}

void ZeYaoTebcoPrivate::publishReportData()
{
    // 断开串口数据接收
    m_pSerialPort->disconnectReadEvent();
    // 推送的数据
    Json::Value pdata;
    pdata["reporttime"] = m_reportData.reportTime();
    pdata["deviceid"] = m_deviceId;
    pdata["name"] = m_data.id + "-" + m_data.name;
    pdata["modify"] = 0;
    pdata["reportdata"] = m_reportData.getJson();
    m_client.publish(upload_data_publish_topic(), pdata.toStyledString(), QOS, false);
    // 清空
    m_reportData.clear();
    m_jsonObject.clear();
    // 使用数量+1
    try {
        int usedcount = std::atoi(m_deviceInfo["usedcount"].asString().c_str()) + 1;
        m_deviceInfo["usedcount"] = std::to_string(usedcount);
    } catch (...) {
    }
}

void ZeYaoTebcoPrivate::serialPortReconnect(itas109::CSerialPort *serialPort)
{
    int hotplug_sock = init_hotplug_sock();
    while(1)
    {
        /* Netlink message buffer */
        char buf[4096] = {0};
        recv(hotplug_sock, &buf, sizeof(buf), 0);
        auto msg = std::string(buf);
        auto portname = std::string(serialPort->getPortName());
        if (!portname.empty()) {
            auto ppos = portname.rfind('/');
            if (std::string::npos != ppos) {
                portname = portname.substr(ppos + 1);
            }
            if (serialPort->isOpen()) {
                if (std::string::npos != msg.find("unbind") && std::string::npos != msg.rfind(portname)) {
                    serialPort->close();
                }
            }
            else {
                if (std::string::npos != msg.find("bind") && std::string::npos != msg.rfind(portname)) {
                    std::thread t([&](){
                        do {
                            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                        }while (!serialPort->open());
                    });
                    t.detach();
                }
            }
        }
    }
}

bool ZeYaoTebcoPrivate::dataIntegrity() const
{
    return (m_jsonObject.size() >= 6);
}

std::string ZeYaoTebcoPrivate::toString(const double &d)
{
    std::ostringstream ss;
    ss<<d;
    return ss.str();
}

int ZeYaoTebcoPrivate::init_hotplug_sock()
{
    const int buffersize = 1024;
    int ret;

    struct sockaddr_nl snl;
    memset(&snl, 0, sizeof(struct sockaddr_nl));
    snl.nl_family = AF_NETLINK;
    snl.nl_pid = getpid();
    snl.nl_groups = 1;

    int s = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_KOBJECT_UEVENT);
    if (s == -1)
    {
        perror("socket");
        return -1;
    }
    setsockopt(s, SOL_SOCKET, SO_RCVBUF, &buffersize, sizeof(buffersize));

    ret = bind(s, (struct sockaddr *)&snl, sizeof(struct sockaddr_nl));
    if (ret < 0)
    {
        perror("bind");
        close(s);
        return -1;
    }

    return s;
}

std::string ZeYaoTebcoPrivate::login_publish_topic()
{
    return std::string("request/devicedata/mac_" + mac_address());
}

std::string ZeYaoTebcoPrivate::upload_data_publish_topic()
{
    return std::string("request/uploaddata/mac_" + mac_address());
}

std::string ZeYaoTebcoPrivate::subscribe_topic()
{
    return std::string("response/devicedata/mac_" + mac_address());
}

std::string ZeYaoTebcoPrivate::server_address()
{
    return std::string("nicas.zeyaotebco.com:1883");
}

std::string ZeYaoTebcoPrivate::mac_address()
{
    struct ifreq ifr;
    struct ifconf ifc;
    char buf[1024];
    int success = 0;

    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
    if (sock == -1) { /* handle error*/ };

    ifc.ifc_len = sizeof(buf);
    ifc.ifc_buf = buf;
    if (ioctl(sock, SIOCGIFCONF, &ifc) == -1) { /* handle error */ }

    struct ifreq* it = ifc.ifc_req;
    const struct ifreq* const end = it + (ifc.ifc_len / sizeof(struct ifreq));

    for (; it != end; ++it) {
        strncpy(ifr.ifr_name, it->ifr_name, sizeof(it->ifr_name));
        if (ioctl(sock, SIOCGIFFLAGS, &ifr) == 0) {
            if (! (ifr.ifr_flags & IFF_LOOPBACK)) { // don't count loopback
                if (ioctl(sock, SIOCGIFHWADDR, &ifr) == 0) {
                    success = 1;
                    break;
                }
            }
        }
        else { /* handle error */ }
    }

    unsigned char mac_address[6] = "";

    if (success) memcpy(mac_address, ifr.ifr_hwaddr.sa_data, 6);
    std::string mac(12, 0);
    snprintf(mac.data(), mac.max_size(), "%02x%02x%02x%02x%02x%02x",
            mac_address[0],
            mac_address[1],
            mac_address[2],
            mac_address[3],
            mac_address[4],
            mac_address[5]);
    return mac;
}
