#ifndef NICASDATAFILTERWIDGET_H
#define NICASDATAFILTERWIDGET_H

#include <QStackedWidget>
#include <QMap>
#include <QRadioButton>
#include <QGridLayout>
#include <QGroupBox>
#include "datanfilterwidgetns.h"

class NicasDataFilterWidget : public QWidget
{
    Q_OBJECT
public:
    explicit NicasDataFilterWidget(QWidget *parent = nullptr);
    QVector<DatanFilterWidgetNs::NicasFilter *> filters();
public slots:
    void init();
signals:
private:
    QButtonGroup *buttonGroup;
    QStackedWidget *widget;
    QVector<DatanFilterWidgetNs::NicasFilter *> m_filters;
};

#endif // NICASDATAFILTERWIDGET_H
