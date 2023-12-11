#ifndef SUBTCPSERVER_H
#define SUBTCPSERVER_H

#include <QObject>
#include <QSqlDatabase>
#include "tcpclientsocket.h"

class SubTcpServer : public QObject
{
    Q_OBJECT
public:
    explicit SubTcpServer(QObject *parent = nullptr);
    int count() const;
public slots:
    void newSocketDescriptor(qintptr socketDescriptor);
signals:
    void writeReady(qintptr socketDescriptor, const QByteArray &data);
private slots:
    void received(qintptr socketDescriptor, TelegramType type, const QByteArray &data);
    void clientDeleted(QObject *client);
private:
    template <class T>
    void sendWrite(qintptr socketDescriptor, TelegramType type, const T &json);
private:
    QSqlDatabase m_db;
    QList<TcpClientSocket *> m_clientList;
};

#endif // SUBTCPSERVER_H