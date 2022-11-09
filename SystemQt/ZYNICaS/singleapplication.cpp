#include "singleapplication.h"
#include "mainwidget.h"
#include <QMessageBox>
#include <QEventLoop>

SingleApplication::SingleApplication(int argc, char *argv[])
    : QApplication{argc, argv},
      mainWidget{nullptr},
      running{false},
      localServer{nullptr}
{
    serverName = QFileInfo(QCoreApplication::applicationFilePath()).absoluteFilePath();
    initLocalConnection();
}

bool SingleApplication::isRunning() const
{
    return running;
}

void SingleApplication::newLocalConnection()
{
    QLocalSocket *socket = localServer->nextPendingConnection();
    if (!socket)
        return;
    socket->waitForReadyRead(1000);
    QTextStream stream(socket);
    //其他处理
    delete socket;
    if (mainWidget != NULL)
    {
        //激活窗口
        mainWidget->raise();
        mainWidget->activateWindow();
//        mainWidget->setWindowState((mainWidget->windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
        QMessageBox::warning(mainWidget, tr("提示"), tr("程序正在运行中！！！"));
    }
}

void SingleApplication::initLocalConnection()
{
    running = false;
    QLocalSocket socket;
    socket.connectToServer(serverName);
    if(socket.waitForConnected(500))
    {
        running = true;
        // 其他处理，如：将启动参数发送到服务端
        QTextStream stream(&socket);
        QStringList args = QCoreApplication::arguments();
        if (args.count() > 1)
            stream << args.last();
        else
            stream << QString();
        stream.flush();
        socket.waitForBytesWritten();

        return;
    }
    //连接不上服务器，就创建一个
    newLocalServer();
}

void SingleApplication::newLocalServer()
{
    localServer = new QLocalServer(this);
    connect(localServer, SIGNAL(newConnection()), this, SLOT(newLocalConnection()));
    if(!localServer->listen(serverName))
    {
        // 此时监听失败，可能是程序崩溃时,残留进程服务导致的,移除之
        if(localServer->serverError() == QAbstractSocket::AddressInUseError)
        {
            QLocalServer::removeServer(serverName); // <-- 重点
            localServer->listen(serverName); // 再次监听
        }
    }
}
