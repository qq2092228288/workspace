#ifndef DRAWWAVEFORMS_H
#define DRAWWAVEFORMS_H

#include <QObject>
#include <QChartView>
#include <QChart>
#include <QtCharts>
#include <QValueAxis>
#include <QLineSeries>
#include <QQueue>


class DrawWaveforms : public QObject
{
    Q_OBJECT
public:
    explicit DrawWaveforms(bool setY = false, QObject *parent = nullptr);
    virtual ~DrawWaveforms();
    QChartView *getView() const;
public slots:
    void addValue(uchar value);
    void clear();
signals:

private:
    bool autoSetY;
    uint m_tickCout = 120;
    QList<QPointF> m_values;
    QQueue<uchar> m_oldValues;
    uint m_x = 0;
    QChartView *m_pView;
    QChart *m_pChart;
    QValueAxis *m_pAxisX;
    QValueAxis *m_pAxisY;
    QLineSeries *m_pSeries;
};

#endif // DRAWWAVEFORMS_H
