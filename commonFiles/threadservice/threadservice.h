#ifndef THREADSERIVCE_H
#define THREADSERIVCE_H

#include <QObject>
#include <QThread>
#include <QMutexLocker>

class ThreadService : public QObject
{
    Q_OBJECT
public:
    QThread *objectMoveToThread(QObject *object);
    bool setMaxcount(int count);
    void setMaxIdle(int count);
    qint32 currentCount() const;
    qint32 maxcount() const;
    qint32 maxIdle() const;
private Q_SLOTS:
    void checkIdleCount();
Q_SIGNALS:
    void destroyedObjectSpace(QObject *obj);
private:
    QMutex mutex;
    qint32 m_maxcount;
    qint32 m_maxIdle;
    QList<QThread *> threadList;
private:
    explicit ThreadService();
    virtual ~ThreadService();
    ThreadService(const ThreadService &) = delete;
    ThreadService(const ThreadService &&) = delete;
    ThreadService &operator=(const ThreadService &) = delete;
public:
    static ThreadService *getInstance();
};

#endif // THREADSERIVCE_H
