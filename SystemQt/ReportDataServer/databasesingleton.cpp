#include "databasesingleton.h"
#include "singleton.h"
#include <QSqlError>

void DatabaseSingleton::openDatabase()
{
    if (!m_db.isOpen()) {
        m_db = QSqlDatabase::addDatabase(Singleton::databaseType(),
                                         QUuid::createUuid().toString().remove("{").remove("}").remove("-"));
        m_db.setHostName(Singleton::hostname());
        m_db.setPort(Singleton::databasePort());
        m_db.setDatabaseName(Singleton::databaseName());
        m_db.setUserName(Singleton::userName());
        m_db.setPassword(Singleton::password());
        if (m_db.open()) {
            TIME_DEBUG()<<"Database opened successfully.";
        }
        else {
            TIME_DEBUG()<<m_db.lastError();
        }
    }
}

QSqlDatabase DatabaseSingleton::db()
{
    return m_db;
}

DatabaseSingleton::DatabaseSingleton()
{

}

DatabaseSingleton::~DatabaseSingleton()
{
    if (m_db.isOpen()) {
        m_db.close();
    }
}

DatabaseSingleton *DatabaseSingleton::getInstance()
{
    static DatabaseSingleton instance;
    return &instance;
}



