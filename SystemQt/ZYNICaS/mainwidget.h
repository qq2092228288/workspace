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
//#include "showreportdialog.h"


class MainWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MainWidget(QWidget *parent = nullptr);
    virtual ~MainWidget();
    virtual void paintEvent(QPaintEvent *event);
public slots:
    void newVersion(const QJsonObject &object);
//    bool nativeEvent(const QByteArray &eventType, void *message, long *result);
signals:
    // Windows 消息，设备插拔 wParam(0x8000)已插入设备或介质，现在可用。wParam(0x8004)设备或介质已被移出。
//    void wmDeviceChange(bool insert);
private slots:
    void enterBtnSlot();
    void demoBtnSlot();
    void createdReportSlot(const qint64 &timestamp, const QString &baseDataString);
private:
    QVBoxLayout *mainLayout;
    QLabel *titleLabel;
    QButtonGroup *btnGroup;
    QPushButton *enterBtn;          //进入系统
    QPushButton *reportBtn;         //查看报告
    QPushButton *demoBtn;           //演示模式
    QPushButton *configBtn;         //系统配置
    QPushButton *exitBtn;           //退出系统

//    ShowReportDialog *reportDialog;
    SystemConfigDialog *configDialog;
    EnterSystemWidget *enterSystemWidget;
};
typedef QSharedPointer<MainWidget> MainWidget_PTR;

#endif // MAINWIDGET_H
