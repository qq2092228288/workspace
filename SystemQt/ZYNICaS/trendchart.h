#ifndef TRENDCHART_H
#define TRENDCHART_H

#include <QChartView>
#include <QChart>
#include <QtCharts>
#include <QValueAxis>
#include <QLineSeries>
#include <QDateTimeAxis>
#include <QWidget>
#include <QGridLayout>
#include <QLabel>

#include "selectitemdialog.h"
class CustomCtrl;
class TrendChart : public QWidget
{
    Q_OBJECT
public:
    explicit TrendChart(CustomCtrl *customCtrl, QWidget *parent = nullptr);
    virtual ~TrendChart();
    void mouseDoubleClickEvent(QMouseEvent *event);
public slots:
    void clear();
    void addValue(double value);
    void getChangeText(const QString &text);
signals:
    void changeName(const QString &, const QString &);
private:
    QLabel *m_pLabel;
    QChartView *m_pChartView;
    QList<double> m_values;
    QDateTimeAxis *m_pAxisX;
    QValueAxis *m_pAxisY;
    QLineSeries *m_pSeries;
    bool setMinX = true;
    SelectItemDialog *dialog;
    int digit;
private:
    CustomCtrl *m_pCustomCtrl;
};

#endif // TRENDCHART_H
