#include <QCoreApplication>
#include "serverbackend.h"


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    ServerBackend_PTR server(new ServerBackend);
    auto ptr = server.data();
    ptr->connectToHost("nicas.zeyaotebco.com", 1883);
    return a.exec();
}
