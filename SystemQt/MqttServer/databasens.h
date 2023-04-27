#ifndef DATABASENS_H
#define DATABASENS_H

#include <QObject>

namespace DatabaseEnumNs {
Q_NAMESPACE
enum class MessageError
{
    NoError,
    TopicInvalid,
    IsEmpty,
    IncorrectFormat,
    NoAnalysis,
    RequiredDataIsIncomplete
};
Q_ENUM_NS(MessageError)
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
    reportTime,
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
}

#endif // DATABASENS_H
