#include "tcpclientsocket.h"
#include <QJsonDocument>
#include <QJsonObject>

TcpClientSocket::TcpClientSocket(qintptr socketDescriptor, QObject *parent)
    : QTcpSocket{parent}
    , m_socketDescriptor{socketDescriptor}
{
    setSocketDescriptor(socketDescriptor);
    connect(this, &TcpClientSocket::readyRead, this, &TcpClientSocket::dataReceived);
}

TcpClientSocket::~TcpClientSocket()
{
    // qDebug()<<__FUNCTION__;
}

void TcpClientSocket::writeReady(qintptr socketDescriptor, const QByteArray &data)
{
    if (this->socketDescriptor() == socketDescriptor) {
        write(data);
    }
}

void TcpClientSocket::dataReceived()
{
    m_data.append(readAll());
    auto info = TProfile::baseInfo(m_data);
    if (info.index != -1 && info.length <= m_data.length()) {
        auto tp = TProfile::fromUtf8(m_data.mid(info.index, info.length));
        m_data = m_data.mid(info.index + info.length);
        if (tp.telegramType() != TelegramType::InvalidType) {
            emit send(socketDescriptor(), tp.telegramType(), QJsonDocument::fromJson(tp.bodyData()).object());
        }
    }
}

