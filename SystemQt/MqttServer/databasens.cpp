#include "databasens.h"

namespace DatabaseEnumNs {
QString DatabaseEnumNs::EnumTextCN::cn_AgentInfo(const AgentInfo &en)
{
    switch (en) {
    case AgentInfo::agentId:
        return "经销商编号";
    case AgentInfo::name:
        return "姓名";
    case AgentInfo::contact:
        return "联系方式";
    case AgentInfo::address:
        return "地址";
    case AgentInfo::remarks:
        return "备注";
    }
    return QString();
}

QString DatabaseEnumNs::EnumTextCN::cn_AdministratorInfo(const AdministratorInfo &en)
{
    switch (en) {
    case AdministratorInfo::adminId:
        return "用户名";
    case AdministratorInfo::password:
        return "密码";
    case AdministratorInfo::name:
        return "姓名";
    case AdministratorInfo::permission:
        return "权限";
    case AdministratorInfo::uniqueIds:
        return "关联的设备编号";
    case AdministratorInfo::remarks:
        return "备注";
    }
    return QString();
}

QString DatabaseEnumNs::EnumTextCN::cn_AllocatedConsumables(const AllocatedConsumables &en)
{
    switch (en) {
    case AllocatedConsumables::consumablesId:
        return "耗材批次编号";
    case AllocatedConsumables::type:
        return "耗材类型";
    case AllocatedConsumables::uniqueId:
        return "设备编号";
    case AllocatedConsumables::agentId:
        return "经销商编号";
    case AllocatedConsumables::adminId:
        return "用户名";
    case AllocatedConsumables::count:
        return "耗材数量";
    case AllocatedConsumables::time:
        return "创建时间";
    }
    return QString();
}

QString DatabaseEnumNs::EnumTextCN::cn_CombinedDevice(const CombinedDevice &en)
{
    switch (en) {
    case CombinedDevice::uniqueId:
        return "设备编号";
    case CombinedDevice::name:
        return "设备名称";
    case CombinedDevice::deviceId:
        return "公司设备编号";
    case CombinedDevice::macAddress:
        return "MAC地址";
    case CombinedDevice::placeId:
        return "场所编号";
    case CombinedDevice::agentId:
        return "经销商编号";
    case CombinedDevice::status:
        return "当前状态";
    case CombinedDevice::totalCount:
        return "耗材总量";
    case CombinedDevice::usedCount:
        return "已使用数量";
    }
    return QString();
}

QString DatabaseEnumNs::EnumTextCN::cn_Device(const Device &en)
{
    switch (en) {
    case Device::deviceId:
        return "公司设备编号";
    case Device::type:
        return "设备类型";
    case Device::batch:
        return "批次";
    case Device::status:
        return "当前状态";
    case Device::remarks:
        return "备注";
    }
    return QString();
}

QString DatabaseEnumNs::EnumTextCN::cn_Computer(const Computer &en)
{
    switch (en) {
    case Computer::macAddress:
        return "MAC地址";
    case Computer::type:
        return "电脑类型";
    case Computer::systemVersion:
        return "系统版本";
    case Computer::batch:
        return "批次";
    case Computer::status:
        return "当前状态";
    case Computer::remarks:
        return "备注";
    }
    return QString();
}

QString DatabaseEnumNs::EnumTextCN::cn_ReportInfo(const ReportInfo &en)
{
    switch (en) {
    case ReportInfo::uniqueId:
        return "设备编号";
    case ReportInfo::time:
        return "报告时间";
    case ReportInfo::type:
        return "报告类型";
    case ReportInfo::reportData:
        return "报告数据";
    }
    return QString();
}

QString DatabaseEnumNs::EnumTextCN::cn_SoftwareManagement(const SoftwareManagement &en)
{
    switch (en) {
    case SoftwareManagement::appId:
        return "软件编号";
    case SoftwareManagement::name:
        return "软件名称";
    case SoftwareManagement::version:
        return "软件版本";
    case SoftwareManagement::downloadPath:
        return "下载地址";
    case SoftwareManagement::content:
        return "内容";
    case SoftwareManagement::time:
        return "创建时间";
    }
    return QString();
}

QString DatabaseEnumNs::EnumTextCN::cn_PlaceInfo(const PlaceInfo &en)
{
    switch (en) {
    case PlaceInfo::placeId:
        return "场所编号";
    case PlaceInfo::hostName:
        return "主场所名称";
    case PlaceInfo::secondaryName:
        return "二级场所名称";
    }
    return QString();
}

QString EnumTextCN::cn_EnumValue(const QString &enumName, const QString &en)
{
    if (compare<AgentInfo>(enumName))
        return cn_AgentInfo(toEnumValue<AgentInfo>(en));
    else if (compare<AdministratorInfo>(enumName))
        return cn_AdministratorInfo(toEnumValue<AdministratorInfo>(en));
    else if (compare<AllocatedConsumables>(enumName))
        return cn_AllocatedConsumables(toEnumValue<AllocatedConsumables>(en));
    else if (compare<CombinedDevice>(enumName))
        return cn_CombinedDevice(toEnumValue<CombinedDevice>(en));
    else if (compare<Device>(enumName))
        return cn_Device(toEnumValue<Device>(en));
    else if (compare<Computer>(enumName))
        return cn_Computer(toEnumValue<Computer>(en));
    else if (compare<ReportInfo>(enumName))
        return cn_ReportInfo(toEnumValue<ReportInfo>(en));
    else if (compare<SoftwareManagement>(enumName))
        return cn_SoftwareManagement(toEnumValue<SoftwareManagement>(en));
    else if (compare<PlaceInfo>(enumName))
        return cn_PlaceInfo(toEnumValue<PlaceInfo>(en));
    return en;
}
}
