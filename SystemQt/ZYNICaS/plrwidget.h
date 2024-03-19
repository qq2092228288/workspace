#ifndef PLRWIDGET_H
#define PLRWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QProgressBar>
#include <QGridLayout>
#include <QPainter>
#include <QGroupBox>
#include <QComboBox>
#include <QStackedWidget>

class PlrWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PlrWidget(QWidget *parent = nullptr);
public slots:
    void initWidget();
signals:
    void stop();
protected:
    void paintEvent(QPaintEvent *event) override;
};

#endif // PLRWIDGET_H
