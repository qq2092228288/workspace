#ifndef REPORTGRAPHICSITEM_H
#define REPORTGRAPHICSITEM_H

#include <QObject>
#include <QGraphicsItem>
#include <QPainter>

class ReportGraphicsItem : public QObject, public QGraphicsItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)
public:
    explicit ReportGraphicsItem(const QSize &size);
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0) override;
protected:
    void timerEvent(QTimerEvent *event) override;
private:
    const QSize &m_size;
};

#endif // REPORTGRAPHICSITEM_H
