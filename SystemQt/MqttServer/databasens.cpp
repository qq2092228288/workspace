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
        return "用户编号";
    case AdministratorInfo::password:
        return "密码";
    case AdministratorInfo::name:
        return "姓名";
    case AdministratorInfo::permission:
        return "权限";
#if ENABLE_COMBINE_DEVICE
    case AdministratorInfo::uniqueIds:
        return "关联的设备编号";
#else
    case AdministratorInfo::deviceIds:
        return "关联的设备编号";
#endif
    case AdministratorInfo::remarks:
        return "备注";
    }
    return QString();
}

QString DatabaseEnumNs::EnumTextCN::cn_AllocatedConsumables(const AllocatedConsumables &en)
{
    switch (en) {
    case AllocatedConsumables::createTime:
        return "创建时间";
    case AllocatedConsumables::type:
        return "耗材类型";
#if ENABLE_COMBINE_DEVICE
    case AllocatedConsumables::uniqueId:
        return "设备编号";
#else
    case AllocatedConsumables::deviceId:
        return "设备编号";
#endif
    case AllocatedConsumables::agentId:
        return "经销商编号";
    case AllocatedConsumables::adminId:
        return "用户编号";
    case AllocatedConsumables::count:
        return "耗材数量";
    }
    return QString();
}
#if ENABLE_COMBINE_DEVICE
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
#endif
QString DatabaseEnumNs::EnumTextCN::cn_Device(const Device &en)
{
    switch (en) {
    case Device::deviceId:
        return "设备编号";
#if ENABLE_COMBINE_DEVICE
    case Device::batch:
        return "批次";
    case Device::remarks:
        return "备注";
#else
    case Device::password:
        return "密码";
    case Device::placeId:
        return "场所编号";
    case Device::agentId:
        return "经销商编号";
    case Device::adminId:
        return "用户编号";
#endif
    case Device::type:
        return "设备类型";
    case Device::status:
        return "状态(未激活:0, 激活:1)";
    }
    return QString();
}
#if ENABLE_COMBINE_DEVICE
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
#endif
QString DatabaseEnumNs::EnumTextCN::cn_ReportInfo(const ReportInfo &en)
{
    switch (en) {
    case ReportInfo::reportTime:
        return "报告时间";
#if ENABLE_COMBINE_DEVICE
    case ReportInfo::uniqueId:
        return "设备编号";
#else
    case ReportInfo::deviceId:
        return "设备编号";
#endif
    case ReportInfo::name:
        return "名称";
    case ReportInfo::modify:
        return "会诊(0:否, 1:是)";
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
    case SoftwareManagement::createTime:
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

QString EnumTextCN::cn_EnumName(const QString &en)
{
    if (compareEname<AgentInfo>(en))
        return "经销商";
    else if (compareEname<AdministratorInfo>(en))
        return "账号信息";
    else if (compareEname<AllocatedConsumables>(en))
        return "耗材分配";
#if ENABLE_COMBINE_DEVICE
    else if (compareEname<CombinedDevice>(en))
        return "组合设备";
#endif
    else if (compareEname<Device>(en))
        return "设备";
#if ENABLE_COMBINE_DEVICE
    else if (compareEname<Computer>(en))
        return "电脑";
#endif
    else if (compareEname<ReportInfo>(en))
        return "报告";
    else if (compareEname<SoftwareManagement>(en))
        return "软件管理";
    else if (compareEname<PlaceInfo>(en))
        return "场所";
    return "未定义";
}

QString EnumTextCN::cn_EnumValue(const QString &enumName, const QString &en)
{
    if (compareEname<AgentInfo>(enumName))
        return cn_AgentInfo(toEnumValue<AgentInfo>(en));
    else if (compareEname<AdministratorInfo>(enumName))
        return cn_AdministratorInfo(toEnumValue<AdministratorInfo>(en));
    else if (compareEname<AllocatedConsumables>(enumName))
        return cn_AllocatedConsumables(toEnumValue<AllocatedConsumables>(en));
#if ENABLE_COMBINE_DEVICE
    else if (compareEname<CombinedDevice>(enumName))
        return cn_CombinedDevice(toEnumValue<CombinedDevice>(en));
#endif
    else if (compareEname<Device>(enumName))
        return cn_Device(toEnumValue<Device>(en));
#if ENABLE_COMBINE_DEVICE
    else if (compareEname<Computer>(enumName))
        return cn_Computer(toEnumValue<Computer>(en));
#endif
    else if (compareEname<ReportInfo>(enumName))
        return cn_ReportInfo(toEnumValue<ReportInfo>(en));
    else if (compareEname<SoftwareManagement>(enumName))
        return cn_SoftwareManagement(toEnumValue<SoftwareManagement>(en));
    else if (compareEname<PlaceInfo>(enumName))
        return cn_PlaceInfo(toEnumValue<PlaceInfo>(en));
    return en;
}
}
