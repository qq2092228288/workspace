#ifndef ADSMSGSTRUCT_H
#define ADSMSGSTRUCT_H

#include <QObject>
#include <QMetaEnum>

namespace AdsMsgStructNs {
Q_NAMESPACE
/*! Structure and size of data frame 1, [0-37] */
enum class DataFrameI
{
    FrameHeader = 3, // 0-2
    FrameLength = 1, // 3
    LeadWireI   = 4, // 4-7
    LeadWireII  = 4, // 8-11
    LeadWireV1  = 4, // 12-15
    LeadWireV2  = 4, // 16-19
    LeadWireV3  = 4, // 20-23
    LeadWireV4  = 4, // 24-27
    LeadWireV5  = 4, // 28-31
    LeadWireV6  = 4, // 32-35
    Checksums = 1, // 36
    AdditionalVerification = 1 // 37
};
Q_ENUM_NS(DataFrameI)
/*! Structure and size of data frame 2, [0-24] */
enum class DataFrameII
{
    FrameHeader = 3, // 0-2
    FrameLength = 1, // 3
    LeadWireIII = 4, // 4-7
    LeadWireAVR = 4, // 8-11
    LeadWireAVL = 4, // 12-15
    LeadWireAVF = 4, // 16-19
    FallingOffMark = 2, // 20-21
    IncrementalTestData = 1, // 22
    Checksums = 1, // 23
    AdditionalVerification = 1 // 24
};
Q_ENUM_NS(DataFrameII)
/*! Get enum value to enum class key */
template<class T>
static qint32 evalue(const T &key)
{
    return static_cast<qint32>(key);
}
/*! Sum the values of enum */
template<class T>
static qint32 esum()
{
    qint32 sum = 0;
    QMetaEnum meta = QMetaEnum::fromType<T>();
    for (int i = 0, count = meta.keyCount(); i < count; ++i) {
        sum += meta.value(i);
    }
    return sum;
}
}

#endif // ADSMSGSTRUCT_H
