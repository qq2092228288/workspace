#include "mainwidget.h"
#include "datamanagement.h"
//#include "idcheck.h"
#include "updateappdialog.h"
#include "viewreportdialog.h"

MainWidget::MainWidget(QWidget *parent)
    : QWidget{parent}
{
    setWindowTitle(tr("芫泽无创血流动力学检测系统"));
    auto &instance = DataManagement::getInstance();
    setMinimumSize(1600*instance.wZoom(), 900*instance.hZoom());
    mainLayout = new QVBoxLayout(this);
    titleLabel = new QLabel(tr("无创血流动力学检测系统"), this);
    btnGroup = new QButtonGroup(this);
    enterBtn = new QPushButton(tr("进入系统"), this);
    reportBtn = new QPushButton(tr("查看报告"), this);
    demoBtn = new QPushButton(tr("演示模式"), this);
    configBtn = new QPushButton(tr("系统配置"), this);
    exitBtn = new QPushButton(tr("退出系统"), this);

//    reportDialog = new ShowReportDialog;
    configDialog = new SystemConfigDialog;
    enterSystemWidget = new EnterSystemWidget;
    QObject::connect(enterSystemWidget, &EnterSystemWidget::widgetClose, this, &EnterSystemWidget::showMaximized);

    btnGroup->addButton(enterBtn);
    btnGroup->addButton(reportBtn);
    btnGroup->addButton(demoBtn);
    btnGroup->addButton(configBtn);
    btnGroup->addButton(exitBtn);

    //布局
    mainLayout->addStretch();
    mainLayout->addWidget(titleLabel, 0, Qt::AlignCenter);
    mainLayout->addStretch();
    mainLayout->addWidget(enterBtn, 0, Qt::AlignCenter);
    mainLayout->addWidget(reportBtn, 0, Qt::AlignCenter);
    mainLayout->addWidget(demoBtn, 0, Qt::AlignCenter);
    mainLayout->addWidget(configBtn, 0, Qt::AlignCenter);
    mainLayout->addWidget(exitBtn, 0, Qt::AlignCenter);
    mainLayout->addStretch();

    //样式表
    titleLabel->setStyleSheet("QLabel {background-color: transparent;color:#1296db;}");
    QFont lfont = instance.font(110,tr("华文行楷"));
    lfont.setBold(true);
    titleLabel->setFont(lfont);
    QString btnQss = QString("QPushButton{\
    color:#ffffff;\
    background-color:qlineargradient(x1:0,y1:0,x2:0,y2:1,stop:0 #aa55ff,stop:1 #1296db);\
    border-style:outset;\
    border-width:2px;\
    border-color:#0055ff;\
    border-radius:10px;\
    padding:4px;}\
    QPushButton:hover{\
    background-color:qlineargradient(x1:0,y1:0,x2:0,y2:1,stop:0 #ff55ff,stop:1 #1296db);}\
    QPushButton:pressed{\
    background-color:qlineargradient(x1:0,y1:0,x2:0,y2:1,stop:0 #ff55ff,stop:1 #aa00ff);}");

    QFont font = instance.font(50,"Microsoft YaHei");
    font.setBold(true);
    QSize size = instance.rectSize(300,100);
    foreach(auto btn, btnGroup->buttons())
    {
        btn->setStyleSheet(btnQss);
        btn->setFont(font);
        btn->setFixedSize(size);
    }

    //按键槽函数关联
    connect(enterBtn, &QPushButton::clicked, this, &MainWidget::enterBtnSlot);
    connect(reportBtn, &QPushButton::clicked, this, [=](){
        auto dialog = new ViewReportDialog;
        dialog->exec();
        dialog->deleteLater();
    });
    connect(demoBtn, &QPushButton::clicked, this, &MainWidget::demoBtnSlot);
    connect(configBtn, &QPushButton::clicked, configDialog, &SystemConfigDialog::exec);
    connect(exitBtn, &QPushButton::clicked, this, &MainWidget::close);

    connect(configDialog, &SystemConfigDialog::modeChanged, enterSystemWidget, &EnterSystemWidget::systemModeChanged);
//    connect(this, &MainWidget::wmDeviceChange, instance.getTebco(), &ZyTebco::hotPlug);
    configDialog->updateHospitalInfo();
    // 未show控件截屏问题处理
//    enterSystemWidget->trendChartLayout();
}

MainWidget::~MainWidget()
{
//    delete reportDialog;
    delete configDialog;
    delete enterSystemWidget;
    //    qDebug()<<"~MainWidget()";
}

void MainWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    auto &instance = DataManagement::getInstance();
    QPainter painter(this);
    QPixmap pixmap = QPixmap(":/images/logo.png");
    painter.drawPixmap(rect(), QPixmap(":/images/background.jpg"));
    painter.drawPixmap(10, 10, pixmap.scaled(pixmap.size()*instance.zoom()));
}

void MainWidget::newVersion(const QJsonObject &object)
{
    UpdateAppDialog dialog(object);
    connect(&dialog, &UpdateAppDialog::installNewApp, &dialog, &UpdateAppDialog::close);
    connect(&dialog, &UpdateAppDialog::installNewApp, this, &MainWidget::close);
    dialog.exec();
}

