#include "drawsudoku.h"
#include "datamanagement.h"
#include <iostream>
#include <math.h>
using namespace std;

DrawSudoku::DrawSudoku(QWidget *parent)
    : QDialog{parent}
{
    this->setWindowTitle(tr("九宫格分析图"));
    this->setFixedSize(480, 500);
    setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
    this->setStyleSheet("QDialog{background-color:#ffffff;}");
}

void DrawSudoku::clear()
{
    m_cPositon = -1;
    m_cSi = 0;
    m_cMap = 0;
    m_rPositon = -1;
    m_rSi = 0;
    m_rMap = 0;
}

void DrawSudoku::setSi(char position, qreal si, bool record)
{
    if (!record) {
        this->m_cPositon = position;
        this->m_cSi = si;
    }
    else {
        this->m_rPositon = position;
        this->m_rSi = si;
    }
    this->repaint();
}

void DrawSudoku::setMap(char position, qreal map, bool record)
{
    if (!record) {
        this->m_cPositon = position;
        this->m_cMap = map;
    }
    else {
        this->m_rPositon = position;
        this->m_rMap = map;
    }
    this->repaint();
}
void DrawSudoku::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    auto &args = DataManagement::getInstance().getArgs();
    //起始点
    const QPoint point(60, height() - 80);
    const int len = 400;
    QPainter painter(this);
    // 消除锯齿
    painter.setRenderHint(QPainter::Antialiasing, true);
    // 画笔颜色，宽度
    painter.setPen(QPen(QColor(Qt::black), 2));
    // 坐标轴/边框
    QLine axisX(point, QPoint(point.x() + len, point.y()));
    QLine axisY(point, QPoint(point.x(), point.y() - len));
    QLine topX(point.x(), point.y() - len, point.x() + len, point.y() - len);
    QLine topY(point.x() + len, point.y(), point.x() + len, point.y() - len);
    painter.drawLines(QVector<QLine>()<<axisX<<axisY<<topX<<topY);
    // 刻度
    const qreal minX = 10, maxX = 100;    //范围
    const qreal minY = 20, maxY = 180;
    const qreal stepLenX = len/(maxX - minX);   //步长
    const qreal stepLenY = len/(maxY - minY);
    for (int x = minX; x <= maxX; x += 10) {
        QPoint temp(point.x() + (x-minX)*stepLenX, point.y() + 15);
        painter.drawPoint(temp.x(), point.y() - 2);
        painter.drawText(temp, QString::number(x));
    }
    for (int y = minY; y <= maxY; y += 20) {
        QPoint temp(point.x()-20, point.y()-(y-minY)*stepLenY);
        painter.drawPoint(point.x() + 2, temp.y());
        painter.drawText(temp, QString::number(y));
    }
    // 标签
    QFont font = painter.font();
    font.setPointSize(18);
    painter.setFont(font);
    painter.drawText(QPoint(point.x() - 55, point.y() - len/2), QString("MAP"));
    painter.drawText(QPoint(point.x() + len/2, point.y() + 40), QString("SI"));

    painter.setPen(QPen(QColor(Qt::black), 1));
    // MAP SI
    Argument mapArg, siArg, ssvriArg, lswiArg;
    foreach (Argument argument, args.arguments) {
        if ("MAP" == argument.en)
            mapArg = argument;
        else if("SI" == argument.en)
            siArg = argument;
        else if("SSVRI" == argument.en)
            ssvriArg = argument;
        else if("LSWI" == argument.en)
            lswiArg = argument;
    }
    QLine minMap(point.x(), point.y()-(mapArg.min-minY)*stepLenY, point.x()+len, point.y()-(mapArg.min-minY)*stepLenY);
    QLine maxMap(point.x(), point.y()-(mapArg.max-minY)*stepLenY, point.x()+len, point.y()-(mapArg.max-minY)*stepLenY);
    QLine minSi(point.x()+(siArg.min-minX)*stepLenX, point.y(), point.x()+(siArg.min-minX)*stepLenX, point.y()-len);
    QLine maxSi(point.x()+(siArg.max-minX)*stepLenX, point.y(), point.x()+(siArg.max-minX)*stepLenX, point.y()-len);
    painter.drawLines(QVector<QLine>()<<minMap<<maxMap<<minSi<<maxSi);
    // SSVRI LSWI
    QList<QPointF> minSsvriPoints;
    QList<QPointF> maxSsvriPoints;
    QList<QPointF> minLswiPoints;
    QList<QPointF> maxLswiPoints;
    for (int x = 0; x <= len; ++x) {
        qreal realX = x/stepLenX+minX;
        //计算y坐标值
        qreal minSsvri = ssvriArg.min*realX/80+args.CVP;
        qreal maxSsvri = ssvriArg.max*realX/80+args.CVP;
        qreal minLswi = lswiArg.min/0.0144/realX+args.LAP;
        qreal maxLswi = lswiArg.max/0.0144/realX+args.LAP;
        if(minSsvri >= minY && minSsvri <= maxY) {
            minSsvriPoints.append(QPointF(point.x()+x, point.y()+(minY-minSsvri)*stepLenY));
        }
        if(maxSsvri >= minY && maxSsvri <= maxY) {
            maxSsvriPoints.append(QPointF(point.x()+x, point.y()+(minY-maxSsvri)*stepLenY));
        }
        if(minLswi >= minY && minLswi <= maxY) {
            minLswiPoints.append(QPointF(point.x()+x, point.y()+(minY-minLswi)*stepLenY));
        }
        if(maxLswi >= minY && maxLswi <= maxY) {
            maxLswiPoints.append(QPointF(point.x()+x, point.y()+(minY-maxLswi)*stepLenY));
        }
    }
    font.setPointSize(10);
    painter.setFont(font);
    drawCurve(painter, minSsvriPoints, "SSVRI");
    drawCurve(painter, maxSsvriPoints, "SSVRI");
    drawCurve(painter, minLswiPoints, "LSWI");
    drawCurve(painter, maxLswiPoints, "LSWI");

    // 相交点X坐标
    qreal minMin_X = (intersectAxisX(lswiArg.min,ssvriArg.min)-minX)*stepLenX+point.x();
    qreal minMax_X = (intersectAxisX(lswiArg.min,ssvriArg.max)-minX)*stepLenX+point.x();
    qreal maxMin_X = (intersectAxisX(lswiArg.max,ssvriArg.min)-minX)*stepLenX+point.x();
    qreal maxMax_X = (intersectAxisX(lswiArg.max,ssvriArg.max)-minX)*stepLenX+point.x();
    // 去除交点外的点
    removePoints(minLswiPoints, minMax_X, minMin_X);
    removePoints(maxLswiPoints, maxMax_X, maxMin_X);
    removePoints(minSsvriPoints, minMin_X, maxMin_X);
    removePoints(maxSsvriPoints, minMax_X, maxMax_X);

    // normal values
    painter.setBrush(QColor(50, 205, 50));
    painter.setPen(QPen(Qt::black));
    QPolygonF pointList;
    pointList<<getNormalPoint(minLswiPoints, minMap.y1(), maxMap.y1(), minSi.x1(),maxSi.x1());
    pointList<<getNormalPoint(minSsvriPoints, minMap.y1(), maxMap.y1(), minSi.x1(),maxSi.x1());
    pointList<<getNormalPoint(maxLswiPoints, minMap.y1(), maxMap.y1(), minSi.x1(),maxSi.x1(), false);
    pointList<<getNormalPoint(maxSsvriPoints, minMap.y1(), maxMap.y1(), minSi.x1(),maxSi.x1(), false);
    QPainterPath path;
    path.addPolygon(pointList);
    painter.drawPath(path);

    // actual values
    // draw record position
    painter.setBrush(Qt::black);
    font.setPointSize(12);
    painter.setFont(font);
    if (m_cSi != 0 && m_cMap != 0) {
        drawPosition(painter, m_cPositon, QPointF((m_cSi-minX)*stepLenX+point.x(), point.y()-(m_cMap-minY)*stepLenY), false);
    }
    if (m_rSi != 0 && m_rMap != 0) {
        drawPosition(painter, m_rPositon, QPointF((m_rSi-minX)*stepLenX+point.x(), point.y()-(m_rMap-minY)*stepLenY), true);
    }
}

