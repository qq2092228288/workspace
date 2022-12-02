#ifndef GETIDDIALOG_H
#define GETIDDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QGridLayout>
#include <QNetworkInterface>
#include <QClipboard>
#include <QMessageBox>
#include "devicedatabase.h"

class HttpPost;
struct DeviceInfo;
class GetIdDialog : public QDialog
{
    Q_OBJECT
public:
    explicit GetIdDialog(QWidget *parent = nullptr);
    DeviceDatabase *getGeviceDatabase();
signals:
private slots:
    void setConsumables(const int &count);
    void copySlot();
//    void usedSlot();
    // 创建设备
    void createDeviceSlot();
    // 在线获取
    void onlineGetSlot();
    // 接收设备信息
    void receiveDeviceInfo(const DeviceInfo &deviceInfo);
    // 接收耗材
//    void receiveConsumableCount(const QString &id, const int &totalCount);
private:
    DeviceDatabase m_deviceDatabase;
    QLabel *consumablesLabel;
    QLabel *macLabel;
    QLabel *codeLabel;
    QLineEdit *consumablesEdit;
    QLineEdit *macEdit;
    QLineEdit *codeEdit;
    QPushButton *copyBtn;
    QPushButton *usedBtn;
    QPushButton *createDeviceBtn;
    QPushButton *onlineGetBtn;
};

#endif // GETIDDIALOG_H