//bool MainWidget::nativeEvent(const QByteArray &eventType, void *message, long *result)
//{
//    Q_UNUSED(eventType);
//    Q_UNUSED(result);
//    MSG* msg = reinterpret_cast<MSG*>(message);
//    if (msg->message == WM_DEVICECHANGE)    // 发生硬件热插拔事件
//    {
//        if (msg->wParam == 0x8000) {
//            emit wmDeviceChange(true);
//        }
//        else if (msg->wParam == 0x8004) {
//            emit wmDeviceChange(false);
//        }
//    }
//    return false;
//}

void MainWidget::enterBtnSlot()
{
//    int surplus = DataManagement::getInstance().surplus();
//    if (surplus <= 0) {
//        QMessageBox::warning(this, tr("警告！"), tr("有效验证码已使用完，请联系厂家！"));
//        return;
//    }
//    else if (surplus <= 20) {
//        QMessageBox::warning(this, tr("警告！"), tr("有效验证码剩余 %1 ，请注意！").arg(surplus));
//    }
    auto surplus = DataManagement::getInstance().mqttClient()->surplus();
    if (surplus <= 0) {
        QMessageBox::warning(this, tr("警告！"), tr("有效验证码已使用完，请联系厂家！"));
        return;
    }
    else if (surplus <= 20) {
        QMessageBox::warning(this, tr("警告！"), tr("有效验证码剩余 %1 ，请注意！").arg(surplus));
    }
    foreach (QSerialPortInfo info, QSerialPortInfo::availablePorts()) {
        if(configDialog->getPortName() == info.portName()) {
            emit enterSystemWidget->startDemoMode(false);
            if(disconnect(enterSystemWidget, &EnterSystemWidget::startDemoMode,
                    DataManagement::getInstance().getTebco(), &ZyTebco::startDemoMode)) {
                DataManagement::getInstance().clearSlot();
                enterSystemWidget->clearUiSlot();
            }
            // connect(&DataManagement::getInstance(), &DataManagement::startCheck,
            //         DataManagement::getInstance().getTebco(), &ZyTebco::startWriteF,
            //         Qt::ConnectionType(Qt::AutoConnection|Qt::UniqueConnection));
            connect(&DataManagement::getInstance(), &DataManagement::startCheck,
                    DataManagement::getInstance().getTebco(), &ZyTebco::startCheckSlot,
                    Qt::ConnectionType(Qt::AutoConnection|Qt::UniqueConnection));
            connect(enterSystemWidget, &EnterSystemWidget::createdReport, this, &MainWidget::createdReportSlot,
                    Qt::ConnectionType(Qt::AutoConnection|Qt::UniqueConnection));
            this->close();
            enterSystemWidget->showMaximized();
            // 打开串口
            emit DataManagement::getInstance().sendSerialName(info.portName());
            return;
        }
    }
    QMessageBox::warning(this, tr("警告！"), tr("串口“%1”打开失败，请检查装置接入的串口是否和“系统配置”一致！")
                         .arg(configDialog->getPortName()));
}

void MainWidget::demoBtnSlot()
{
    connect(enterSystemWidget, &EnterSystemWidget::startDemoMode,
            DataManagement::getInstance().getTebco(), &ZyTebco::startDemoMode,
            Qt::ConnectionType(Qt::AutoConnection|Qt::UniqueConnection));
    if (disconnect(&DataManagement::getInstance(), &DataManagement::startCheck,
            DataManagement::getInstance().getTebco(), &ZyTebco::startCheckSlot)) {
        DataManagement::getInstance().getTebco()->startCheckSlot(false);
        DataManagement::getInstance().clearSlot();
        enterSystemWidget->clearUiSlot();
    }
    // disconnect(&DataManagement::getInstance(), &DataManagement::startCheck,
    //         DataManagement::getInstance().getTebco(), &ZyTebco::startWriteF);
    disconnect(enterSystemWidget, &EnterSystemWidget::createdReport, this, &MainWidget::createdReportSlot);
    this->close();
    enterSystemWidget->showMaximized();
}

void MainWidget::createdReportSlot(const qint64 &timestamp, const QString &baseDataString)
{
//    auto &instance = DataManagement::getInstance();
//    instance.reportDataBase()->insert(QDateTime::currentMSecsSinceEpoch(), 0, baseDataString);
//    IdCheck idCheck(this);
//    if (idCheck.getCurrentConsumables() <= 0) {
//        instance.deviceDatabase()->offlineUsed();
//    }
//    else {
//        idCheck.consumablesUsed();
//    }
//    int surplus = instance.surplus();
//    if (surplus <= 0) {
//        QMessageBox::warning(this, tr("警告！"), tr("有效验证码已使用完，请联系厂家！"));
//        enterSystemWidget->hide();
//        this->show();
//    }
//    else if (surplus <= 20) {
//        QMessageBox::warning(this, tr("警告！"), tr("有效验证码剩余 %1 ，请注意！").arg(surplus));
//    }
    auto client = DataManagement::getInstance().mqttClient();
    client->insert(timestamp, baseDataString);
    auto surplus = client->surplus();
    if (surplus <= 0) {
        QMessageBox::warning(this, tr("警告！"), tr("有效验证码已使用完，请联系厂家！"));
        enterSystemWidget->hide();
        this->show();
    }
    else if (surplus <= 20) {
        QMessageBox::warning(this, tr("警告！"), tr("有效验证码剩余 %1 ，请注意！").arg(surplus));
    }
}

