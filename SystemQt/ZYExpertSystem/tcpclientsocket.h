#ifndef TCPCLIENTSOCKET_H
#define TCPCLIENTSOCKET_H

#include <QObject>
#include <QTcpSocket>
#include <QCoreApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QSqlDatabase>
#include <QSqlQuery>
#include "telegramprofile.h"

class TcpClientSocket : public QTcpSocket
{
    Q_OBJECT
public:
    QSqlDatabase &db();
public slots:
    void connectToServer();
    void getSoftwareInfo();
    void login(const QString &username, const QString &password);
    void getReports();
    void getNewReports();
    void writeReady(const TelegramType &type, const QByteArray &bodyData);
signals:
    void versionInfo(const QJsonObject &);
    void userInfo(const QJsonObject &);
    void reportReady(int);
    void reportPulled();
private slots:
    void connectedSlot();
    void dataReceived();
private:
    const QString m_appId;
    QByteArray m_userInfo;
    QSqlDatabase m_db;
    QByteArray m_data;
private:
    explicit TcpClientSocket(QObject *parent = nullptr);
    virtual ~TcpClientSocket();
    TcpClientSocket(const TcpClientSocket &) = delete;
    TcpClientSocket(const TcpClientSocket &&) = delete;
    TcpClientSocket &operator=(const TcpClientSocket &) = delete;
public:
    static TcpClientSocket *getInstance();
};

#endif // TCPCLIENTSOCKET_H
