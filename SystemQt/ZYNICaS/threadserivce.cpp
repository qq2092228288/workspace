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

void ThreadSerivce::objectMoveToThread(QObject *object)
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
        thread = new QThread;
        threadList.append(thread);
    }
    object->moveToThread(thread);
    thread->start();
}

ThreadSerivce::ThreadSerivce()
{

}

ThreadSerivce::~ThreadSerivce()
{
    foreach (auto thread, threadList) {
        thread->quit();
        thread->wait();
        delete thread;
    }
}

ThreadSerivce &ThreadSerivce::getInstance()
{
    return instance;
}

