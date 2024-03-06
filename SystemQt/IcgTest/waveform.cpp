#include "waveform.h"

bool pred(QPointF f, QPointF s)
{
    return f.y() < s.y();
}

Waveform::Waveform(const QString &title, QWidget *parent)
    : QChartView{parent}
    , vqueue{VQueue<QPointF>(500)}
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
    xAxis->setRange(0, vqueue.maxcount());
    xAxis->setTickCount(vqueue.maxcount() / 100 + 1);
    xAxis->setGridLinePen(QPen(Qt::red, 1, Qt::SolidLine));
    xAxis->setMinorTickCount(9);
    xAxis->setMinorGridLinePen(QPen(Qt::red, 0.2, Qt::SolidLine));

    // yAxis->setLabelsVisible(false);
    yAxis->setLabelFormat("%d uV");
    yAxis->setRange(-1000, 1000);
    yAxis->setTickCount(3);
    yAxis->setGridLinePen(QPen(Qt::red, 1, Qt::SolidLine));
    yAxis->setMinorTickCount(9);
    yAxis->setMinorGridLinePen(QPen(Qt::red, 0.2, Qt::SolidLine));

    series->attachAxis(xAxis);
    series->attachAxis(yAxis);
    // series->setUseOpenGL(true);
    clear();
}

void Waveform::clear()
{
    vqueue.clear();
    count = 0;
    series->clear();
}

void Waveform::newdata(const short &value)
{
#if 1
    vqueue.enqueue(QPointF(count++, value));
    if (count%5 == 0) {
        xAxis->setRange(vqueue.first().x(), count);
        series->replace(vqueue);
    }
#else

#endif
}
