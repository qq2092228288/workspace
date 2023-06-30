#include <QCoreApplication>
#include <qtsinglecoreapplication.h>
#include <QSqlDatabase>
#include <singleton.h>
#include "htmlserver.h"

int main(int argc, char *argv[])
{
    QtSingleCoreApplication a("zeyao management system", argc, argv);
    if (a.isRunning()) {
        TIME_DEBUG()<<"The program is currently running.";
        return EXIT_SUCCESS;
    }
    // init database
    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL");
    db.setHostName(Singleton::hostname());
    db.setPort(Singleton::databasePort());
    db.setDatabaseName(Singleton::databaseName());
    db.setUserName(Singleton::userName());
    db.setPassword(Singleton::password());
    // open database
    if (db.open()) {
        TIME_DEBUG()<<"Database opened successfully.";
    }
    else {
        TIME_DEBUG()<<db.lastError();
        return -1;
    }
    Singleton::getInstance()->setDatabase(&db);
    // init html server
    HtmlServer_PTR server(new HtmlServer);
    server->startListening();
    return a.exec();
}
