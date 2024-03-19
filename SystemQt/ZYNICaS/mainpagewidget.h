#ifndef MAINPAGEWIDGET_H
#define MAINPAGEWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QRadioButton>
#include <QPushButton>
#include <QGroupBox>
#include <QGridLayout>
#include <QComboBox>
#include <QStackedWidget>
#include <QButtonGroup>
#include <QFile>

class MainPageWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MainPageWidget(QWidget *parent = nullptr);

signals:
private:
    QHBoxLayout *mainLayout;
};

#endif // MAINPAGEWIDGET_H
