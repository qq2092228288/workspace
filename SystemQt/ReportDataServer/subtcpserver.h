#ifndef SUBTCPSERVER_H
#define SUBTCPSERVER_H

#include <QObject>
#include <QSqlDatabase>
#include "tcpclientsocket.h"

class SubTcpServer : public QObject
{
    Q_OBJECT
public:
    explicit SubTcpServer(const int index, QObject *parent = nullptr);
    virtual ~SubTcpServer();
    int index() const;
    int count() const;
public slots:
    void newSocketDescriptor(qintptr socketDescriptor, int index);
signals:
    void writeReady(qintptr socketDescriptor, QByteArray data);
    void appendReports(qintptr socketDescriptor, QJsonArray reports);
private slots:
    void received(qintptr socketDescriptor, TelegramType type, const QByteArray &data);
    void clientDeleted(QObject *client);
private:
    template <class T>
    void clientWrite(qintptr socketDescriptor, TelegramType type, const T &json);
private:
    const int m_index;
    const QString m_reportInfoStr;
    const QString m_reportTimeStr;
    const QString m_deviceIdStr;
    const QString m_modifyStr;
    const QString m_reportDataStr;
    QSqlDatabase m_db;
    QList<TcpClientSocket *> m_clientList;
};

#endif // SUBTCPSERVER_H
