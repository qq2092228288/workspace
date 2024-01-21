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
#include <QGroupBox>
#include <QFile>
#include <QComboBox>

#include "painterconfig.h"

PrintGraphicsView::PrintGraphicsView(QGraphicsScene *scene, QWidget *parent)
    : QGraphicsView(scene, parent)
{
    QFile qssFile(":/qss/printgraphicsviewui.qss");
    if (qssFile.open(QIODevice::ReadOnly)) setStyleSheet(qssFile.readAll());
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
    // 缓存背景
    setCacheMode(QGraphicsView::CacheBackground);
    // 只更新需要更新的图元
    setViewportUpdateMode(QGraphicsView::MinimalViewportUpdate);
    // 禁止滚动条
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    auto mainLayout = new QVBoxLayout(this);
    auto hLayout = new QHBoxLayout();
    mainLayout->addLayout(hLayout);
    mainLayout->setContentsMargins(100, 0, 100, 0);
    mainLayout->addStretch();

    hLayout->addStretch();
    auto groupBox = new QGroupBox(this);
    hLayout->addWidget(groupBox);
    hLayout->addStretch();

    auto boxLayout = new QHBoxLayout(groupBox);

    auto defaultButton = new QPushButton(tr("默认"), groupBox);
    auto scaleComboBox = new QComboBox(groupBox);
    auto zoomInButton = new QPushButton(tr("缩小"), groupBox);
    auto zoomOutButton = new QPushButton(tr("放大"), groupBox);
    auto pageSetButton = new QPushButton(tr("页面设置"), groupBox);
    auto printButton = new QPushButton(tr("打印"), groupBox);

    boxLayout->addWidget(defaultButton);
    boxLayout->addWidget(scaleComboBox);
    boxLayout->addWidget(zoomInButton);
    boxLayout->addWidget(zoomOutButton);
    boxLayout->addStretch();
    boxLayout->addWidget(pageSetButton);
    boxLayout->addWidget(printButton);

    zoomInButton->setIconSize(QSize(80, 80));
    zoomInButton->setIcon(QIcon(":/images/zoomIn.svg"));
}

bool PrintGraphicsView::viewportEvent(QEvent *event)
{
    switch (event->type()) {
    case QEvent::TouchBegin:
    case QEvent::TouchUpdate:
    {
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
            totalScaleFactor *= currentScaleFactor;
            if (totalScaleFactor < minFactor) {
                totalScaleFactor = minFactor;
            }
            else if (totalScaleFactor > maxFactor) {
                totalScaleFactor = maxFactor;
            }
            else {
                emit startRecache();
            }
            setTransform(QTransform::fromScale(totalScaleFactor, totalScaleFactor));
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
        if (event->angleDelta().y() > 0) {
            totalScaleFactor += 0.25;
            if (totalScaleFactor > maxFactor) {
                totalScaleFactor = maxFactor;
            }
            else {
                emit startRecache();
            }
        }
        else {
            totalScaleFactor -= 0.25;
            if (totalScaleFactor < minFactor) {
                totalScaleFactor = minFactor;
            }
            else {
                emit startRecache();
            }
        }
        setTransform(QTransform::fromScale(totalScaleFactor, totalScaleFactor));
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

