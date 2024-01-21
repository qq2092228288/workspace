#include "reportgraphicsitem.h"
#include "painterconfig.h"
#include <QDebug>


ReportGraphicsItem::ReportGraphicsItem(const QSize &size)
    : m_size{size}
{
    setCacheMode(QGraphicsItem::ItemCoordinateCache);
}

QRectF ReportGraphicsItem::boundingRect() const
{
    return QRectF(-0.5, -100, m_size.width() + 1, m_size.height() + 100);
}

void ReportGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    PainterConfig config(painter, m_size);
    HeaderMsg msg("无创心功能监测报告", "中医院", "心内科", "测试", "123", "女", 40, 170, 60, 1.72);
    auto hEnd = config.paintHeader(msg);
    config.paintFooter(QDateTime::currentDateTime(), "李四");
    config.paintMain(hEnd, QJsonObject());
}

void ReportGraphicsItem::startRecache()
{
    if (cacheMode() == QGraphicsItem::ItemCoordinateCache) return;
    setCacheMode(QGraphicsItem::ItemCoordinateCache);
}

void ReportGraphicsItem::endRecache()
{
    setCacheMode(QGraphicsItem::DeviceCoordinateCache);
}
