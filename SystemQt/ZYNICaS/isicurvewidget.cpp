#include "isicurvewidget.h"
#include <QtMath>

IsiCurveWidget::IsiCurveWidget(QWidget *parent)
    : QWidget{parent}
{
    this->setFixedSize(300, 150);
    this->setStyleSheet("IsiCurveWidget{background-color:#ffffff;}");
}

void IsiCurveWidget::setIsi(const qreal &oldIsi, const qreal &newIsi)
{
    m_oldIsi = oldIsi;
    m_newIsi = newIsi;
}

void IsiCurveWidget::setSv(const qreal &oldSv, const qreal &newSv)
{
    m_oldSv = oldSv;
    m_newSv = newSv;
}

void IsiCurveWidget::clear()
{
    m_oldIsi = 0;
    m_newIsi = 0;
    m_oldSv = 0;
    m_newSv = 0;
}

void IsiCurveWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    const QPoint point(20, height() - 40);
    int len = width() - 40;
    QPainter painter(this);
    // 消除锯齿
    painter.setRenderHint(QPainter::Antialiasing, true);
    // 画笔颜色，宽度
    painter.setPen(QPen(QColor(Qt::black), 1.5));
    // 坐标轴
    painter.drawLine(QLine(point, QPoint(width() - 10, point.y())));
    painter.drawLine(QLine(point, QPoint(point.x(), 10)));
    // 垂直字体
    painter.drawText(5, 30, 20, 100, Qt::AlignTop | Qt::TextSingleLine | Qt::TextWordWrap, tr("心脏泵力"));
    // 画弧线
    QList<QPointF> curvePoints;
    for (int x = 15; x < len; ++x) {
        curvePoints.append(QPointF(point.x() + x, point.y() - gety(x)));
    }
    QPainterPath path;
    path.moveTo(curvePoints.at(0));
    foreach (const QPointF &point, curvePoints) {
        path.lineTo(point);
    }
    painter.drawPath(path);
    // ISI的点
    if (m_oldIsi != 0 && m_newIsi != 0) {
        qreal op, np;
        QString xText("回心血量增加后，泵力");
        if (m_oldIsi < m_newIsi) {
            op = 50;
            np = 100;
            xText += tr("增加");
        }
        else if (m_oldIsi == m_newIsi) {
            op = 75;
            np = 75;
            xText += tr("不变");
        }
        else {
            op = 100;
            np = 50;
            xText += tr("减少");
        }
        xText += tr("，搏排量");
        if (m_oldSv < m_newSv) {
            xText += tr("增加");
        }
        else if (m_oldSv == m_newSv) {
            xText += tr("不变");
        }
        else {
            xText += tr("减少");
        }
        painter.drawText(QPointF(point.x() + 110, point.y() + 15), tr("容量"));
        painter.drawText(QPointF(point.x(), point.y() + 30), xText);
        QPointF oldPoint(point.x() + getx(op, true), point.y() - op);
        QPointF newPoint(point.x() + getx(np, false), point.y() - np);
        const int radius = 2;
        painter.drawEllipse(oldPoint, radius, radius);
        painter.drawEllipse(newPoint, radius, radius);
        painter.drawPoint(QPointF(point.x() + getx(op, true), point.y() - 2));
        painter.drawPoint(QPointF(point.x() + getx(np, false), point.y() - 2));
        const qreal cx = qSqrt(75);
        if (op == np) {
            drawArrow(painter, QPointF(oldPoint.x() + 10, oldPoint.y()), QPointF(newPoint.x() - 10, newPoint.y()));
        }
        else if (op > np) {
            drawArrow(painter, QPointF(oldPoint.x() + cx, oldPoint.y() + 5), QPointF(newPoint.x() - cx, newPoint.y() - 5));
        }
        else {
            drawArrow(painter, QPointF(oldPoint.x() + cx, oldPoint.y() - 5), QPointF(newPoint.x() - cx, newPoint.y() + 5));
        }
    }
}

qreal IsiCurveWidget::gety(const qreal &x) const
{
    return (-qPow(x, 2)/180.0 + 14/9.0*x - 80/9.0);
}

qreal IsiCurveWidget::getx(const qreal &y, bool isOld) const
{
    if (y > 100) return -1;
    return (isOld ? 140 - qSqrt(18000 - 180*y) : 140 + qSqrt(18000 - 180*y));
}

void IsiCurveWidget::drawArrow(QPainter &painter, const QPointF &point1, const QPointF &point2)
{
    qreal arrowSize = 8;
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
