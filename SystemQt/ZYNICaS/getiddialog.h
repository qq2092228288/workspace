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


class HttpPost;
class DeviceInfo;
class GetIdDialog : public QDialog
{
    Q_OBJECT
public:
    explicit GetIdDialog(QWidget *parent = nullptr);
    void showEvent(QShowEvent *event);
signals:
private slots:
    void setConsumables(const int &count);
    void copySlot();
//    void usedSlot();
    // 创建设备
    void createDeviceSlot();
    // 在线获取
    void onlineGetSlot();
private:
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
