#ifndef TELEGRAM_PROFILE_H
#define TELEGRAM_PROFILE_H

#include <QDebug>

/*! 报文起始标识 */
#define START_IDENTIFIER                0x07E65A59
/*! 报文头长度 */
#define HEADER_LENGTH                   44
/*! MD5校验和长度 */
#define MD5_CHECKSUM_LENGTH             16
/*! 报文类型宏 */
#define ENUM_TELEGRAM_TYPE  \
        X_MACRO(SoftwareInfomation,     0)  \
        X_MACRO(UserInfomation,         1)  \
        X_MACRO(DeviceInfomation,       2)  \
        X_MACRO(ReportData,             3)  \
        X_MACRO(InvalidType,    INT16_MAX)
/*! 报文类型枚举 */
enum TelegramType : int16_t
{
#define X_MACRO(type, value) type = value,
    ENUM_TELEGRAM_TYPE
#undef X_MACRO
};
/*! 报文基本信息 */
typedef struct TelegramBaseInfo
{
    TelegramBaseInfo(int _index = -1, int _length = 0):index(_index),length(_length){}
    const int index;  // 在数据中的起始位置
    const int length; // 报文大小
}TBInfo;

/*! 报文结构 */
typedef class TelegramProfile
{
public:
    explicit TelegramProfile();
    explicit TelegramProfile(const TelegramType &type, const QByteArray &data);
public:
    QByteArray toByteArray() const;
    uint32_t startIdentifier() const;
    int16_t headerLength() const;
    TelegramType telegramType() const;
    int32_t dataLength() const;
    QByteArray dataChecksum() const;
    QByteArray headerChecksum() const;
    QByteArray bodyData() const;
public:
    static TBInfo baseInfo(const QByteArray &data);
    static TelegramProfile fromUtf8(const QByteArray &data);
private:
    /*! 起始标识 */
    uint32_t start_identifier;
    /*! 头长度 */
    int16_t header_length;
    /*! 报文类型 */
    TelegramType telegram_type;
    /*! 数据长度 */
    int32_t data_length;
    /*! 数据校验和(MD5) */
    QByteArray data_checksum;
    /*! 报文头校验和(MD5) */
    QByteArray header_checksum;
    /*! 数据 */
    QByteArray body_data;
private:
    bool operator==(const TelegramProfile &tp);
}TProfile;
QDebug operator<<(QDebug debug, const TelegramProfile &tp);

#endif  // TELEGRAM_PROFILE_H

