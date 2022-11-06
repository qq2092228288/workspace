#ifndef SINGLEAPPLICATION_H
#define SINGLEAPPLICATION_H

#include <QApplication>
#include <QtNetwork/QLocalServer>
#include <QtNetwork/QLocalSocket>

class MainWidget;

class SingleApplication : public QApplication
{
    Q_OBJECT
public:
    SingleApplication(int argc, char *argv[]);
    bool isRunning() const;
    MainWidget *mainWidget;
private slots:
    void newLocalConnection();
private:
    void initLocalConnection();
    void newLocalServer();
    bool running;
    QLocalServer *localServer;
    QString serverName;
};

#endif // SINGLEAPPLICATION_H
