#include "clientworker.h"
#include "tcpclientsocket.h"

ClientWorker::ClientWorker(qintptr socketDescriptor)
    : m_socketDescriptor{socketDescriptor}
{

}

void ClientWorker::run()
{
    // TcpClientSocket client(m_socketDescriptor);
    // connect(&client, &TcpClientSocket::disconnected, this, &ClientWorker::quit);
    // exec();
}
