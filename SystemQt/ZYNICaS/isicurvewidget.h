#ifndef ISICURVEWIDGET_H
#define ISICURVEWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QPoint>
#include <QPainterPath>
#include <QPolygonF>

class IsiCurveWidget : public QWidget
{
    Q_OBJECT
public:
    explicit IsiCurveWidget(QWidget *parent = nullptr);
public slots:
    // set isi value
    void setIsi(const qreal &oldIsi, const qreal &newIsi);
    void setSv(const qreal &oldSv, const qreal &newSv);
    // init isi value
    void clear();
protected:
    virtual void paintEvent(QPaintEvent *event);
    qreal gety(const qreal &x) const;
    qreal getx(const qreal &y, bool isOld) const;
    void drawArrow(QPainter &painter, const QPointF &point1, const QPointF &point2);
signals:
private:
    qreal m_oldIsi;
    qreal m_newIsi;
    qreal m_oldSv;
    qreal m_newSv;
};

#endif // ISICURVEWIDGET_H
