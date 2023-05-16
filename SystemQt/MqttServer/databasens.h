#ifndef DATABASENS_H
#define DATABASENS_H

#include <QObject>
#include <QMetaEnum>
#include <QDebug>

namespace DatabaseEnumNs {
Q_NAMESPACE
enum class MessageError
{
    TopicInvalid,
    IsEmpty,
    IncorrectFormat,
    NoAnalysis,
    RequiredDataIsIncomplete,
    IllegalId
};
Q_ENUM_NS(MessageError)
enum class UserPermissions
{
    SuperAdministrator,
    SecondaryAdministrator,
    ReportSelectAndModify,
    ReportSelect
};
Q_ENUM_NS(UserPermissions)
enum class AgentInfo
{
    agentId,
    name,
    contact,
    address,
    remarks
};
Q_ENUM_NS(AgentInfo)
enum class AdministratorInfo
{
    adminId,
    password,
    name,
    permission,
    uniqueIds,
    remarks
};
Q_ENUM_NS(AdministratorInfo)
enum class AllocatedConsumables
{
    consumablesId,
    type,
    uniqueId,
    agentId,
    adminId,
    count,
    time
};
Q_ENUM_NS(AllocatedConsumables)
enum class CombinedDevice
{
    uniqueId,
    name,
    deviceId,
    macAddress,
    placeId,
    agentId,
    status,
    totalCount,
    usedCount
};
Q_ENUM_NS(CombinedDevice)
enum class Device
{
    deviceId,
    type,
    batch,
    status,
    remarks
};
Q_ENUM_NS(Device)
enum class Computer
{
    macAddress,
    type,
    systemVersion,
    batch,
    status,
    remarks
};
Q_ENUM_NS(Computer)
enum class ReportInfo
{
    uniqueId,
    time,
    type,
    reportData
};
Q_ENUM_NS(ReportInfo)
enum class SoftwareManagement
{
    appId,
    name,
    version,
    downloadPath,
    content,
    time
};
Q_ENUM_NS(SoftwareManagement)
enum class PlaceInfo
{
    placeId,
    hostName,
    secondaryName
};
Q_ENUM_NS(PlaceInfo)
class EnumTextCN
{
public:
    static QString cn_AgentInfo(const AgentInfo &en);
    static QString cn_AdministratorInfo(const AdministratorInfo &en);
    static QString cn_AllocatedConsumables(const AllocatedConsumables &en);
    static QString cn_CombinedDevice(const CombinedDevice &en);
    static QString cn_Device(const Device &en);
    static QString cn_Computer(const Computer &en);
    static QString cn_ReportInfo(const ReportInfo &en);
    static QString cn_SoftwareManagement(const SoftwareManagement &en);
    static QString cn_PlaceInfo(const PlaceInfo &en);
    template<class T>
    static QString cn_EnumName()
    {
        if (std::is_same<T, AgentInfo>::value)
            return "经销商";
        else if (std::is_same<T, AdministratorInfo>::value)
            return "账号信息";
        else if (std::is_same<T, AllocatedConsumables>::value)
            return "耗材分配";
        else if (std::is_same<T, CombinedDevice>::value)
            return "设备";
        else if (std::is_same<T, Device>::value)
            return "公司设备";
        else if (std::is_same<T, Computer>::value)
            return "电脑";
        else if (std::is_same<T, ReportInfo>::value)
            return "报告";
        else if (std::is_same<T, SoftwareManagement>::value)
            return "软件管理";
        else if (std::is_same<T, PlaceInfo>::value)
            return "场所";
        return "未定义";
    }
    static QString cn_EnumName(const QString &en)
    {
        if (compare<AgentInfo>(en))
            return "经销商";
        else if (compare<AdministratorInfo>(en))
            return "账号信息";
        else if (compare<AllocatedConsumables>(en))
            return "耗材分配";
        else if (compare<CombinedDevice>(en))
            return "设备";
        else if (compare<Device>(en))
            return "公司设备";
        else if (compare<Computer>(en))
            return "电脑";
        else if (compare<ReportInfo>(en))
            return "报告";
        else if (compare<SoftwareManagement>(en))
            return "软件管理";
        else if (compare<PlaceInfo>(en))
            return "场所";
        return "未定义";
    }
    static QString cn_EnumValue(const QString &enumName, const QString &en);
    template<class T>
    static bool compare(const QString &en)
    {
        return (0 == en.compare(QString(QMetaEnum::fromType<T>().enumName()), Qt::CaseInsensitive));
    }
    template <class T>
    static T toEnumValue(const QString &key)
    {
        return T(QMetaEnum::fromType<T>().keyToValue(toEnumKey<T>(key).toStdString().c_str()));
    }
    // en change to enum key(case insensitive)
    template <class T>
    static QString toEnumKey(const QString &en)
    {
        auto meta = QMetaEnum::fromType<T>();
        for (int index = 0; index < meta.keyCount(); ++index) {
            if (0 == en.compare(QString(meta.key(index)), Qt::CaseInsensitive)) {
                return QString(meta.key(index));
            }
        }
        return en;
    }
};
}

#endif // DATABASENS_H