void DrawSudoku::drawCurve(QPainter &painter, QList<QPointF> &points, QString name)
{
    if (!points.isEmpty()) {
        QPainterPath path;
        path.moveTo(points.at(0));
        painter.drawText(points.at(points.size()/10), name);
        foreach (const QPointF &point, points) {
            path.lineTo(point);
        }
        painter.drawPath(path);
    }
}

QPolygonF DrawSudoku::getNormalPoint(QList<QPointF> &points, int map1, int map2,
                                     int si1, int si2, bool flag)
{
    QPolygonF pointList;
    if(flag) {
        foreach (const QPointF &point, points) {
            if (point.y() <= map1 && point.y() >= map2 && point.x() >= si1 && point.x() <= si2) {
                pointList<<point;
            }
        }
    }
    else {
        for(auto it = points.rbegin(); it != points.rend(); ++it) {
            if (it->y() <= map1 && it->y() >= map2 && it->x() >= si1 && it->x() <= si2) {
                pointList<<*it;
            }
        }
    }
    return pointList;
}

qreal DrawSudoku::intersectAxisX(qreal lswi, qreal ssvri)
{
    auto &args = DataManagement::getInstance().getArgs();
    return (sqrt((80*lswi/(0.0144*ssvri))+pow((80*(args.CVP-args.LAP))/(2*ssvri), 2))
            - (80*(args.CVP-args.LAP))/(2*ssvri));
}

