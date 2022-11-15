#include "trendchart.h"
#include <iostream>
using namespace std;
#include <algorithm>
#include <numeric>
#include "datamanagement.h"

TrendChart::TrendChart(CustomCtrl *customCtrl, QWidget *parent)
    : QWidget{parent},m_pCustomCtrl{customCtrl}
{
    digit = customCtrl->getDigit();
    m_pChartView = new QChartView(this);
    m_pLabel = new QLabel(this);
    m_pAxisX = new QDateTimeAxis(this);
    m_pAxisY = new QValueAxis(this);
    m_pSeries = new QLineSeries(this);
    dialog = new SelectItemDialog(true);

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

    m_pChartView->chart()->addAxis(m_pAxisX, Qt::AlignBottom);
    m_pChartView->chart()->addAxis(m_pAxisY, Qt::AlignLeft);
    m_pSeries->attachAxis (m_pAxisX);
    m_pSeries->attachAxis (m_pAxisY);
    m_pSeries->setPen(QPen(Qt::blue,2,Qt::SolidLine));

    m_pSeries->setUseOpenGL(true);
    m_pChartView->chart()->legend()->hide();

    connect(dialog, &SelectItemDialog::currentText, this, &TrendChart::getChangeText);
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
    setMinX = true;
    m_pSeries->clear();
    m_pAxisX->setRange(QDateTime(),QDateTime());
    m_values.clear();
}

void TrendChart::addValue(double value)
{
    if (setMinX) {
        m_pAxisX->setMin(QDateTime::currentDateTime());
        setMinX = false;
    }
    m_values.append(value);
    m_pSeries->append(QDateTime::currentMSecsSinceEpoch(),value);
    double min = *min_element(m_values.begin(),m_values.end());
    double max = *max_element(m_values.begin(),m_values.end());
    m_pLabel->setText(QString("最大值：%1 最小值：%2 平均值：%3").arg(min).arg(max)
                      .arg(QString::number(accumulate(m_values.begin(),m_values.end(),0)/m_values.size(),'f',digit)));
    if (m_pAxisY->min() != min - 0.01 || m_pAxisY->max() != max + 0.01) {
        m_pAxisY->setRange(min - 0.01, max + 0.01);
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
