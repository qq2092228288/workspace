#ifndef THREADSERIVCE_H
#define THREADSERIVCE_H

#include <QObject>
#include <QThread>
#include <QMutexLocker>

class ThreadSerivce : public QObject
{
    Q_OBJECT
public:
    bool objectMoveToThread(QObject *object);
    bool setMaxcount(int maxcount);
    void setMaxIdleThreadCount(int maxIdle);
    int count() const;
private slots:
    void objectDestroyed(QObject *obj);
private:
    QMutex mutex;
    int maxcount;
    int maxIdle;
    QList<QThread *> threadList;
private:
    explicit ThreadSerivce();
    virtual ~ThreadSerivce();
    ThreadSerivce(const ThreadSerivce &) = delete;
    ThreadSerivce &operator=(const ThreadSerivce &) = delete;
private:
    static ThreadSerivce instance;
public:
    static ThreadSerivce &getInstance();
};

#endif // THREADSERIVCE_H
