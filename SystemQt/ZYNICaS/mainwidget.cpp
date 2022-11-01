#include "mainwidget.h"
#include "datamanagement.h"

MainWidget::MainWidget(QWidget *parent)
    : QWidget{parent}
{
    setWindowTitle(tr("泽耀无创血流动力学检测系统"));
//    setStyleSheet("background-color:white;");
    auto &instance = DataManagement::getInstance();
    setMinimumSize(1600*instance.wZoom(),900*instance.hZoom());
    mainLayout = new QVBoxLayout(this);
    titleLabel = new QLabel(tr("无创血流动力学检测系统"),this);
    btnGroup = new QButtonGroup(this);
    enterBtn = new QPushButton(tr("进入系统"),this);
    reportBtn = new QPushButton(tr("查看报告"),this);
    demoBtn = new QPushButton(tr("演示模式"),this);
    configBtn = new QPushButton(tr("系统配置"),this);
    exitBtn = new QPushButton(tr("退出系统"),this);

    reportDialog = new ShowReportDialog;
    configDialog = new SystemConfigDialog;
    enterSystemWidget = new EnterSystemWidget(configDialog->getPortName());
    QObject::connect(enterSystemWidget,&EnterSystemWidget::widgetClose,this,&EnterSystemWidget::show);

    btnGroup->addButton(enterBtn);
    btnGroup->addButton(reportBtn);
    btnGroup->addButton(demoBtn);
    btnGroup->addButton(configBtn);
    btnGroup->addButton(exitBtn);

    //布局
    mainLayout->addStretch();
    mainLayout->addWidget(titleLabel,0,Qt::AlignCenter);
    mainLayout->addStretch();
    mainLayout->addWidget(enterBtn,0,Qt::AlignCenter);
    mainLayout->addWidget(reportBtn,0,Qt::AlignCenter);
    mainLayout->addWidget(demoBtn,0,Qt::AlignCenter);
    mainLayout->addWidget(configBtn,0,Qt::AlignCenter);
    mainLayout->addWidget(exitBtn,0,Qt::AlignCenter);
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
    connect(enterBtn,&QPushButton::clicked,this,&MainWidget::enterBtnSlot);
    connect(reportBtn,&QPushButton::clicked,reportDialog,&ShowReportDialog::exec);
    connect(demoBtn,&QPushButton::clicked,this,&MainWidget::demoBtnSlot);
    connect(configBtn,&QPushButton::clicked,configDialog,&SystemConfigDialog::exec);
    connect(exitBtn,&QPushButton::clicked,this,&MainWidget::close);
}

MainWidget::~MainWidget()
{
    delete reportDialog;
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
    painter.drawPixmap(rect(),QPixmap(":/images/background.jpg"));
    painter.drawPixmap(10,10,pixmap.scaled(pixmap.size()*instance.zoom()));
}

void MainWidget::enterBtnSlot()
{
    if (IdCheck::getCurrentConsumables() <= 0) {
        QMessageBox::warning(this,tr("警告！"),tr("有效验证码已使用完，请联系厂家！"));
        return;
    }
    foreach (QSerialPortInfo info, QSerialPortInfo::availablePorts()) {
        if(enterSystemWidget->currentPortName() == info.portName() ) {
            emit enterSystemWidget->startDemoMode(false);
            if(disconnect(enterSystemWidget, &EnterSystemWidget::startDemoMode,
                    DataManagement::getInstance().getTebco(), &ZyTebco::startDemoMode)) {
                DataManagement::getInstance().clearSlot();
                enterSystemWidget->clearUiSlot();
            }
            connect(&DataManagement::getInstance(),&DataManagement::startCheck,
                    DataManagement::getInstance().getTebco(),&ZyTebco::startCheckSlot,
                    Qt::ConnectionType(Qt::AutoConnection|Qt::UniqueConnection));
            connect(enterSystemWidget, &EnterSystemWidget::createdReport,
                    DataManagement::getInstance().getIdCheck(), &IdCheck::consumablesUsed,
                    Qt::ConnectionType(Qt::AutoConnection|Qt::UniqueConnection));
            this->close();
            enterSystemWidget->show();
            return;
        }
    }
    QMessageBox::warning(this,tr("警告！"),tr("串口'%1'打开失败，请检查配置并重启程序！！！")
                         .arg(enterSystemWidget->currentPortName()));
}

void MainWidget::demoBtnSlot()
{
    connect(enterSystemWidget, &EnterSystemWidget::startDemoMode,
            DataManagement::getInstance().getTebco(), &ZyTebco::startDemoMode,
            Qt::ConnectionType(Qt::AutoConnection|Qt::UniqueConnection));
    if (disconnect(&DataManagement::getInstance(),&DataManagement::startCheck,
            DataManagement::getInstance().getTebco(),&ZyTebco::startCheckSlot)) {
        DataManagement::getInstance().getTebco()->startCheckSlot(false);
        DataManagement::getInstance().clearSlot();
        enterSystemWidget->clearUiSlot();
    }
    disconnect(enterSystemWidget, &EnterSystemWidget::createdReport,
            DataManagement::getInstance().getIdCheck(), &IdCheck::consumablesUsed);
    this->close();
    enterSystemWidget->show();
}
