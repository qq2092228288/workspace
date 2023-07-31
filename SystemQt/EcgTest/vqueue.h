#ifndef VQUEUE_H
#define VQUEUE_H

#include <QQueue>

template <class T>
class VQueue : public QQueue<T>
{
public:
    VQueue(const int &max = INT_MAX) : m_max(max) {}
    void enqueue(const T &v)
    {
        if (this->length() >= m_max) this->dequeue();
        QQueue<T>::enqueue(v);
    }
    int maxcount() const
    {
        return m_max;
    }
private:
    int m_max;
};

#endif // VQUEUE_H
