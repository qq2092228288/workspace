#ifndef WAVEFORM_H
#define WAVEFORM_H

#include <QtCharts>
#include <QChartView>
#include <QDateTimeAxis>
#include <QValueAxis>
#include <QSplineSeries>
#include <QGridLayout>
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

class Waveform : public QChartView
{
    Q_OBJECT
public:
    explicit Waveform(const QString &title, QWidget *parent = nullptr);
public slots:
    void clear();
    void newdata(const short &value);
private:
    QValueAxis *xAxis;
    QValueAxis *yAxis;
    QSplineSeries *series;
private:
    VQueue<QPointF> vqueue;
    int count;
};

#endif // WAVEFORM_H
