#include <qtsinglecoreapplication.h>
#include "tcpserver.h"
#include "config.h"

int main(int argc, char *argv[])
{
    QtSingleCoreApplication a(argc, argv);
    if (a.isRunning()) {
        return EXIT_SUCCESS;
    }
    TcpServer server(LISTEN_PORT, MAX_NUM_CONNECTION);
    return a.exec();
}
