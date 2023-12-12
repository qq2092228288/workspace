#ifndef TCPCLIENTSOCKET_H
#define TCPCLIENTSOCKET_H

#include <QObject>
#include <QTcpSocket>
#include <QQueue>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include "telegramprofile.h"

class TcpClientSocket : public QTcpSocket
{
    Q_OBJECT
public:
    explicit TcpClientSocket(qintptr socketDescriptor, QObject *parent = nullptr);
    virtual ~TcpClientSocket();
public slots:
    void writeReady(qintptr socketDescriptor, const QByteArray &data);
    void appendReports(qintptr socketDescriptor, const QJsonArray &reports);
signals:
    void send(qintptr socketDescriptor, TelegramType type, const QByteArray &data);
private slots:
    void dataReceived();
private:
    qintptr m_socketDescriptor;
    QByteArray m_data;
    QQueue<QJsonArray> m_reportQueue;
};

#endif // TCPCLIENTSOCKET_H