qreal DrawSudoku::intersectAxisY(qreal lswi, qreal ssvri)
{
    auto &args = DataManagement::getInstance().getArgs();
    return (sqrt(lswi*ssvri/0.0144/80-args.CVP*args.LAP+pow((args.CVP+args.LAP)/2,2))
            + (args.CVP+args.LAP)/2);
}

void DrawSudoku::removePoints(QList<QPointF> &points, int min, int max)
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

void DrawSudoku::drawPosition(QPainter &painter, char position, QPointF point, bool record)
{
    const int radius = 6;
    const int txtSize = painter.font().pointSize();
    QPointF titleGraph(80, height()-20);
    QPointF titleTxt(titleGraph.x()+txtSize/2, titleGraph.y()+txtSize/2);
    if (record) {
        titleGraph.setX(160);
        titleTxt.setX(titleGraph.x()+radius);
    }
    switch (position) {
    case 0: // 圆
        painter.drawEllipse(point, radius, radius);
        painter.drawEllipse(titleGraph, txtSize/2, txtSize/2);
        break;
    case 1: // 矩形
        painter.drawRect(point.x()-radius/2, point.y()-radius/2, 2*radius, 2*radius);
        painter.drawRect(titleGraph.x()-txtSize/2, titleGraph.y()-txtSize/2, txtSize, txtSize);
        break;
    case 2: // 三角形
    {
        QPainterPath path;
        path.addPolygon(QPolygonF()<<QPointF(point.x(), point.y()-radius)
                        <<QPointF(point.x()-radius,point.y()+radius)
                        <<QPointF(point.x()+radius,point.y()+radius)
                        <<QPointF(point.x(), point.y()-radius));
        painter.drawPath(path);
        QPainterPath titlePath;
        titlePath.addPolygon(QPolygonF()<<QPointF(titleGraph.x(), titleGraph.y()-txtSize/2)
                             <<QPointF(titleGraph.x()-txtSize/2, titleGraph.y()+txtSize/2)
                             <<QPointF(titleGraph.x()+txtSize/2, titleGraph.y()+txtSize/2)
                             <<QPointF(titleGraph.x(), titleGraph.y()-txtSize/2));
        painter.drawPath(titlePath);
    }
        break;
    }
    if (position >= 0 && position <= 2) {
        painter.drawText(titleTxt, ":"+getPosStr(position));
    }
}

QString DrawSudoku::getPosStr(char position)
{
    switch (position) {
    case 0:
        return tr("半卧");
        break;
    case 1:
        return tr("平躺");
        break;
    case 2:
        return tr("抬腿");
        break;
    }
    return nullptr;
}

