#include "trendchart.h"
#include <iostream>
using namespace std;
#include <algorithm>
#include <numeric>
#include "datamanagement.h"

TrendChart::TrendChart(CustomCtrl *customCtrl, QWidget *parent)
    : QWidget{parent},m_pCustomCtrl{customCtrl}
{
    auto &instance = DataManagement::getInstance();
    digit = customCtrl->getDigit();
    m_pChartView = new QChartView(this);
    m_pLabel = new QLabel(this);
    m_pAxisX = new QDateTimeAxis(this);
    m_pAxisY = new QValueAxis(this);
    m_pSeries = new QLineSeries(this);
    dialog = new SelectItemDialog(true);

    m_pLabel->setStyleSheet(QString("QLabel{color:#000000;font:bold %1px;font-family:Microsoft YaHei;}")
                            .arg(20*instance.zoom() + 1));

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(m_pChartView);
    mainLayout->addWidget(m_pLabel,0,Qt::AlignHCenter);
    // 消除重影
    m_pChartView->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    // 标题
    m_pChartView->chart()->setTitle(QString("%1(%2)").arg(m_pCustomCtrl->getArgItems().dataName,
                                                          m_pCustomCtrl->getArgItems().dataUnit));
    // 背景样式
    this->setStyleSheet("QWidget{background-color:#ffffff;}");
    m_pChartView->chart()->setBackgroundVisible(false);
    // 图形边距
//    m_pChartView->chart()->setContentsMargins(0, -10, 0, -10);
    m_pChartView->chart()->layout()->setContentsMargins(0, 0, 0, 0);

    m_pChartView->chart()->addSeries(m_pSeries);
    m_pAxisX->setFormat("hh:mm:ss");
    m_pAxisX->setTickCount(2);
    m_pAxisY->setTickCount(4);

    m_pChartView->chart()->addAxis(m_pAxisX, Qt::AlignBottom);
    m_pChartView->chart()->addAxis(m_pAxisY, Qt::AlignLeft);
    m_pSeries->attachAxis (m_pAxisX);
    m_pSeries->attachAxis (m_pAxisY);
    m_pSeries->setPen(QPen(Qt::blue,2,Qt::SolidLine));

    m_pSeries->setUseOpenGL(true);
    m_pChartView->chart()->legend()->hide();

    connect(dialog, &SelectItemDialog::currentText, this, &TrendChart::getChangeText);
    connect(&instance, &DataManagement::startCheck, this, [=](bool isStart){
        if (isStart) {
            m_pAxisX->setMin(QDateTime::currentDateTime());
        }
    });
}

TrendChart::~TrendChart()
{
    delete dialog;
}

void TrendChart::mouseDoubleClickEvent(QMouseEvent *event)
{
    event->accept();
    dialog->exec();
}

void TrendChart::clear()
{
    m_pSeries->clear();
    m_pAxisX->setRange(QDateTime(), QDateTime());
    m_values.clear();
}

void TrendChart::addValue(double value)
{
    m_values.append(value);
    m_pSeries->append(QDateTime::currentMSecsSinceEpoch(),value);
    double min = *min_element(m_values.begin(),m_values.end());
    double max = *max_element(m_values.begin(),m_values.end());
    m_pLabel->setText(QString("MAX: %1 MIN: %2 AVG: %3")
                      .arg(QString::number(min, 'f', digit),
                           QString::number(max, 'f', digit),
                           QString::number(accumulate(m_values.begin(), m_values.end(),0)/m_values.size(), 'f', digit)));
    double offset = 10/qPow(10, digit);
    if (m_pAxisY->min() != min - offset || m_pAxisY->max() != max + offset) {
        m_pAxisY->setRange(min - offset, max + offset);
    }
    m_pAxisX->setMax(QDateTime::currentDateTime());
//    m_pChartView->update();
}

void TrendChart::getChangeText(const QString &text)
{
    auto &instance = DataManagement::getInstance();
    emit changeName(m_pCustomCtrl->getArgItems().dataName, text);
    instance.getRegulator()->saveNames(instance.getPaths().trendCharts(),instance.getRegulator()->getCurrentNames(true));
}
