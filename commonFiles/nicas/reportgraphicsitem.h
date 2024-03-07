#ifndef REPORTGRAPHICSITEM_H
#define REPORTGRAPHICSITEM_H

#include <QObject>
#include <QGraphicsItem>
#include <QPainter>

#include "painterconfig.h"

class ReportGraphicsItem : public QObject, public QGraphicsItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)
public:
    explicit ReportGraphicsItem(const QSize &size, const QJsonObject &object,
                                const bool &samepage, const QPixmap &logo, const PageType &ptype);
    PageType pageType() const;
    // void resize(const QSize &size, const int &index);
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0) override;
public slots:
    void startRecache();
    void endRecache();
private:
    QSize m_size;
    const QJsonObject &m_object;
    const bool m_samepage;
    const QPixmap m_logo;
    const PageType m_ptype;
};

#endif // REPORTGRAPHICSITEM_H
