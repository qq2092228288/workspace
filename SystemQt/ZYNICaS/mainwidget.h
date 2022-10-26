#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QGridLayout>
#include <QButtonGroup>
#include <QDebug>
#include <QDesktopWidget>
#include <QApplication>
#include <QMessageBox>
#include <QSerialPortInfo>

#include "entersystemwidget.h"
#include "systemconfigdialog.h"
#include "showreportdialog.h"

class MainWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MainWidget(QWidget *parent = nullptr);
    virtual ~MainWidget();
    virtual void paintEvent(QPaintEvent *event);
signals:

private slots:
    void enterBtnSlot();
    void demoBtnSlot();
private:
    QVBoxLayout *mainLayout;
    QLabel *titleLabel;
    QButtonGroup *btnGroup;
    QPushButton *enterBtn;          //进入系统
    QPushButton *reportBtn;         //查看报告
    QPushButton *demoBtn;           //演示模式
    QPushButton *configBtn;         //系统配置
    QPushButton *exitBtn;           //退出系统

    ShowReportDialog *reportDialog;
    SystemConfigDialog *configDialog;
    EnterSystemWidget *enterSystemWidget;
};

#endif // MAINWIDGET_H
