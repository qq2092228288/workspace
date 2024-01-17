#ifndef PRINTGRAPHICSVIEW_H
#define PRINTGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QPushButton>

class PrintGraphicsView : public QGraphicsView
{
    Q_OBJECT

public:
    PrintGraphicsView(QGraphicsScene *scene, QWidget *parent = nullptr);

    bool viewportEvent(QEvent *event) override;
private slots:
    void buttonClicked();
private:
    qreal totalScaleFactor = 1;
private:
    QPushButton *button;
};

#endif // PRINTGRAPHICSVIEW_H
