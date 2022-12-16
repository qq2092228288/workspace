#include "getiddialog.h"
#include "datamanagement.h"
#include "httppost.h"

GetIdDialog::GetIdDialog(QWidget *parent)
    : QDialog{parent}
{
    auto &instance = DataManagement::getInstance();
    setFixedSize(500*instance.wZoom(),300*instance.hZoom());
    this->setWindowTitle(tr("有效验证码"));
    QGridLayout *mainLayout = new QGridLayout(this);
    consumablesLabel = new QLabel(tr("剩余有效验证码:"), this);
    macLabel = new QLabel(tr("MAC地址:"), this);
//    codeLabel = new QLabel(tr("有效验证码:"), this);
    consumablesEdit = new QLineEdit(this);
    macEdit = new QLineEdit(this);
//    codeEdit = new QLineEdit(this);
    copyBtn = new QPushButton(tr("复制"), this);
//    usedBtn = new QPushButton(tr("使用"), this);
    deviceNameLabel = new QLabel(tr("设备信息:"), this);
    deviceNameLineEdit = new QLineEdit(this);
    createDeviceBtn = new QPushButton(tr("创建设备"), this);
    onlineGetBtn = new QPushButton(tr("在线获取"), this);
    // 最大长度100
    deviceNameLineEdit->setMaxLength(100);

    QHBoxLayout *deviceLayout = new QHBoxLayout;

    deviceLayout->addWidget(deviceNameLineEdit);
    deviceLayout->addWidget(createDeviceBtn);
    mainLayout->addWidget(consumablesLabel, 0, 0);
    mainLayout->addWidget(consumablesEdit, 0, 1);
    mainLayout->addWidget(macLabel, 1, 0);
    mainLayout->addWidget(macEdit, 1, 1);
    mainLayout->addWidget(copyBtn, 1, 2);
//    mainLayout->addWidget(codeLabel, 2, 0);
//    mainLayout->addWidget(codeEdit, 2, 1);
//    mainLayout->addWidget(usedBtn, 2, 2);
    mainLayout->addWidget(deviceNameLabel, 3, 0);
    mainLayout->addLayout(deviceLayout, 3, 1);
    mainLayout->addWidget(onlineGetBtn, 3, 2);

    consumablesEdit->setFixedWidth(100);
    consumablesEdit->setReadOnly(true);
    consumablesEdit->setAlignment(Qt::AlignCenter);
    macEdit->setReadOnly(true);
    macEdit->setAlignment(Qt::AlignCenter);

    macEdit->setText(instance.getMac());

    connect(copyBtn, &QPushButton::clicked, this, &GetIdDialog::copySlot);
//    connect(usedBtn, &QPushButton::clicked, this, &GetIdDialog::usedSlot);
    connect(createDeviceBtn, &QPushButton::clicked, this, &GetIdDialog::createDeviceSlot);
    connect(onlineGetBtn, &QPushButton::clicked, this, &GetIdDialog::onlineGetSlot);

//    connect(instance.getIdCheck(), &IdCheck::currentConsumables, this, &GetIdDialog::setConsumables);

    connect(instance.httpPost(), &HttpPost::deviceCreated, this, [=](const QString &tip){
        QMessageBox::warning(this, tr("提示"), tip);
    });
    connect(instance.httpPost(), &HttpPost::getNewBatch, this, [=](const int &increase){
        int surplus = DataManagement::getInstance().deviceDatabase()->getConsumableSurplus();
        consumablesEdit->setText(QString::number(surplus));
        QMessageBox::information(this, tr("有效验证码"), tr("获取 %1，剩余 %2。")
                                 .arg(increase).arg(surplus));
    });
    if (!instance.deviceDatabase()->databaseIntegralityCheck()) {
        QMessageBox::warning(this, tr("错误！"), tr("关键文件被非法修改，程序启动失败！"));
        exit(0);
    }
}

void GetIdDialog::showEvent(QShowEvent *event)
{
    consumablesEdit->setText(QString::number(DataManagement::getInstance().deviceDatabase()->getConsumableSurplus()));
    event->accept();
}

void GetIdDialog::setConsumables(const int &count)
{
    consumablesEdit->setText(QString::number(count));
    if (count <= 20 && count > 0) {
        QMessageBox::warning(this,tr("警告！"), tr("有效验证码剩余：%1 ，请注意！").arg(count));
    }
    else if(count == 0) {
        QMessageBox::warning(this,tr("警告！"), tr("有效验证码已使用完，请联系厂家！"));
    }
}

void GetIdDialog::copySlot()
{
    QApplication::clipboard()->setText(macEdit->text());
}

//void GetIdDialog::usedSlot()
//{
//    if (codeEdit->text().isEmpty()) {
//        QMessageBox::warning(this,tr("提示"),tr("有效验证码不能为空！"));
//        return;
//    }
//    auto idCheck = DataManagement::getInstance().getIdCheck();
//    QByteArray array = idCheck->strToByteArray(codeEdit->text());
//    switch (idCheck->modifyConfig(array, DataManagement::getInstance().getMac())) {
//    case ZipError::Validity:
//        QMessageBox::information(this,tr("提示"),tr("有效验证码增加%1，剩余%2")
//                                 .arg(idCheck->getCodeCount(array, DataManagement::getInstance().getMac()))
//                                 .arg(idCheck->getCurrentConsumables()));
//        break;
//    case ZipError::CodeError:
//        QMessageBox::warning(this,tr("警告！"),tr("有效验证码不正确！"));
//        break;
//    case ZipError::Overdue:
//        QMessageBox::warning(this,tr("警告！"),tr("有效验证码过期！"));
//        break;
//    case ZipError::Used:
//        QMessageBox::warning(this,tr("警告！"),tr("有效验证码已经被使用！"));
//        break;
//    }
//}

void GetIdDialog::createDeviceSlot()
{
    if (deviceNameLineEdit->text().isEmpty()) {
        QMessageBox::warning(this, tr("提示！"), tr("请入设备信息。"));
        return ;
    }
    auto &instance = DataManagement::getInstance();
    instance.httpPost()->createDevice(instance.getMac(), deviceNameLineEdit->text());
//    instance.httpPost()->activeDevice(instance.getMac());
}

void GetIdDialog::onlineGetSlot()
{
    auto &instance = DataManagement::getInstance();
    instance.requestConsumableList();
    if (instance.httpPost()->deviceOnlineNotice(instance.deviceDatabase()->getDeviceInfo("deviceId"))) {
        QMessageBox::information(this, tr("提示！"), tr("已获取最新数据。"));
    }
    else {
        QMessageBox::warning(this, tr("提示！"), tr("获取失败！"));
    }
}
