#include "telegramprofile.h"
#include <QCryptographicHash>
#include <QDataStream>

TelegramProfile::TelegramProfile()
    : start_identifier(0),
      header_length(0),
      telegram_type(TelegramType::InvalidType),
      data_length(0),
      data_checksum(nullptr),
      header_checksum(nullptr),
      body_data(nullptr)
{

}

TelegramProfile::TelegramProfile(const TelegramType &type, const QByteArray &data)
    : start_identifier(START_IDENTIFIER),
      header_length(HEADER_LENGTH),
      telegram_type(type),
      data_length(data.length()),
      body_data(data)
{
    data_checksum = QCryptographicHash::hash(data, QCryptographicHash::Md5);
    QByteArray header;
    QDataStream stream(&header, QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_5_12);
    stream.setFloatingPointPrecision(QDataStream::DoublePrecision);
    stream<<start_identifier<<header_length<<telegram_type<<data_length;
    stream.writeRawData(data_checksum, data_checksum.length());
    header_checksum = QCryptographicHash::hash(header, QCryptographicHash::Md5);
}

QByteArray TelegramProfile::toByteArray() const
{
    QByteArray bytes;
    QDataStream stream(&bytes, QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_5_12);
    stream.setFloatingPointPrecision(QDataStream::DoublePrecision);
    stream<<startIdentifier()<<headerLength()<<telegramType()<<dataLength();
    stream.writeRawData(dataChecksum(), dataChecksum().length());
    stream.writeRawData(headerChecksum(), headerChecksum().length());
    stream.writeRawData(bodyData(), bodyData().length());
    if (fromUtf8(bytes) == TelegramProfile()) {
        return QByteArray();
    }
    return bytes;
}

uint32_t TelegramProfile::startIdentifier() const
{
    return start_identifier;
}

uint16_t TelegramProfile::headerLength() const
{
    return header_length;
}

TelegramType TelegramProfile::telegramType() const
{
    return telegram_type;
}

uint32_t TelegramProfile::dataLength() const
{
    return data_length;
}

QByteArray TelegramProfile::dataChecksum() const
{
    return data_checksum;
}

QByteArray TelegramProfile::headerChecksum() const
{
    return header_checksum;
}

QByteArray TelegramProfile::bodyData() const
{
    return body_data;
}

TBInfo TelegramProfile::baseInfo(const QByteArray &data)
{
    QByteArray header;
    QDataStream in(&header, QIODevice::WriteOnly);
    in.setVersion(QDataStream::Qt_5_12);
    in.setFloatingPointPrecision(QDataStream::DoublePrecision);
    in<<static_cast<uint32_t>(START_IDENTIFIER)<<static_cast<uint16_t>(HEADER_LENGTH);
    auto index = data.indexOf(header);
    if (-1 == index) {
        return TBInfo();
    }

    auto temp = data.mid(index);
    if (temp.length() < HEADER_LENGTH) {
        return TBInfo();
    }

    QDataStream out(temp);
    out.setVersion(QDataStream::Qt_5_12);
    out.setFloatingPointPrecision(QDataStream::DoublePrecision);

    uint32_t start_identifier;
    uint16_t header_length;
    int16_t telegram_type;
    uint32_t data_length;

    out>>start_identifier>>header_length>>telegram_type>>data_length;

    if (start_identifier != START_IDENTIFIER ||
        header_length != HEADER_LENGTH ||
        header_length + data_length != temp.length()) {
        return TBInfo();
    }
    switch (TelegramType(telegram_type)) {
    #define X_MACRO(type, value) case type:
        ENUM_TELEGRAM_TYPE
    #undef X_MACRO
        break;
    default:
        return TBInfo();
    }
    return TBInfo(index, temp.length());
}

TelegramProfile TelegramProfile::fromUtf8(const QByteArray &data)
{
    if (data.length() < HEADER_LENGTH) {
        return TelegramProfile();
    }

    QDataStream out(data);
    out.setVersion(QDataStream::Qt_5_12);
    out.setFloatingPointPrecision(QDataStream::DoublePrecision);

    uint32_t start_identifier;
    uint16_t header_length;
    int16_t telegram_type;
    uint32_t data_length;

    out>>start_identifier>>header_length>>telegram_type>>data_length;

    if (start_identifier != START_IDENTIFIER ||
        header_length != HEADER_LENGTH ||
        header_length + data_length != data.length()) {
        return TelegramProfile();
    }
    switch (TelegramType(telegram_type)) {
    #define X_MACRO(type, value) case type:
        ENUM_TELEGRAM_TYPE
    #undef X_MACRO
        break;
    default:
        return TelegramProfile();
    }

    QByteArray data_checksum;
    QByteArray header_checksum;
    QByteArray body_data;

    data_checksum.resize(MD5_CHECKSUM_LENGTH);
    header_checksum.resize(MD5_CHECKSUM_LENGTH);
    body_data.resize(data_length);

    out.readRawData(data_checksum.data(), MD5_CHECKSUM_LENGTH);
    out.readRawData(header_checksum.data(), MD5_CHECKSUM_LENGTH);
    out.readRawData(body_data.data(), data_length);

    TelegramProfile tp(TelegramType(telegram_type), body_data);
    if (tp.dataChecksum() != data_checksum || tp.headerChecksum() != header_checksum) {
        return TelegramProfile();
    }

    return tp;
}

bool TelegramProfile::operator==(const TelegramProfile &tp)
{
    if (tp.startIdentifier() != startIdentifier() ||
        tp.headerLength() != headerLength() ||
        tp.telegramType() != telegramType() ||
        tp.dataLength() != dataLength() ||
        tp.dataChecksum() != dataChecksum() ||
        tp.headerChecksum() != headerChecksum() ||
        tp.bodyData() != bodyData()) {
        return false;
    }
    return true;
}

QDebug operator<<(QDebug debug, const TelegramProfile &tp)
{
    if (TelegramType::InvalidType == tp.telegramType()) {
        debug<<QString("Invalid telegram.");
    }
    else {
        debug.noquote().nospace()
            <<QString("start_identifier: 0x%1\n").arg(tp.startIdentifier(), 8, 16, QLatin1Char('0'))
            <<QString("header_length: %1\n").arg(tp.headerLength())
            <<QString("telegram_type: %1\n").arg(tp.telegramType())
            <<QString("data_length: %1\n").arg(tp.dataLength())
            <<QString("data_checksum: ")<<tp.dataChecksum().toHex()<<"\n"
            <<QString("header_checksum: ")<<tp.headerChecksum().toHex()<<"\n"
            <<QString("body_data: ")<<QString::fromUtf8(tp.bodyData());
    }
    return debug;
}
