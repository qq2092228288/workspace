#include <qtsinglecoreapplication.h>
#include "tcpserver.h"

int main(int argc, char *argv[])
{
    QtSingleCoreApplication a(argc, argv);
    if (a.isRunning()) {
        return EXIT_SUCCESS;
    }
    TcpServer server;
    return a.exec();
}
