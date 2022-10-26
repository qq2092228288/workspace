#include "drawwaveforms.h"
#include <iostream>
#include <algorithm>
using namespace std;

DrawWaveforms::DrawWaveforms(bool setY, QObject *parent)
    : QObject{parent}
{
    autoSetY = setY;
    m_pView = new QChartView;
    m_pChart = new QChart;
    m_pAxisX = new QValueAxis(this);
    m_pAxisY = new QValueAxis(this);
    m_pSeries = new QLineSeries(this);
    // 背景样式
    m_pView->setStyleSheet(R"(QGraphicsView{ background-color:#ffffff;
                                border-radius:5px;border-width:1px;border-color:#000000;})");
    m_pChart->setBackgroundVisible(false);
    // 图形边距
    m_pChart->setContentsMargins(-30, -10, 0, -10);
    m_pChart->layout()->setContentsMargins(0, 0, 0, 0);
    // 坐标轴位置
    m_pChart->addSeries(m_pSeries);
    m_pChart->addAxis(m_pAxisX,Qt::AlignBottom);
    m_pChart->addAxis(m_pAxisY,Qt::AlignLeft);
    // 刻度
    m_pAxisX->setRange(0,m_tickCout);
    m_pAxisY->setRange(0,255);
    m_pAxisY->setTickCount(4);
    // 刻度不可见
    m_pAxisX->setVisible(false);
    m_pAxisY->setLabelsVisible(false);
    // 关联坐标轴
    m_pSeries->attachAxis(m_pAxisX);
    m_pSeries->attachAxis(m_pAxisY);
    // 波形样式
    m_pSeries->setPen(QPen(Qt::blue,2,Qt::SolidLine));

    m_pSeries->setUseOpenGL(true);

    m_pChart->legend()->hide();

    m_pView->setChart(m_pChart);
}

DrawWaveforms::~DrawWaveforms()
{
    delete m_pChart;
    delete m_pView;
//    qDebug()<<"~DrawWaveforms()";
}

QChartView *DrawWaveforms::getView() const
{
    return this->m_pView;
}

void DrawWaveforms::addValue(uchar value)
{
    m_values.append(QPointF(++m_x,value));
    m_oldValues.enqueue(value);
    if (m_x%6 == 0) {
        m_pSeries->replace(m_values);
        if(m_x >= m_tickCout+10) {
            m_pAxisX->setRange(m_x-m_tickCout,m_x);
            m_values.removeFirst();
            m_oldValues.dequeue();
            if(autoSetY) {
                m_pAxisY->setRange((*min_element(m_oldValues.begin(),m_oldValues.end()))/30*30-30,
                                   (*max_element(m_oldValues.begin(),m_oldValues.end()))/30*30+30);
                m_pView->update();
            }
        }
    }
}

void DrawWaveforms::clear()
{
    m_pSeries->clear();
    m_values.clear();
    m_oldValues.clear();
    m_pAxisX->setRange(0,m_tickCout);
    m_pAxisY->setRange(0,255);
    m_x = 0;
}
