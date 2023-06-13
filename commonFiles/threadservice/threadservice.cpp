#include "threadservice.h"
#include <QDebug>

/*!
 * @class ThreadService
 * Use the Singleton pattern to uniformly allocate and manage the threads of the project's classes
 * that inherit from QObject and need to be executed in the threads.
*/

/*!
 * Object moved to thread run.
 * If the current number of threads is less than the maximum number of threads or if there are idle threads,
 * return true; else return false
*/
QThread *ThreadService::objectMoveToThread(QObject *object)
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
        if (threadList.count() >= m_maxcount) {
            return nullptr;
        }
        thread = new QThread;
        threadList.append(thread);
    }
    connect(object, &QObject::destroyed, thread, &QThread::quit);
    connect(thread, &QThread::finished, this, &ThreadService::checkIdleCount);
    object->moveToThread(thread);
    thread->start();
    return thread;
}
/*!
 * Set maximum number of threads.
 * If the current number of threads is less than maxcount, return true; else return false.
*/
bool ThreadService::setMaxcount(int count)
{
    QMutexLocker locker(&mutex);
    if (threadList.count() > count)
        return false;
    m_maxcount = count;
    return true;
}
/*!
 * Set max idle number of threads.
*/
void ThreadService::setMaxIdle(int count)
{
    m_maxIdle = count;
}
/*!
 * Returns the current number of threads.
*/
qint32 ThreadService::currentCount() const
{
    return threadList.count();
}
/*!
 * Returns the m_maxcount.
*/
qint32 ThreadService::maxcount() const
{
    return m_maxcount;
}
/*!
 * Returns the m_maxIdle.
*/
qint32 ThreadService::maxIdle() const
{
    return m_maxIdle;
}
/*!
 * After the thread is finished, check the number of idle threads.
*/
void ThreadService::checkIdleCount()
{
    QMutexLocker locker(&mutex);
    int idleCount = 0;
    foreach (auto thread, threadList) {
        if (!thread->isRunning()) {
            if (idleCount <= m_maxIdle) {
                ++idleCount;
            }
            else {
                threadList.removeOne(thread);
                delete thread;
                thread = nullptr;
            }
        }
    }
}
/*!
 * Private constructor, initializing m_maxcount and m_maxIdle parameter.
*/
ThreadService::ThreadService()
    : m_maxcount{1024},
      m_maxIdle{32}
{

}
/*!
 * Private destructor, freeing threads space.
*/
ThreadService::~ThreadService()
{
    foreach (auto thread, threadList) {
        thread->quit();
        thread->wait();
        delete thread;
        thread = nullptr;
    }
    threadList.clear();
}
/*!
 * Return ThreadService class local static instance.
*/
ThreadService *ThreadService::getInstance()
{
    static ThreadService instance;
    return &instance;
}

