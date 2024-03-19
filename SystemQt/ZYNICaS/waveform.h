#ifndef WAVEFORM_H
#define WAVEFORM_H

#include <QWidget>
#include <QPainter>

class Waveform : public QWidget
{
    Q_OBJECT
public:
    explicit Waveform(QWidget *parent = nullptr);
public slots:
    void clear();
signals:
protected:
    void paintEvent(QPaintEvent *event) override;
};

#endif // WAVEFORM_H
