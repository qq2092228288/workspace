#ifndef BASEARGWIDGETSINST_H
#define BASEARGWIDGETSINST_H

#include <QObject>
#include <QList>

#include "baseargwidget.h"

class BaseArgWidgetsInst : public QObject
{
    Q_OBJECT
public:
    explicit BaseArgWidgetsInst(QObject *parent = nullptr);
    virtual ~BaseArgWidgetsInst();
    QList<BaseArgWidget *> *getBaseArgWidgetList();
public slots:
    void clearArgWidgets();
signals:
protected:
    void appendWidget(const QString &secondary, const QString &main, const QString &unit,
                      const QString &scope, const int &digit);
    void appendWidget(const QString &secondary, const QString &main, const QString &unit,
                      const qreal &min, const qreal &max, const int &digit);
private slots:
    void mouseDoubleClickedWidget();
private:
    QList<BaseArgWidget *> m_pWidgets;
};

#endif // BASEARGWIDGETSINST_H
