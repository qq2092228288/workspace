#include "threadserivce.h"


bool ThreadSerivce::objectMoveToThread(QObject *object)
{
    QMutexLocker locker(&mutex);
    QThread *thread = nullptr;
    foreach (auto index, threadList) {
        if (nullptr == index && !index->isRunning()) {
            thread = index;
            break;
        }
    }
    if (nullptr == thread) {
        if (threadList.count() >= maxcount) {
            return false;
        }
        thread = new QThread;
        threadList.append(thread);
    }
    connect(object, &QObject::destroyed, this, &ThreadSerivce::objectDestroyed);
    object->moveToThread(thread);
    thread->start();
    return true;
}

bool ThreadSerivce::setMaxcount(int maxcount)
{
    QMutexLocker locker(&mutex);
    if (threadList.count() > maxcount)
        return false;
    this->maxcount = maxcount;
    return true;
}

void ThreadSerivce::setMaxIdleThreadCount(int maxIdle)
{
    this->maxIdle = maxIdle;
}

int ThreadSerivce::count() const
{
    return threadList.count();
}

void ThreadSerivce::objectDestroyed(QObject *obj)
{
    QMutexLocker locker(&mutex);
    obj->thread()->quit();
    obj->thread()->wait();
    int idleCount = 0;
    foreach (auto thread, threadList) {
        if (!thread->isRunning()) {
            ++idleCount;
            if (idleCount > maxIdle) {
                threadList.removeOne(thread);
                delete thread;
                thread = nullptr;
            }
        }
    }
}

ThreadSerivce::ThreadSerivce()
    : maxcount{1024},
      maxIdle{32}
{

}

ThreadSerivce::~ThreadSerivce()
{
    foreach (auto thread, threadList) {
        thread->quit();
        thread->wait();
        delete thread;
        thread = nullptr;
    }
    threadList.clear();
}

ThreadSerivce &ThreadSerivce::getInstance()
{
    return instance;
}

