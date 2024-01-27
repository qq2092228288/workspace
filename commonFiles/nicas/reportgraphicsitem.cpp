#include "reportgraphicsitem.h"
#include <QDebug>
#include <QJsonObject>

ReportGraphicsItem::ReportGraphicsItem(const QSize &size, const QJsonObject &object, const bool &samepage,
                                       const PageType &ptype)
    : m_size{size}
    , m_object{object}
    , m_samepage{samepage}
    , m_ptype{ptype}
{
    setCacheMode(QGraphicsItem::DeviceCoordinateCache);
}

PageType ReportGraphicsItem::pageType() const
{
    return m_ptype;
}

// void ReportGraphicsItem::resize(const QSize &size, const int &index)
// {
//     prepareGeometryChange();
//     m_size = size;
//     setPos(0, index * boundingRect().height() + 20);
// }

QRectF ReportGraphicsItem::boundingRect() const
{
    return QRectF(-0.5, m_size.height() * -0.1, m_size.width() + 1, m_size.height() * 1.1);
}

void ReportGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    PainterConfig config(painter, m_size, m_object, m_samepage);
    config.paintPage(m_ptype);
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
