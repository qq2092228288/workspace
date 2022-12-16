#ifndef TRENDCHARTSWIDGET_H
#define TRENDCHARTSWIDGET_H

#include <QWidget>
#include <QHBoxLayout>
#include <QGridLayout>

class TrendChartsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TrendChartsWidget(QWidget *parent = nullptr);
    void changeShow(const QString &current, const QString &change);
public slots:
    void widgetShow();
signals:
private:
    QGridLayout *dataGLayout;
    QList<QHBoxLayout *> hLayouts;
};

#endif // TRENDCHARTSWIDGET_H
