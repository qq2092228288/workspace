#ifndef THREADSERIVCE_H
#define THREADSERIVCE_H

#include <QObject>
#include <QThread>
#include <QMutexLocker>
#include <QDebug>

class ThreadSerivce : public QObject
{
    Q_OBJECT
public:
    QThread *getThread();
    bool objectMoveToThread(QObject *object);
    int getMaxThreads() const;
    bool setMaxThreads(int max);
private slots:
    void objectDestroyed(QObject *obj);
private:
    QMutex mutex;
    int maxThreads;
    QList<QThread *> threadList;
signals:
    void error(QString);
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
