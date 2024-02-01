#include "telegramprofile.h"
#include <QCryptographicHash>
#include <QDataStream>

TelegramProfile::TelegramProfile()
    : m_start_identifier(0),
      m_header_length(0),
      m_telegram_type(TelegramType::InvalidType),
      m_data_length(0),
      m_data_checksum(nullptr),
      m_header_checksum(nullptr),
      m_body_data(nullptr)
{

}

TelegramProfile::TelegramProfile(const TelegramType &type, const QByteArray &data)
    : m_start_identifier(START_IDENTIFIER),
      m_header_length(HEADER_LENGTH),
      m_telegram_type(type),
      m_data_length(data.length()),
      m_body_data(data)
{
    m_data_checksum = QCryptographicHash::hash(QString(data).toUtf8(), QCryptographicHash::Md5);
    QByteArray header;
    QDataStream stream(&header, QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_5_12);
    stream.setFloatingPointPrecision(QDataStream::DoublePrecision);
    stream<<m_start_identifier<<m_header_length<<static_cast<int16_t>(m_telegram_type)<<m_data_length;
    stream.writeRawData(m_data_checksum, m_data_checksum.length());
    m_header_checksum = QCryptographicHash::hash(QString(header).toUtf8(), QCryptographicHash::Md5);
}

TelegramProfile::~TelegramProfile()
{
    m_data_checksum.clear();
    m_header_checksum.clear();
    m_body_data.clear();
}

QByteArray TelegramProfile::toByteArray() const
{
    QByteArray bytes;
    QDataStream stream(&bytes, QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_5_12);
    stream.setFloatingPointPrecision(QDataStream::DoublePrecision);
    stream<<startIdentifier()<<headerLength()<<static_cast<int16_t>(telegramType())<<dataLength();
    stream.writeRawData(dataChecksum(), dataChecksum().length());
    stream.writeRawData(headerChecksum(), headerChecksum().length());
    stream.writeRawData(bodyData(), bodyData().length());
    if (fromUtf8(bytes) == TelegramProfile()) {
        return QByteArray();
    }
    return bytes;
}

int32_t TelegramProfile::startIdentifier() const
{
    return m_start_identifier;
}

int16_t TelegramProfile::headerLength() const
{
    return m_header_length;
}

TelegramType TelegramProfile::telegramType() const
{
    return m_telegram_type;
}

int32_t TelegramProfile::dataLength() const
{
    return m_data_length;
}

QByteArray TelegramProfile::dataChecksum() const
{
    return m_data_checksum;
}

QByteArray TelegramProfile::headerChecksum() const
{
    return m_header_checksum;
}

QByteArray TelegramProfile::bodyData() const
{
    return m_body_data;
}

TBInfo TelegramProfile::baseInfo(const QByteArray &data)
{
    QByteArray header;
    QDataStream in(&header, QIODevice::WriteOnly);
    in.setVersion(QDataStream::Qt_5_12);
    in.setFloatingPointPrecision(QDataStream::DoublePrecision);
    in<<static_cast<int32_t>(START_IDENTIFIER)<<static_cast<int16_t>(HEADER_LENGTH);
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

    int32_t start_identifier;
    int16_t header_length;
    int16_t telegram_type;
    int32_t data_length;

    out>>start_identifier>>header_length>>telegram_type>>data_length;

    if (start_identifier != START_IDENTIFIER ||
        header_length != HEADER_LENGTH ||
        header_length + data_length > temp.length()) {
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

    int32_t start_identifier;
    int16_t header_length;
    int16_t telegram_type;
    int32_t data_length;

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
