#include "trendchartswidget.h"
#include "datamanagement.h"

TrendChartsWidget::TrendChartsWidget(QWidget *parent)
    : QWidget{parent}
{
    setWindowTitle(tr("趋势图"));
    auto &instance = DataManagement::getInstance();
    setMinimumSize(1600*instance.wZoom(),900*instance.hZoom());
    this->setStyleSheet("QWidget{background-color:#ffffff;}");
    CustomCtrlRegulator *regulator = instance.getRegulator();
    foreach (auto customCtrl, regulator->getAllCustomCtrls()) {
        connect(customCtrl->getTrendChart(), &TrendChart::changeName, this, &TrendChartsWidget::changeShow);
    }
    dataGLayout = new QGridLayout(this);
    QList<CustomCtrl *> customCtrls = regulator->getSaveCustomCtrls(true);
    for (int num = 0; num < customCtrls.size(); ++num) {
        QHBoxLayout *hLayout = new QHBoxLayout;
        hLayouts.append(hLayout);
        hLayout->addWidget(customCtrls.at(num)->getTrendChart());
        dataGLayout->addLayout(hLayout,num/4,num%4);
        customCtrls.at(num)->getTrendChart()->show();
    }
}

void TrendChartsWidget::changeShow(const QString &current, const QString &change)
{
    auto regulator = DataManagement::getInstance().getRegulator();
    TrendChart *cuTc = regulator->getCustomCtrl(current)->getTrendChart();
    TrendChart *chTc = regulator->getCustomCtrl(change)->getTrendChart();
    foreach (QHBoxLayout *layout, hLayouts) {
        if(-1 != layout->indexOf(cuTc)) {
            cuTc->hide();
            layout->removeWidget(cuTc);
            layout->addWidget(chTc);
            chTc->show();
            regulator->changeCurrentNames(current,change,true);
            break;
        }
    }
}

void TrendChartsWidget::widgetShow()
{
    if (this->isHidden()) {
        this->show();
    }
    else {
        this->raise();
        this->activateWindow();
        this->setWindowState((this->windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
    }
}
