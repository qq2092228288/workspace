#ifndef TRENDCHARTSWIDGET_H
#define TRENDCHARTSWIDGET_H

#include <QWidget>
#include <QHBoxLayout>
#include <QGridLayout>
#include "datatype.h"

class TrendChart;
class TrendChartsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TrendChartsWidget(QWidget *parent = nullptr);
    void changeShow(const Type &current, const Type &change);
public slots:
    void widgetShow();
signals:
private:
    QGridLayout *dataGLayout;
//    QList<QHBoxLayout *> hLayouts;
    QList<TrendChart *> trendCharts;
};

#endif // TRENDCHARTSWIDGET_H
