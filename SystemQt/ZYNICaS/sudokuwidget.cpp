#include "sudokuwidget.h"
#include <QGuiApplication>
#include <QScreen>
#include <iostream>
#include <math.h>


SudokuWidget::SudokuWidget(const SuBArg &suBArg, QWidget *parent)
    : QWidget{parent},m_suBArg{suBArg}
{
    setMinimumSize(210*winScale(), 220*winScale());
}

void SudokuWidget::clear()
{
    m_suBArg.points.clear();
}

void SudokuWidget::setMapScope(const double &min, const double &max)
{
    m_suBArg.map = ArgScope(min, max);
}

void SudokuWidget::setSiScope(const double &min, const double &max)
{
    m_suBArg.si = ArgScope(min, max);
}

void SudokuWidget::setSsvriScope(const double &min, const double &max)
{
    m_suBArg.ssvri = ArgScope(min, max);
}

void SudokuWidget::setLswiScope(const double &min, const double &max)
{
    m_suBArg.lswi = ArgScope(min, max);
}

void SudokuWidget::setPoint(const double &map, const double &si, const SignType &type, const bool &firstPos)
{
    if (firstPos) {
        m_suBArg.points.clear();
    }
    else if (m_suBArg.points.size() == 2)  {
        m_suBArg.points.removeLast();
    }
    m_suBArg.points<<DataPoint(map, si, type);
}

void SudokuWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    auto burush = painter.brush();
    painter.setBrush(QColor(255, 255, 255));
    painter.setPen(QPen(QColor(Qt::white), 1));
