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


class GetIdDialog : public QDialog
{
    Q_OBJECT
public:
    explicit GetIdDialog(QWidget *parent = nullptr);
    void showEvent(QShowEvent *event);
public slots:
//    void hideCreateDevice(bool isHide);
signals:
    void requestDeviceOnlineNotice(const QString &deviceId);
private slots:
//    void setConsumables(const int &count);
    // 复制MAC
    void copySlot();
    // 离线获取
//    void usedSlot();
    // 创建设备
//    void createDeviceSlot();
    // 在线获取
    void onlineGetSlot();
private:
    QLabel *consumablesLabel;
    QLabel *deviceIdLabel;
    QLabel *macLabel;
//    QLabel *codeLabel;
    QLineEdit *consumablesEdit;
    QLineEdit *deviceIdEdit;
    QLineEdit *macEdit;
//    QLineEdit *codeEdit;
    QPushButton *copyBtn;
//    QPushButton *usedBtn;
//    QLabel *deviceNameLabel;
//    QLineEdit *deviceNameLineEdit;
//    QPushButton *createDeviceBtn;
//    QPushButton *onlineGetBtn;
};

#endif // GETIDDIALOG_H
