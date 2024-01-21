#include "printgraphicsview.h"
#include <QTouchEvent>
#include <QScrollBar>
#include <QPageSize>
#include <QScrollBar>
#include <QTouchEvent>
#include <QPrinter>
#include <QPrintDialog>
#include <QGraphicsItem>
#include <QScreen>
#include <QDebug>
#include <QDateTime>
#include <QJsonObject>
#include <QOpenGLWidget>
#include <QMouseEvent>
#include <QKeyEvent>

#include "painterconfig.h"

PrintGraphicsView::PrintGraphicsView(QGraphicsScene *scene, QWidget *parent)
    : QGraphicsView(scene, parent)
{
    // 背景颜色
    setBackgroundBrush(QBrush(Qt::gray));
    // 不保持painter状态
    setOptimizationFlag(DontSavePainterState, true);
    // 不调整消除混叠
    setOptimizationFlag(DontAdjustForAntialiasing, true);
    // 启用手势操作
    viewport()->setAttribute(Qt::WA_AcceptTouchEvents);
    // 滚动手动拖动
    setDragMode(ScrollHandDrag);
    // 抗锯齿
    // setRenderHint(QPainter::Antialiasing);
    setCacheMode(QGraphicsView::CacheBackground);
    // 只更新需要更新的图元
    setViewportUpdateMode(QGraphicsView::MinimalViewportUpdate);
    // 启用OpenGL
    setViewport(new QOpenGLWidget());

    // button = new QPushButton(tr("打印"), this);
    // connect(button, &QPushButton::clicked, this, &PrintGraphicsView::buttonClicked);
}

bool PrintGraphicsView::viewportEvent(QEvent *event)
{
    switch (event->type()) {
    case QEvent::TouchBegin:
    case QEvent::TouchUpdate:
    {
        emit startRecache();
        QTouchEvent *touchEvent = static_cast<QTouchEvent *>(event);
        QList<QTouchEvent::TouchPoint> touchPoints = touchEvent->touchPoints();
        if (touchPoints.count() == 2) {
            // determine scale factor
            const QTouchEvent::TouchPoint &touchPoint0 = touchPoints.first();
            const QTouchEvent::TouchPoint &touchPoint1 = touchPoints.last();
            qreal currentScaleFactor =
                QLineF(touchPoint0.pos(), touchPoint1.pos()).length()
                / QLineF(touchPoint0.startPos(), touchPoint1.startPos()).length();
            if (touchEvent->touchPointStates() & Qt::TouchPointReleased) {
                // if one of the fingers is released, remember the current scale
                // factor so that adding another finger later will continue zooming
                // by adding new scale factor to the existing remembered value.
                totalScaleFactor *= currentScaleFactor;
                currentScaleFactor = 1;
            }
            setTransform(QTransform::fromScale(totalScaleFactor * currentScaleFactor,
                                               totalScaleFactor * currentScaleFactor));
        }
        return true;
    }
    case QEvent::TouchEnd:
        emit endRecache();
        return true;
    default:
        break;
    }
    return QGraphicsView::viewportEvent(event);
}

void PrintGraphicsView::wheelEvent(QWheelEvent *event)
{
    if (event->modifiers() == Qt::ControlModifier) {
        emit startRecache();
        if (event->angleDelta().y() > 0) {
            if (totalScaleFactor <= 3) {
                totalScaleFactor += 0.25;
                setTransform(QTransform::fromScale(totalScaleFactor, totalScaleFactor));
            }

        }
        else {
            if (totalScaleFactor >= 0.5) {
                totalScaleFactor -= 0.25;
                setTransform(QTransform::fromScale(totalScaleFactor, totalScaleFactor));
            }
        }
    }
    QGraphicsView::wheelEvent(event);
}

void PrintGraphicsView::keyReleaseEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Control) {
        emit endRecache();
    }
    return QGraphicsView::keyReleaseEvent(event);
}

void PrintGraphicsView::buttonClicked()
{
    QPrinter printer;
    printer.setFullPage(true);
    printer.setPageMargins(QMarginsF(0, 0, 0, 0), QPageLayout::Millimeter);
    if (QPrintDialog(&printer).exec() == QDialog::Accepted) {
        QPainter painter(&printer);
        painter.setRenderHint(QPainter::Antialiasing);

        auto items = this->scene()->items();
        for (int i = 0; i < items.size(); ++i) {
            PainterConfig config(&painter, QPageSize::sizePixels(QPageSize::A4, screen()->logicalDotsPerInch()));
            config.initPage();
            HeaderMsg msg("无创心功能监测报告", "中医院", "心内科", "测试", "123", "女", 40, 170, 60, 1.72);
            auto hEnd = config.paintHeader(msg);
            config.paintFooter(QDateTime::currentDateTime(), "李四");
            config.paintMain(hEnd, QJsonObject());
            if (i + 1 != items.size()) {
                printer.newPage();
            }
        }
        painter.end();
    }
}

