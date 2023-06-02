#include "threadserivce.h"


QThread *ThreadSerivce::getThread()
{
    QMutexLocker locker(&mutex);
    foreach (auto thread, threadList) {
        if (!thread->isRunning()) {
            return thread;
        }
    }
    QThread *thread = new QThread;
    threadList.append(thread);
    return thread;
}

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
        if (threadList.count() >= maxThreads) {
            emit error(QString("The maximum number of threads has been reached."));
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

int ThreadSerivce::getMaxThreads() const
{
    return maxThreads;
}

bool ThreadSerivce::setMaxThreads(int max)
{
    QMutexLocker locker(&mutex);
    if (threadList.count() > max)
        return false;
    maxThreads = max;
    return true;
}

void ThreadSerivce::objectDestroyed(QObject *obj)
{
    QMutexLocker locker(&mutex);
    auto thread = obj->thread();
    thread->quit();
    thread->wait();
}

ThreadSerivce::ThreadSerivce()
    : maxThreads{1024}
{

}

ThreadSerivce::~ThreadSerivce()
{
    foreach (auto thread, threadList) {
        threadList.removeOne(thread);
        thread->quit();
        thread->wait();
        delete thread;
        thread = nullptr;
    }
}

ThreadSerivce &ThreadSerivce::getInstance()
{
    return instance;
}

