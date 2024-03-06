#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QTextBrowser>
#include <QGridLayout>
#include <QRadioButton>
#include <QGroupBox>

#include "icgcontrolmodule.h"

class MainWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MainWidget(QWidget *parent = nullptr);
    ~MainWidget();
signals:
    void readyClose();
protected:
    void closeEvent(QCloseEvent *event) override;
private:
    QString ntos(const float &value) const;
private:
    IcgControlModule *module;
};

#endif // MAINWIDGET_H
