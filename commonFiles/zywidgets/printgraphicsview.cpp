#include "printgraphicsview.h"
#include <QTouchEvent>
#include <QScrollBar>
#include <QPageSize>
#include <QScrollBar>
#include <QTouchEvent>

#include <QPrintDialog>
#include <QGraphicsItem>
#include <QScreen>
#include <QDateTime>
#include <QJsonObject>
#include <QOpenGLWidget>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QGroupBox>
#include <QFile>

#include <QPageSetupDialog>
#include <QApplication>
#include <QDesktopWidget>
#include <QPageLayout>
#include <QLineEdit>

#include "painterconfig.h"
#include "reportgraphicsitem.h"

PrintGraphicsView::PrintGraphicsView(const bool &samepage, const QPixmap &logo, const QJsonObject &object,
                                     QGraphicsScene *scene, QWidget *parent)
    : QGraphicsView{scene, parent}
    , m_samepage{samepage}
    , m_logo{logo}
    , m_object{object}
    , itemList{QStringList {"50%", "75%", "100%", "125%", "150%", "200%",
                            "300%", "400%", "500%", "600%", "800%", "1000%"}}
{
    m_printer.setFullPage(true);
    m_printer.setPageMargins(QMarginsF(0, 0, 0, 0), QPageLayout::Millimeter);

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
    // setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    // setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    auto mainLayout = new QVBoxLayout(this);
    auto hLayout = new QHBoxLayout();
    mainLayout->addLayout(hLayout);
    mainLayout->setContentsMargins(0, 10, 0, 0);
    mainLayout->addStretch();

    hLayout->addStretch();
    auto groupBox = new QGroupBox(this);
    hLayout->addWidget(groupBox);
    hLayout->addSpacing(qApp->style()->pixelMetric(QStyle::PM_ScrollBarExtent));
    hLayout->addStretch();

    auto boxLayout = new QHBoxLayout(groupBox);

    scaleComboBox = new QComboBox(groupBox);
    auto zoomOutButton = new QPushButton(groupBox);
    auto zoomInButton = new QPushButton(groupBox);
    auto pageSettingButton = new QPushButton(groupBox);
    auto printerButton = new QPushButton(groupBox);

    boxLayout->addStretch();
    boxLayout->addWidget(scaleComboBox);
    boxLayout->addSpacing(10);
    boxLayout->addWidget(zoomOutButton);
    boxLayout->addWidget(zoomInButton);
    boxLayout->addSpacing(100);
    boxLayout->addWidget(pageSettingButton);
    boxLayout->addSpacing(10);
    boxLayout->addWidget(printerButton);
    boxLayout->addStretch();

    scaleComboBox->setEditable(true);
    scaleComboBox->addItems(itemList);
    scaleComboBox->setMaxVisibleItems(itemList.size());
    scaleComboBox->setCurrentIndex(2);
    QRegExp rx("^([1-9]\\d{0,2}|1000)%?$");
    scaleComboBox->lineEdit()->setValidator(new QRegExpValidator(rx));

    zoomOutButton->setIcon(QIcon(":/images/zoomOut.svg"));
    zoomInButton->setIcon(QIcon(":/images/zoomIn.svg"));
    pageSettingButton->setIcon(QIcon(":/images/pageSetting.svg"));
    printerButton->setIcon(QIcon(":/images/printer.svg"));

    zoomOutButton->setToolTip(tr("缩小"));
    zoomInButton->setToolTip(tr("放大"));
    pageSettingButton->setToolTip(tr("页面设置"));
    printerButton->setToolTip(tr("打印"));

    connect(scaleComboBox, QOverload<int>::of(&QComboBox::activated), this, &PrintGraphicsView::scaleComboBoxActivated);
    connect(scaleComboBox->lineEdit(), &QLineEdit::editingFinished, this, &PrintGraphicsView::editingFinished);
    connect(zoomOutButton, &QPushButton::clicked, this, [=]() { zoom(false); emit endRecache();});
    connect(zoomInButton, &QPushButton::clicked, this, [=]() { zoom(true); emit endRecache();});
    connect(pageSettingButton, &QPushButton::clicked, this, &PrintGraphicsView::pageSettingButtonClicked);
    connect(printerButton, &QPushButton::clicked, this, &PrintGraphicsView::printerButtonClicked);
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
            totalScaleFactor = static_cast<int>(totalScaleFactor * 100) / 100.0;
            changeScale();
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
        zoom(event->angleDelta().y() > 0);
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

void PrintGraphicsView::changeScale()
{
    scaleComboBox->setCurrentText(QString("%1%").arg(totalScaleFactor * 100));
    setTransform(QTransform::fromScale(totalScaleFactor, totalScaleFactor));
}

void PrintGraphicsView::zoom(bool in)
{
    if (in) {
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
    changeScale();
}

void PrintGraphicsView::editingFinished()
{
    if (scaleComboBox->lineEdit() != nullptr) {
        auto text = scaleComboBox->lineEdit()->text();
        if (text.indexOf("%") == -1) {
            scaleComboBox->lineEdit()->setText(text + "%");
        }
        while (scaleComboBox->count() > itemList.count()) {
            if (itemList.count() + 1 == scaleComboBox->count()) {
                auto view = qobject_cast<QListView *>(scaleComboBox->view());
                view->setRowHidden(itemList.count(), true);
                break;
            }
            else {
                scaleComboBox->removeItem(itemList.count());
            }
        }
    }
}

void PrintGraphicsView::scaleComboBoxActivated()
{
    auto factor = scaleComboBox->currentText().remove("%").toDouble() / 100.0;
    if (factor != totalScaleFactor && factor > 0) {
        totalScaleFactor = factor;
        changeScale();
    }
}

void PrintGraphicsView::pageSettingButtonClicked()
{
    QPageSetupDialog(&m_printer).exec();
}

void PrintGraphicsView::printerButtonClicked()
{
    m_printer.setFullPage(true);
    m_printer.setPageMargins(QMarginsF(0, 0, 0, 0), QPageLayout::Millimeter);
    if (QPrintDialog(&m_printer).exec() == QDialog::Accepted) {
        QPainter painter(&m_printer);
        painter.setRenderHint(QPainter::Antialiasing);
        auto items = this->scene()->items();
        for (int i = 0; i < items.size(); ++i) {
            auto item = dynamic_cast<ReportGraphicsItem *>(items.at(i));
            if (item != nullptr) {
                PainterConfig config(&painter, pageSize(), m_object, m_samepage, m_logo);
                config.paintPage(item->pageType());
                if (i + 1 != items.size()) {
                    m_printer.newPage();
                }
            }
        }
        painter.end();
    }
}

QSize PrintGraphicsView::pageSize() const
{
    auto sizef = m_printer.pageLayout().pageSize().size(QPageSize::Unit::Inch);
    if (m_printer.pageLayout().orientation() == QPageLayout::Landscape) {
        sizef.transpose();
    }
    return QSize(sizef.width() * screen()->logicalDotsPerInchX(),
                 sizef.height() * screen()->logicalDotsPerInchY());
}

