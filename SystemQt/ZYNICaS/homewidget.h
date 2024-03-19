#ifndef HOMEWIDGET_H
#define HOMEWIDGET_H

#include <QWidget>
#include <QStyle>
#include <QHBoxLayout>
#include <QPushButton>
#include <QStackedWidget>

class HomeWidget : public QWidget
{
    Q_OBJECT
public:
    explicit HomeWidget(QWidget *parent = nullptr);

signals:
private:
    QStackedWidget *stack;
};

#endif // HOMEWIDGET_H
