#ifndef DATABASENS_H
#define DATABASENS_H

#include <QObject>
#include <QMetaEnum>
#include <QDebug>

#define ENABLE_COMBINE_DEVICE 0

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
#if ENABLE_COMBINE_DEVICE
    uniqueIds,
#else
    deviceIds,
#endif
    remarks
};
Q_ENUM_NS(AdministratorInfo)
enum class AllocatedConsumables
{
    createTime,
    type,
#if ENABLE_COMBINE_DEVICE
    uniqueId,
#else
    deviceId,
#endif
    agentId,
    adminId,
    count,
};
Q_ENUM_NS(AllocatedConsumables)
#if ENABLE_COMBINE_DEVICE
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
#endif
enum class Device
{
    deviceId,

#if ENABLE_COMBINE_DEVICE
    batch,
    remarks,
#else
    password,
    placeId,
    agentId,
    adminId,
#endif
    type,
    status
};
Q_ENUM_NS(Device)
#if ENABLE_COMBINE_DEVICE
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
#endif
enum class ReportInfo
{
    reportTime,
#if ENABLE_COMBINE_DEVICE
    uniqueId,
#else
    deviceId,
#endif
    name,
    modify,
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
    createTime
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
#if ENABLE_COMBINE_DEVICE
    static QString cn_CombinedDevice(const CombinedDevice &en);
#endif
    static QString cn_Device(const Device &en);
#if ENABLE_COMBINE_DEVICE
    static QString cn_Computer(const Computer &en);
#endif
    static QString cn_ReportInfo(const ReportInfo &en);
    static QString cn_SoftwareManagement(const SoftwareManagement &en);
    static QString cn_PlaceInfo(const PlaceInfo &en);
    static QString cn_EnumName(const QString &en);
    static QString cn_EnumValue(const QString &enumName, const QString &en);
    template<class T>
    static bool compareEname(const QString &name)
    {
        return (0 == name.compare(QString(QMetaEnum::fromType<T>().enumName()), Qt::CaseInsensitive));
    }
    template <class T>
    static T toEnumValue(const QString &key)
    {
        return T(QMetaEnum::fromType<T>().keyToValue(toEnumKey<T>(key).toStdString().c_str()));
    }
    // en change to enum key(case insensitive)
    template <class T>
    static QString toEnumKey(const QString &key)
    {
        auto meta = QMetaEnum::fromType<T>();
        for (int index = 0; index < meta.keyCount(); ++index) {
            if (0 == key.compare(QString(meta.key(index)), Qt::CaseInsensitive)) {
                return QString(meta.key(index));
            }
        }
        return key;
    }
    template <class T>
    static bool compareEkey(const QString &ekey)
    {
        auto meta = QMetaEnum::fromType<T>();
        for (int index = 0; index < meta.keyCount(); ++index) {
            if (0 == ekey.compare(QString(meta.key(index)), Qt::CaseInsensitive)) {
                return true;
            }
        }
        return false;
    }
};
}

#endif // DATABASENS_H
