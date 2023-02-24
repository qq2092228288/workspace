#include "isicurvewidget.h"
#include <QtMath>

IsiCurveWidget::IsiCurveWidget(QWidget *parent)
    : QWidget{parent}
{
    this->setFixedSize(300, 120);
    this->setStyleSheet("IsiCurveWidget{background-color:#ffffff;}");
}

void IsiCurveWidget::setIsi(const qreal &oldIsi, const qreal &newIsi)
{
    m_oldIsi = oldIsi;
    m_newIsi = newIsi;
}

void IsiCurveWidget::clear()
{
    m_oldIsi = 0;
    m_newIsi = 0;
}

void IsiCurveWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    const QPoint point(20, height() - 10);
    int len = width() - 40;
    QPainter painter(this);
    // 消除锯齿
    painter.setRenderHint(QPainter::Antialiasing, true);
    // 画笔颜色，宽度
    painter.setPen(QPen(QColor(Qt::black), 1.5));
    // 坐标轴
//    drawArrow(painter, point, QPoint(width() - 10, point.y()));
    painter.drawLine(QLine(point, QPoint(width() - 10, point.y())));
    painter.drawLine(QLine(point, QPoint(point.x(), 10)));
    painter.drawText(QPointF(30, 20), "ISI");
    // 画弧线
//    painter.setPen(QPen(QColor(Qt::blue), 2));
    QList<QPointF> curvePoints;
    for (int x = 15; x < len; ++x) {
//        qreal y = -13.0/1440*qPow(x, 2) + 91.0/36*x - 485.0/18;
        curvePoints.append(QPointF(point.x() + x, point.y() - gety(x)));
    }
    QPainterPath path;
    path.moveTo(curvePoints.at(0));
    foreach (const QPointF &point, curvePoints) {
        path.lineTo(point);
    }
    painter.drawPath(path);
    // ISI的点
//    painter.setPen(QPen(QColor(Qt::red), 2));
    if (m_oldIsi != 0 && m_newIsi != 0) {
        const int radius = 2;
        const qreal zoom = 39.0625;
        QPointF oldPoint(point.x() + getx(m_oldIsi*zoom, true), point.y() - m_oldIsi*zoom);
        QPointF newPoint(point.x() + getx(m_newIsi*zoom, false), point.y() - m_newIsi*zoom);
        painter.drawEllipse(oldPoint, radius, radius);
        painter.drawEllipse(newPoint, radius, radius);
        QFont font = painter.font();
        font.setPixelSize(12);
        painter.setFont(font);
        painter.drawText(QPointF(oldPoint.x() - 20, oldPoint.y() - 10), QString::number(m_oldIsi));
        painter.drawText(QPointF(newPoint.x(), newPoint.y() - 10), QString::number(m_newIsi));
        drawArrow(painter, oldPoint, newPoint);
    }
}

qreal IsiCurveWidget::gety(const qreal &x) const
{
//    return (-13.0/1440*qPow(x, 2) + 91.0/36*x - 485.0/18);
    return (-qPow(x, 2)/180.0 + 14/9.0*x - 80/9.0);
}

qreal IsiCurveWidget::getx(const qreal &y, bool isOld) const
{
//    if (y > 150)return -1; // 负值求根
//    qreal temp = qSqrt(19600 - (1440*y + 38800)/13.0);
//    if (isOld)  return (140 - temp);
//    return (140 + temp);
    if (y > 100) return -1;
    return (isOld ? 140 - qSqrt(18000 - 180*y) : 140 + qSqrt(18000 - 180*y));
}

void IsiCurveWidget::drawArrow(QPainter &painter, const QPointF &point1, const QPointF &point2)
{
    qreal arrowSize = 10;
    QLineF line(point2, point1);

    double angle = std::atan2(-line.dy(), line.dx());
    QPointF arrowP1 = line.p1() + QPointF(sin(angle + M_PI / 3) * arrowSize,
                                          cos(angle + M_PI / 3) * arrowSize);
    QPointF arrowP2 = line.p1() + QPointF(sin(angle + M_PI - M_PI / 3) * arrowSize,
                                          cos(angle + M_PI - M_PI / 3) * arrowSize);

    QPolygonF arrowHead;
    arrowHead.clear();
    arrowHead << line.p1() << arrowP1 << arrowP2;
    painter.drawLine(line);
    painter.drawPolygon(arrowHead);
}