//    painter.drawRoundedRect(1, 1, width() - 2, height() - 2, 10*winScale(), 10*winScale());
    painter.drawRect(0, 0, width(), height());
    painter.setBrush(burush);
    auto font = painter.font();
    font.setPixelSize(14);
    painter.setFont(font);
    const QPoint point(30, height() - 40);  // 原点
    const int w = width() - 20 - point.x();
    const int h = point.y() - 15;
    // 消除锯齿
    painter.setRenderHint(QPainter::Antialiasing, true);
    // 画笔颜色，宽度
    painter.setPen(QPen(QColor(Qt::black), 2));
    // 坐标轴/边框
    QLine axisX(point, QPoint(point.x() + w, point.y()));
    QLine axisY(point, QPoint(point.x(), point.y() - h));
    QLine topX(axisX.x1(), axisY.y2(), axisX.x2(), axisY.y2());
    QLine topY(axisX.x2(), axisY.y1(), axisX.x2(), axisY.y2());
    painter.drawLines(QVector<QLine>()<<axisX<<axisY<<topX<<topY);
    // 刻度
    const qreal minX = 10, maxX = 100;    //范围
    const qreal minY = 20, maxY = 180;
    const qreal stepLenX = w/(maxX - minX);   //步长
    const qreal stepLenY = h/(maxY - minY);
    for (int x = minX; x <= maxX; x += 10) {
        QPoint temp(point.x() + (x - minX)*stepLenX, point.y() + 15);
        painter.drawPoint(temp.x(), point.y() - 2);
        painter.drawText(temp.x() - 5, temp.y() + 2, QString::number(x));
    }
    for (int y = minY; y <= maxY; y += 20) {
        QPoint temp(point.x() - 25, point.y() - (y - minY)*stepLenY + 5);
        painter.drawPoint(point.x() + 2, temp.y() - 5);
        painter.drawText(temp, QString("%1").arg(y, 3, 10, QLatin1Char(' ')));
    }
    painter.drawText(point.x(), height() - 5, QString("半卧:%1 平躺:%2 抬腿:%3")
                     .arg(getSign(SignType::Circular), getSign(SignType::Square), getSign(SignType::Delta)));
    // map si scope
    qreal minMapY = (m_suBArg.map.min - minY)*stepLenY;
    qreal maxMapY = (m_suBArg.map.max - minY)*stepLenY;
    qreal minSiX = (m_suBArg.si.min - minX)*stepLenX;
    qreal maxSiX = (m_suBArg.si.max - minX)*stepLenX;
    QLine minMap(axisX.x1(), axisX.y1() - minMapY, axisX.x2(), axisX.y1() - minMapY);
    QLine maxMap(axisX.x1(), axisX.y1() - maxMapY, axisX.x2(), axisX.y1() - maxMapY);
    QLine minSi(axisX.x1() + minSiX, axisY.y1(), axisX.x1() + minSiX, axisY.y2());
    QLine maxSi(axisX.x1() + maxSiX, axisY.y1(), axisX.x1() + maxSiX, axisY.y2());
    painter.drawLines(QVector<QLine>()<<minMap<<maxMap<<minSi<<maxSi);
    painter.drawText(QPointF(minMap.x1() + 2, minMap.y1() - 5), "MAP");
    painter.drawText(QPointF(maxMap.x1() + 2, maxMap.y1() - 5), "MAP");
    painter.drawText(QPointF(minSi.x1() + 2, minSi.y1() - 5), "SI");
    painter.drawText(QPointF(maxSi.x1() + 2, minSi.y1() - 5), "SI");
    // SSVRI LSWI
    QList<QPointF> minSsvriPoints;
    QList<QPointF> maxSsvriPoints;
    QList<QPointF> minLswiPoints;
    QList<QPointF> maxLswiPoints;
    for (int x = 0; x <= w; ++x) {
        qreal realX = x/stepLenX + minX;
        //计算y坐标值
        qreal minSsvri = m_suBArg.ssvri.min*realX/80 + m_suBArg.cvp;
        qreal maxSsvri = m_suBArg.ssvri.max*realX/80 + m_suBArg.cvp;
        qreal minLswi = m_suBArg.lswi.min/0.0144/realX + m_suBArg.lap;
        qreal maxLswi = m_suBArg.lswi.max/0.0144/realX + m_suBArg.lap;
        if(minSsvri >= minY && minSsvri <= maxY) {
            minSsvriPoints.append(QPointF(point.x() + x, point.y() + (minY - minSsvri)*stepLenY));
        }
        if(maxSsvri >= minY && maxSsvri <= maxY) {
            maxSsvriPoints.append(QPointF(point.x() + x, point.y() + (minY - maxSsvri)*stepLenY));
        }
        if(minLswi >= minY && minLswi <= maxY) {
            minLswiPoints.append(QPointF(point.x() + x, point.y() + (minY - minLswi)*stepLenY));
        }
        if(maxLswi >= minY && maxLswi <= maxY) {
            maxLswiPoints.append(QPointF(point.x() + x, point.y() + (minY - maxLswi)*stepLenY));
        }
    }
    drawCurve(painter, minSsvriPoints, "SSVRI", false);
    drawCurve(painter, maxSsvriPoints, "SSVRI", false);
    drawCurve(painter, minLswiPoints, "LSWI", true);
    drawCurve(painter, maxLswiPoints, "LSWI", true);
    // 相交点X坐标
    qreal minMin_X = (intersectAxisX(m_suBArg.lswi.min, m_suBArg.ssvri.min) - minX)*stepLenX + point.x();
    qreal minMax_X = (intersectAxisX(m_suBArg.lswi.min, m_suBArg.ssvri.max) - minX)*stepLenX + point.x();
    qreal maxMin_X = (intersectAxisX(m_suBArg.lswi.max, m_suBArg.ssvri.min) - minX)*stepLenX + point.x();
    qreal maxMax_X = (intersectAxisX(m_suBArg.lswi.max, m_suBArg.ssvri.max) - minX)*stepLenX + point.x();
    // 去除交点外的点
    removePoints(minLswiPoints, minMax_X, minMin_X);
    removePoints(maxLswiPoints, maxMax_X, maxMin_X);
    removePoints(minSsvriPoints, minMin_X, maxMin_X);
    removePoints(maxSsvriPoints, minMax_X, maxMax_X);
    // normal range values
    painter.setBrush(QColor(50, 205, 50));
    painter.setPen(QPen(Qt::black));
    QPolygonF pointList;
    pointList<<getNormalPoint(minLswiPoints, minSi.x1(), maxSi.x1(), minMap.y1(), maxMap.y1(), true);
    pointList<<getNormalPoint(minSsvriPoints, minSi.x1(), maxSi.x1(), minMap.y1(), maxMap.y1(), true);
    pointList<<getNormalPoint(maxLswiPoints, minSi.x1(), maxSi.x1(), minMap.y1(), maxMap.y1(), false);
    pointList<<getNormalPoint(maxSsvriPoints, minSi.x1(), maxSi.x1(), minMap.y1(), maxMap.y1(), false);
    QPainterPath path;
    path.addPolygon(pointList);
    painter.drawPath(path);
    // actual values
    foreach (auto p, m_suBArg.points) {
        if (p.map != 0 && p.si != 0) {
            double offset = painter.font().pixelSize()/2.0;
            auto ds = QGuiApplication::primaryScreen()->availableSize();
            auto map = p.map;
            auto si = p.si;
            if (map < minY)     map = minY;
            else if (map > maxY)map = maxY;
            if (si < minX)      si = minX;
            else if (si > maxX) si = maxX;
            painter.drawText(QPointF(axisX.x1() - offset + (si - minX)*stepLenX,
                                     axisX.y1() + offset*ds.height()/ds.width() - (map - minY)*stepLenY),
                             getSign(p.type));
        }
    }
}

