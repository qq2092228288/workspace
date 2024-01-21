#ifndef PRINTGRAPHICSVIEW_H
#define PRINTGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QPushButton>
#include <QMenuBar>
#include <QGridLayout>

class PrintGraphicsView : public QGraphicsView
{
    Q_OBJECT

public:
    PrintGraphicsView(QGraphicsScene *scene, QWidget *parent = nullptr);
signals:
    void startRecache();
    void endRecache();
protected:
    bool viewportEvent(QEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
private slots:
    void buttonClicked();
private:
    qreal totalScaleFactor = 1;
private:
    QMenuBar *menuBar;
    QPushButton *button;
};

#endif // PRINTGRAPHICSVIEW_H
