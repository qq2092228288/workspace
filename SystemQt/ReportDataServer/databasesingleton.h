#ifndef DATABASESINGLETON_H
#define DATABASESINGLETON_H

#include <QObject>
#include <QSqlDatabase>
#include <QMutexLocker>

class DatabaseSingleton : public QObject
{
    Q_OBJECT
public:
    void openDatabase();
    QSqlDatabase db();
private:
    QMutex mutex;
    QSqlDatabase m_db;
private:
    explicit DatabaseSingleton();
    virtual ~DatabaseSingleton();
    DatabaseSingleton(const DatabaseSingleton &) = delete;
    DatabaseSingleton(const DatabaseSingleton &&) = delete;
    DatabaseSingleton &operator=(const DatabaseSingleton &) = delete;
public:
    static DatabaseSingleton *getInstance();
};

#endif // DATABASESINGLETON_H