double SudokuWidget::winScale() const
{
    return QGuiApplication::primaryScreen()->availableSize().width()/1920.0;
}

QString SudokuWidget::getSign(const SignType &type) const
{
    switch (type) {
    case SignType::Circular:
        return QString("●");
        break;
    case SignType::Square:
        return QString("■");
        break;
    case SignType::Delta:
        return QString("▲");
        break;
    default:
        break;
    }
    return QString();
}

void SudokuWidget::drawCurve(QPainter &painter, QList<QPointF> &points, QString name, bool flag)
{
    if (points.size() > 30) {
        QPainterPath path;
        path.moveTo(points.first());
        if (flag) {
            painter.drawText(points.first().x() - 10, 12, name);
        }
        else {
            painter.drawText(points.last().x() - 40, points.last().y() - 3, name);
        }
        foreach (const QPointF &point, points) {
            path.lineTo(point);
        }
        painter.drawPath(path);
    }
}

qreal SudokuWidget::intersectAxisX(qreal lswi, qreal ssvri)
{
    return (sqrt((80*lswi/(0.0144*ssvri))+pow((80*(m_suBArg.cvp - m_suBArg.lap))/(2*ssvri), 2))
            - (80*(m_suBArg.cvp - m_suBArg.lap))/(2*ssvri));
}

qreal SudokuWidget::intersectAxisY(qreal lswi, qreal ssvri)
{
    return (sqrt(lswi*ssvri/0.0144/80-m_suBArg.cvp*m_suBArg.lap + pow((m_suBArg.cvp + m_suBArg.lap)/2, 2))
            + (m_suBArg.cvp + m_suBArg.lap)/2);
}

void SudokuWidget::removePoints(QList<QPointF> &points, int min, int max)
{
    for (int index = 0; index < points.size(); ++index) {
        if (points.at(index).x() > min) {
            points = points.mid(index);
            break;
        }
    }
    for (int index = 0; index < points.size(); ++index) {
        if (points.at(index).x() > max) {
            points = points.mid(0, index);
            break;
        }
    }
}

QPolygonF SudokuWidget::getNormalPoint(QList<QPointF> &points, qreal minX, qreal maxX, qreal minY, qreal maxY, bool flag)
{
    QPolygonF pointList;
    if (flag) {
        for (auto it = points.begin(); it != points.end(); ++it) {
            if (it->x() > minX && it->x() < maxX && it->y() > maxY && it->y() < minY) {
                pointList<<*it;
            }
        }
    }
    else {
        for(auto it = points.rbegin(); it != points.rend(); ++it) {
            if (it->x() > minX && it->x() < maxX && it->y() > maxY && it->y() < minY) {
                pointList<<*it;
            }
        }
    }
    return pointList;
}
