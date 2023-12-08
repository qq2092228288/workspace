#ifndef CLIENTWORKER_H
#define CLIENTWORKER_H

#include <QThread>
#include <QTcpSocket>

class ClientWorker : public QThread
{
    Q_OBJECT
public:
    explicit ClientWorker(qintptr socketDescriptor);

signals:
private:
    void run();
private:
    qintptr m_socketDescriptor;
};

#endif // CLIENTWORKER_H
