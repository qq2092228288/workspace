#ifndef WAVEFORM_H
#define WAVEFORM_H

#include <QtCharts>
#include <QChartView>
#include <QDateTimeAxis>
#include <QValueAxis>
#include <QSplineSeries>
#include <QGridLayout>

#include "vqueue.h"

class Waveform : public QChartView
{
    Q_OBJECT
public:
    explicit Waveform(const QString &title, QWidget *parent = nullptr);
public slots:
    void append(double value);
    void setSpeed(int speed);
    void clear();
signals:
private:
    QValueAxis *xAxis;
    QValueAxis *yAxis;
    QSplineSeries *series;
private:
    VQueue<QPointF> vqueue;
    int count;
    int speed;
};

#endif // WAVEFORM_H
