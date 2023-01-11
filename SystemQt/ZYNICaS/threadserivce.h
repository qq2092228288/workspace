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
    void objectMoveToThread(QObject *object);
public slots:

private:
    QMutex mutex;
    QList<QThread *> threadList;
private slots:

signals:

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
