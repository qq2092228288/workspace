#ifndef NICASDATAFILTERWIDGET_H
#define NICASDATAFILTERWIDGET_H

#include <QStackedWidget>
#include <QMap>
#include <QRadioButton>
#include <QVBoxLayout>
#include <QGroupBox>
#include "datanfilterwidgetns.h"
// #include "datatype.h"

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
    // QMap<Type, DatanFilterWidgetNs::NicasFilter *> map;
    QVector<DatanFilterWidgetNs::NicasFilter *> m_filters;
};

#endif // NICASDATAFILTERWIDGET_H
