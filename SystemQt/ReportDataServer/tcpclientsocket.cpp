#include "tcpclientsocket.h"
#include "singleton.h"

TcpClientSocket::TcpClientSocket(qintptr socketDescriptor, QObject *parent)
    : QTcpSocket{parent}
    , m_socketDescriptor{socketDescriptor}
{
    setSocketDescriptor(socketDescriptor);
    connect(this, &TcpClientSocket::readyRead, this, &TcpClientSocket::dataReceived);
}

TcpClientSocket::~TcpClientSocket()
{
    m_data.clear();
    m_reportQueue.clear();
}

void TcpClientSocket::writeReady(qintptr socketDescriptor, const QByteArray &data)
{
    if (this->socketDescriptor() == socketDescriptor) {
        write(data);
    }
}

void TcpClientSocket::appendReports(qintptr socketDescriptor, const QJsonArray &reports)
{
    if (this->socketDescriptor() == socketDescriptor) {
        m_reportQueue.enqueue(reports);
    }
}

void TcpClientSocket::dataReceived()
{
    m_data.append(readAll());
    auto info = TelegramProfile::baseInfo(m_data);
    if (info.index != -1 && info.length <= m_data.length()) {
        auto tp = TelegramProfile::fromUtf8(m_data.mid(info.index, info.length));
        m_data = m_data.mid(info.index + info.length);
        if (tp.telegramType() == TelegramType::InvalidType) {   // 无效数据
            return;
        }
        else {
            emit send(socketDescriptor(), tp.telegramType(), tp.bodyData());
        }
    }
}

