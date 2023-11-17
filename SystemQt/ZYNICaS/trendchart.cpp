#include "trendchart.h"
#include <iostream>
using namespace std;
#include <algorithm>
#include <numeric>
#include "datamanagement.h"

TrendChart::TrendChart(QWidget *customCtrl)
    : m_pCustomCtrl{qobject_cast<CustomCtrl *>(customCtrl)}
{
    auto &instance = DataManagement::getInstance();
    // 背景样式
    this->setStyleSheet(QString("QWidget{background-color:#ffffff;}"));
    digit = m_pCustomCtrl->getDigit();
    m_pGroupBox = new QGroupBox(this);
    m_pChartView = new QChartView(this);
    m_pLabel = new QLabel(this);
    m_pAxisX = new QDateTimeAxis(this);
    m_pAxisY = new QValueAxis(this);
    m_pSeries = new QLineSeries(this);
    dialog = new SelectItemDialog(true);

    m_pGroupBox->setStyleSheet("QGroupBox\
                               {\
                                   border:2px solid #cfcfcf;\
                                   margin-top: 2ex;\
                                   color:#000000;\
                                   border-radius:5px;\
                                   font:bold 12px;\
                               }");
    m_pLabel->setStyleSheet(QString("QLabel{font:bold %1px;color: #000000;font-family:Microsoft YaHei;}")
                            .arg(int(18*instance.zoom() + 1)));
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(m_pGroupBox);
    QVBoxLayout *gLayout = new QVBoxLayout(m_pGroupBox);
    gLayout->addWidget(m_pChartView);
    gLayout->addWidget(m_pLabel, 0, Qt::AlignHCenter);
    // 消除重影
    m_pChartView->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    // 标题
    m_pChartView->chart()->setTitle(QString("%1(%2)").arg(m_pCustomCtrl->getArgItems().dataName,
                                                          m_pCustomCtrl->getArgItems().dataUnit));
    m_pChartView->chart()->setTitleFont(m_pLabel->font());
    m_pChartView->chart()->setBackgroundVisible(false);
    // 图形边距
    m_pChartView->chart()->setContentsMargins(0, 0, 0, 0);
    m_pChartView->chart()->layout()->setContentsMargins(0, 0, 0, 0);

    m_pChartView->chart()->addSeries(m_pSeries);
    m_pAxisX->setFormat("hh:mm:ss");
    m_pAxisX->setTickCount(2);
    m_pAxisY->setTickCount(3);
    m_pAxisY->setRange(m_pCustomCtrl->getMinValue(), m_pCustomCtrl->getMaxValue());
    // 精度
    if (digit == 0) {
        m_pAxisY->setLabelFormat("%d");
    }
    else {
        m_pAxisY->setLabelFormat(QString("%.%1f").arg(digit));
    }
    m_pChartView->chart()->addAxis(m_pAxisX, Qt::AlignBottom);
    m_pChartView->chart()->addAxis(m_pAxisY, Qt::AlignLeft);
    m_pSeries->attachAxis (m_pAxisX);
    m_pSeries->attachAxis (m_pAxisY);
    m_pSeries->setPen(QPen(Qt::blue, 1, Qt::SolidLine));

    m_pSeries->setUseOpenGL(true);
    m_pChartView->chart()->legend()->hide();

    connect(dialog, &SelectItemDialog::currentType, this, &TrendChart::getChangeType);
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
    m_pAxisX->setRange(QDateTime::fromMSecsSinceEpoch(0), QDateTime::fromMSecsSinceEpoch(1));
    m_values.clear();
    m_pLabel->clear();
}

void TrendChart::addValue(double value)
{
    QDateTime cTime = QDateTime::currentDateTime();
    if (m_pAxisX->min().toSecsSinceEpoch() == 0) {
        m_pAxisX->setMin(cTime);
    }
    m_values.append(value);
    double max = *max_element(m_values.begin(), m_values.end());
    double min = *min_element(m_values.begin(), m_values.end());
    double avg = accumulate(m_values.begin(), m_values.end(), 0.0)/m_values.size();
    m_pLabel->setText(QString("MAX: %1; MIN: %2; AVG: %3;").arg(QString::number(max, 'f', digit),
                                                                QString::number(min, 'f', digit),
                                                                QString::number(avg, 'f', digit)));
    if (m_pAxisY->min() != min - 0.1*qAbs(min) || m_pAxisY->max() != max + 0.1*qAbs(max)) {
        m_pAxisY->setRange(min - 0.1*qAbs(min), max + 0.1*qAbs(max));
    }
    if (m_pAxisX->min() != cTime) {
        m_pAxisX->setMax(cTime);
    }
    else {
        m_pAxisX->setMax(cTime.addMSecs(1000));
    }
    m_pSeries->append(cTime.toMSecsSinceEpoch(), value);
}

void TrendChart::getChangeType(const Type &type)
{
    auto &instance = DataManagement::getInstance();
    emit changeType(m_pCustomCtrl->getType(), type);
    instance.getRegulator()->saveTypes(instance.getPaths().trendCharts(), instance.getRegulator()->getCurrentNames(true));
}
