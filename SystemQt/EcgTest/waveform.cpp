#include "waveform.h"

bool pred(QPointF f, QPointF s)
{
    return f.y() < s.y();
}

Waveform::Waveform(const QString &title, QWidget *parent)
    : QChartView{parent},
      vqueue{VQueue<QPointF>(500)},
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

    yAxis->setLabelFormat("%.1f mV");
    yAxis->setRange(-1000, 1000);
    yAxis->setTickCount(4);
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
        yAxis->setRange(pair.first->y(), pair.second->y());
        series->replace(vqueue);
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
