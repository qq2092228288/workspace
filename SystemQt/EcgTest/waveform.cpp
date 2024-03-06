#include "waveform.h"

bool pred(QPointF f, QPointF s)
{
    return f.y() < s.y();
}

Waveform::Waveform(const QString &title, QWidget *parent)
    : QChartView{parent},
      vqueue{VQueue<QPointF>(1000)},
      speed{100}
{
    xAxis = new QValueAxis(this);
    yAxis = new QValueAxis(this);
    series = new QSplineSeries(this);

    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

    chart()->setTitle(title);
    chart()->addSeries(series);
    chart()->addAxis(xAxis, Qt::AlignBottom);
    chart()->addAxis(yAxis, Qt::AlignLeft);
    chart()->legend()->hide();
    chart()->setContentsMargins(-10, -20, -10, -30);
    chart()->layout()->setContentsMargins(0, 0, 0, 0);
    setRenderHint(QPainter::Antialiasing);


    xAxis->setLabelsVisible(false);
    xAxis->setRange(0, 1000);
    xAxis->setTickCount(6);
    xAxis->setGridLinePen(QPen(Qt::red, 0.6, Qt::SolidLine));
    xAxis->setMinorTickCount(4);
    xAxis->setMinorGridLinePen(QPen(Qt::red, 0.3, Qt::SolidLine));

    yAxis->setLabelFormat("%d mV");
    yAxis->setRange(-1, 1);
    yAxis->setTickCount(3);
    yAxis->setGridLinePen(QPen(Qt::red, 0.6, Qt::SolidLine));
    yAxis->setMinorTickCount(4);
    yAxis->setMinorGridLinePen(QPen(Qt::red, 0.3, Qt::SolidLine));

    series->attachAxis(xAxis);
    series->attachAxis(yAxis);
    series->setUseOpenGL(true);

    clear();
}

void Waveform::append(double value)
{
    vqueue.enqueue(QPointF(count++, value));
    if (count%speed == 0) {
        xAxis->setRange(vqueue.first().x(), count);
        auto pair = std::minmax_element(vqueue.begin(), vqueue.end(), pred);
//        auto offset = abs((pair.second->y() - pair.first->y()) / 3);
//        yAxis->setRange(pair.first->y() - offset, pair.second->y() + offset);
#if 0
        yAxis->setRange(pair.first->y(), pair.second->y());
        series->replace(vqueue);
#else
        auto average = (pair.first->y() + pair.second->y()) / 2.0;
        yAxis->setRange(average - 2, average + 2);
        series->replace(vqueue);
        // if (average != std::floor(pair.second->y())) {
        //     average += 1;
        // }
        // auto average = std::floor(pair.second->y());
        // QList<QPointF> points;
        // foreach (auto point, vqueue) {
        //     points.append(QPointF(point.x(), point.y() - average));
        // }
        // series->replace(points);
#endif
    }
}

void Waveform::setSpeed(int speed)
{
    this->speed = speed;
}

void Waveform::clear()
{
    series->clear();
    vqueue.clear();
    count = 0;
}
