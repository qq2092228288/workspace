#include "entersystemwidget.h"
#include "datamanagement.h"
#include "waitingdialog.h"
#include <iostream>
#include <numeric>
#include <math.h>
using namespace std;

EnterSystemWidget::EnterSystemWidget(const QString &portName, QWidget *parent)
    : QWidget{parent}
{
    DataManagement &instance = DataManagement::getInstance();
    setMinimumSize(1600*instance.wZoom(),900*instance.hZoom());
    setWindowTitle(tr("泽耀无创血流动力学检测"));
    this->portName = portName;

    auto tebco = instance.getTebco();
    tebco->openSerial(portName);    //打开串口

    //样式表
    int fsize = 16*instance.zoom()+1;         //字体大小
    int gsize = 20*instance.zoom()+1;
    QString qss = QString("QLabel\
    {\
        color:#ffffff;\
        font:bold %1px;\
        font-family:Microsoft YaHei;\
    }\
    QPushButton\
    {\
        color:#ffffff;\
        background-color:#778899;\
        border-style:outset;\
        border-width:1px;\
        border-color:#000000;\
        border-radius: 3px;\
        padding:2px;\
        font:bold %2px;\
    }\
    QPushButton:hover\
    {\
        background-color:qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop:0 #778899, stop: 1 #bebebe);\
    }\
    QPushButton:pressed\
    {\
        background-color:qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop:0 #d3d3d3, stop: 1 #708090);\
    }\
    QGroupBox\
    {\
        border:2px solid #cfcfcf;\
        margin-top: 2ex;\
        color:#ffffff;\
        border-radius:3px;\
        font:bold %3px;\
    }\
    QGroupBox::title\
    {\
        subcontrol-origin: margin;\
        subcontrol-position: top center;\
        padding: 0 3px;\
    }\
    EnterSystemWidget\
    {\
        background-color:#1c1c1c;\
    }\
    QRadioButton\
    {\
        color:#ffffff;\
        font:bold %4px;\
    }\
    QLineEdit\
    {\
        color:#000000;\
        font:bold %5px;\
        background-color:#bebebe;\
        border-radius: 3px;\
    }\
    QMessageBox QLabel\
    {\
        color:#000000;\
    }").arg(fsize).arg(fsize).arg(gsize).arg(fsize).arg(fsize);
    this->setStyleSheet(qss);

    mainLayout = new QGridLayout(this);
    firstColLayout = new QVBoxLayout;
    secColLayout = new QVBoxLayout;
    mainLayout->addLayout(firstColLayout,0,0);
    mainLayout->addLayout(secColLayout,0,1);

    timer = new QTimer(this);
    connect(timer,&QTimer::timeout,this,&EnterSystemWidget::timeoutSlot);
    timer->start(1000);

    initInfoModule();
    initBPModule();
    initPosModule();
    initOscModule();
    initReportModule();
    initDataModule();
    signalsAndSlots();

    mainLayout->setColumnStretch(0,3);
    mainLayout->setColumnStretch(1,7);

    instance.setBodyValue(&bodyValue);
    instance.setRegulator(regulator);
    instance.setdZ(admitDraw->getView());
    instance.setSudoku(sudokuDraw);

    ecgThread->start();
    diffThread->start();
    admitThread->start();
    dataThread->start();
}

EnterSystemWidget::~EnterSystemWidget()
{
    emit startDemoMode(false);
    ecgThread->quit();
    diffThread->quit();
    admitThread->quit();
    dataThread->quit();
    ecgThread->wait();
    diffThread->wait();
    admitThread->wait();
    dataThread->wait();
    timer->stop();
    delete ecgDraw;
    delete diffDraw;
    delete admitDraw;
    delete ecgThread;
    delete diffThread;
    delete admitThread;
    delete dataThread;
    delete infoDialog;
    delete bpDialog;
    delete auxArgDialog;
    delete sudokuDraw;
//    qDebug()<<"~EnterSystemWidget()";
}

void EnterSystemWidget::closeEvent(QCloseEvent *event)
{
    event->accept();
    emit widgetClose();
}

QString EnterSystemWidget::currentPortName() const
{
    return portName;
}

void EnterSystemWidget::initInfoModule()
{
    infoGroupBox = new QGroupBox(tr("患者信息"),this);
    editInfoBtn = new QPushButton(tr("编 辑"),this);
    nameLabel = new QLabel(tr("姓名"),this);
    numLabel = new QLabel(tr("病历号"),this);
    nameLineEdit = new QLineEdit(this);
    numLineEdit = new QLineEdit(this);
    infoDialog = new InfoEditDialog;

    infoLayout = new QHBoxLayout(infoGroupBox);

    firstColLayout->addWidget(infoGroupBox);
    infoLayout->addWidget(nameLabel);
    infoLayout->addWidget(nameLineEdit);
    infoLayout->addWidget(numLabel);
    infoLayout->addWidget(numLineEdit);
    infoLayout->addStretch();
    infoLayout->addWidget(editInfoBtn);
    auto &instance = DataManagement::getInstance();
//    infoGroupBox->setFixedHeight(nameLineEdit->height()*2*instance.hZoom());

    nameLineEdit->setEnabled(false);
    nameLineEdit->setFixedWidth(120*instance.wZoom());
    numLineEdit->setEnabled(false);
    numLineEdit->setFixedWidth(120*instance.wZoom());


}

void EnterSystemWidget::initBPModule()
{
    bpGroupBox = new QGroupBox(tr("血压检测"),this);
    sbpLabel = new QLabel(tr("收缩压"),this);
    dbpLabel = new QLabel(tr("舒张压"),this);
    sbpLineEdit = new QLineEdit(this);
    dbpLineEdit = new QLineEdit(this);
    mdiBtn = new QPushButton(tr("手动输入"),this);
    bpDialog = new BPEditDialog;

    QVBoxLayout *vLayout = new QVBoxLayout(bpGroupBox);
    QHBoxLayout *hLayout = new QHBoxLayout;

    firstColLayout->addWidget(bpGroupBox);
    vLayout->addLayout(hLayout);
    hLayout->addWidget(sbpLabel);
    hLayout->addWidget(sbpLineEdit);
    hLayout->addWidget(dbpLabel);
    hLayout->addWidget(dbpLineEdit);
    hLayout->addStretch();
    hLayout->addWidget(mdiBtn);
    auto &instance = DataManagement::getInstance();
//    bpGroupBox->setFixedHeight(sbpLineEdit->height()*2*instance.hZoom());

    sbpLineEdit->setEnabled(false);
    sbpLineEdit->setFixedWidth(100*instance.wZoom());
    dbpLineEdit->setEnabled(false);
    dbpLineEdit->setFixedWidth(100*instance.wZoom());
}

void EnterSystemWidget::initPosModule()
{
    pldGroupBox = new QGroupBox(tr("PLD-被动抬腿实验"),this);
    manyBtn = new QRadioButton(tr("多体位"),this);
    singleBtn = new QRadioButton(tr("单体位"),this);
    halfLieBtn = new QRadioButton(tr("半卧"),this);
    lieBtn = new QRadioButton(tr("平躺"),this);
    legLiftBtn = new QRadioButton(tr("抬腿"),this);
    recordBtn = new QPushButton(tr("记录体位"),this);

    QVBoxLayout *vLayout = new QVBoxLayout(pldGroupBox);
    QHBoxLayout *hLayout = new QHBoxLayout;
    QHBoxLayout *posLayout = new QHBoxLayout;
    QHBoxLayout *btnLayout = new QHBoxLayout;

    firstColLayout->addWidget(pldGroupBox);
    vLayout->addLayout(hLayout);
    vLayout->addLayout(posLayout);
    hLayout->addWidget(manyBtn);
    hLayout->addWidget(singleBtn);
    hLayout->addStretch();
    posLayout->addWidget(halfLieBtn);
    posLayout->addWidget(lieBtn);
    posLayout->addWidget(legLiftBtn);
    posLayout->addLayout(btnLayout);
    btnLayout->addWidget(recordBtn);
    btnLayout->addStretch();

    patternGroup.addButton(manyBtn,0);
    patternGroup.addButton(singleBtn,1);
    posGroup.addButton(halfLieBtn,0);
    posGroup.addButton(lieBtn,1);
    posGroup.addButton(legLiftBtn,2);

    manyBtn->setChecked(true);
    halfLieBtn->setChecked(true);
}

void EnterSystemWidget::initOscModule()
{
    ecgThread = new QThread;
    diffThread = new QThread;
    admitThread = new QThread;
    ecgGroupBox = new QGroupBox(tr("阻抗心电图"),this);
    diffGroupBox = new QGroupBox(tr("心血流图"),this);
    admitGroupBox = new QGroupBox(tr("心阻抗图"),this);
    ecgDraw = new DrawWaveforms(true);
    diffDraw = new DrawWaveforms;
    admitDraw = new DrawWaveforms(true);
    QVBoxLayout *ecgLayout = new QVBoxLayout(ecgGroupBox);
    QVBoxLayout *diffLayout = new QVBoxLayout(diffGroupBox);
    QVBoxLayout *admitLayout = new QVBoxLayout(admitGroupBox);

    firstColLayout->addWidget(ecgGroupBox);
    firstColLayout->addWidget(diffGroupBox);
    firstColLayout->addWidget(admitGroupBox);
    ecgLayout->addWidget(ecgDraw->getView());
    diffLayout->addWidget(diffDraw->getView());
    admitLayout->addWidget(admitDraw->getView());

    ecgDraw->moveToThread(ecgThread);
    diffDraw->moveToThread(diffThread);
    admitDraw->moveToThread(admitThread);
}

void EnterSystemWidget::initDataModule()
{
    dataThread = new QThread;
    dataGroupBox = new QGroupBox(this);
    dataGLayout = new QGridLayout(dataGroupBox);
    regulator = new CustomCtrlRegulator;
    dataGroupBox->setStyleSheet("QGroupBox::title{subcontrol-position: bottom center;}");
    secColLayout->addWidget(dataGroupBox);

    QList<CustomCtrl *> customCtrls = regulator->getSaveCustomCtrls();
    for (int num = 0; num < customCtrls.size(); ++num) {
        QHBoxLayout *hLayout = new QHBoxLayout;
        hLayouts.append(hLayout);
        hLayout->addWidget(customCtrls.at(num));
        dataGLayout->addLayout(hLayout,num/4,num%4);
        customCtrls.at(num)->show();
    }
    regulator->moveToThread(dataThread);
}

void EnterSystemWidget::initReportModule()
{
    operationGroupBox = new QGroupBox(this);
    backBtn = new QPushButton(tr("返回"),this);
    reportBtn = new QPushButton(tr("生成报告"),this);
    sudokuBtn = new QPushButton(tr("九宫格图"),this);
    auxArgBtn = new QPushButton(tr("辅助参数"),this);
    auxArgDialog = new AuxArgDialog;
    sudokuDraw = new DrawSudoku;

    QHBoxLayout *hLayout = new QHBoxLayout(operationGroupBox);

    firstColLayout->addWidget(operationGroupBox);
    hLayout->addWidget(backBtn);
    hLayout->addStretch();
    hLayout->addWidget(reportBtn);
    hLayout->addStretch();
    hLayout->addWidget(sudokuBtn);
    hLayout->addWidget(auxArgBtn);
}

void EnterSystemWidget::signalsAndSlots()
{
    auto &instance = DataManagement::getInstance();
    connect(&instance, &DataManagement::clear, this, &EnterSystemWidget::clearUiSlot);
    connect(instance.getTebco(), &ZyTebco::data, this, &EnterSystemWidget::setData);
    connect(&instance, &DataManagement::startCheck, &instance, &DataManagement::customCtrlTimer);
    connect(&instance, &DataManagement::startCheck, this, &EnterSystemWidget::startDemoMode);
    // personal info
    connect(editInfoBtn, &QPushButton::clicked, infoDialog, &InfoEditDialog::exec);
    connect(infoDialog, &InfoEditDialog::bodyValue, this, [=](BodyValue bVal){
        bodyValue = bVal;
        nameLineEdit->setText(bodyValue.name);
        numLineEdit->setText(bodyValue.id);
    });
    // bp
    connect(mdiBtn, &QPushButton::clicked, bpDialog, &BPEditDialog::exec);
    connect(bpDialog, &BPEditDialog::value, this, &EnterSystemWidget::setBPValue);
    // position
    connect(&patternGroup, &QButtonGroup::idClicked, this, &EnterSystemWidget::changeMode);
    connect(&posGroup, &QButtonGroup::idClicked, this, &EnterSystemWidget::changePosition);
    connect(recordBtn, &QPushButton::clicked, this, &EnterSystemWidget::recordPosition);
    // data
    foreach (auto customCtrl, regulator->getAllCustomCtrls()) {
        connect(customCtrl, &CustomCtrl::changeName, this, &EnterSystemWidget::changeShow);
        connect(this, &EnterSystemWidget::recordValue, customCtrl, &CustomCtrl::recordValueSlot);
        if (customCtrl->getName() == "SI") {
            connect(customCtrl, &CustomCtrl::currentValue, sudokuDraw, [=](qreal si){
                sudokuDraw->setSi(posGroup.checkedId(),si,!rPos.isEmpty());
            });
        }
        if (customCtrl->getName() == "MAP") {
            connect(customCtrl, &CustomCtrl::currentValue, sudokuDraw, [=](qreal map){
                sudokuDraw->setMap(posGroup.checkedId(),map,!rPos.isEmpty());
            });
        }
    }
    // osc
    connect(instance.getTebco(), &ZyTebco::ecgValue, ecgDraw, &DrawWaveforms::addValue);
    connect(instance.getTebco(), &ZyTebco::diffValue, diffDraw, &DrawWaveforms::addValue);
    connect(instance.getTebco(), &ZyTebco::admitValue, admitDraw, &DrawWaveforms::addValue);
    // report
    connect(backBtn, &QPushButton::clicked, this, &EnterSystemWidget::close);
    connect(reportBtn, &QPushButton::clicked, this, &EnterSystemWidget::createReport);
    connect(auxArgBtn, &QPushButton::clicked, auxArgDialog, &AuxArgDialog::exec);
    connect(sudokuBtn, &QPushButton::clicked, sudokuDraw, &DrawSudoku::exec);
    connect(auxArgDialog, &AuxArgDialog::value, this, [=](int cvp, int lap){
        bodyValue.CVP = cvp;
        bodyValue.LAP = lap;
        DataManagement::getInstance().getArgs().CVP = cvp;
        DataManagement::getInstance().getArgs().LAP = lap;
    });
}

void EnterSystemWidget::timeoutSlot()
{
    dataGroupBox->setTitle(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
}

void EnterSystemWidget::changeShow(const QString &current, const QString &change)
{
    CustomCtrl *cuCtrl = regulator->getCustomCtrl(current);
    CustomCtrl *chCtrl = regulator->getCustomCtrl(change);
    foreach (QHBoxLayout *layout, hLayouts) {
        if(-1 != layout->indexOf(cuCtrl)) {
            cuCtrl->hide();
            layout->removeWidget(cuCtrl);
            layout->addWidget(chCtrl);
            chCtrl->show();
            regulator->changeCurrentNames(current,change);
            break;
        }
    }
}

void EnterSystemWidget::setData(const uchar &type, const double &value)
{
    switch (type) {
    case Type::HR:
        setCtrlValue(Type::HR,value);
        break;
    case Type::VET:
    {
        setCtrlValue(Type::LVET,value);
//        auto PEPCtrl = regulator->getCustomCtrl(typeName(Type::PEP));
//        if(PEPCtrl != nullptr) {
//            double pep = PEPCtrl->getCurrentValue();
//            if(pep != 0) {
//                setCtrlValue(Type::STR,PEPCtrl->getCurrentValue()/value,1);
//            }
//        }
    }
        break;
    case Type::PEP:
    {
        setCtrlValue(Type::PEP,value);
        auto LVETCtrl = regulator->getCustomCtrl(typeName(Type::LVET));
        if(LVETCtrl != nullptr) {
            double lvet = LVETCtrl->getCurrentValue();
            if(lvet != 0) {
                setCtrlValue(Type::STR,value/LVETCtrl->getCurrentValue(),1);
            }
        }
    }
        break;
    case Type::TFC:
        setCtrlValue(Type::TFC,value/1000.0,3);
        break;
    case Type::EPCI:
        setCtrlValue(Type::EPCI,value/1000.0,3);
        break;
    case Type::ISI:
        setCtrlValue(Type::ISI,value/100.0,2);
        if (bodyValue.age != 0) {
            double idealISI = 0;
            if (bodyValue.sex == 1) {
                if (bodyValue.age < 55) {
                    idealISI = 1.35;
                }
                else {
                    idealISI = 1.20;
                }
            }
            else {
                if (bodyValue.age < 55) {
                    idealISI = 1.15;
                }
                else {
                    idealISI = 1.10;
                }
            }
            if (value/100.0 > idealISI) {
                setCtrlValue(Type::Ino,fabs((value/100.0)/idealISI-1)*100);
            }
            else {
                setCtrlValue(Type::Ino,-fabs(idealISI/(value/100.0)-1)*100);
            }
        }
        break;
    case Type::EF:
//    {
//        setCtrlValue(Type::EF,value);
        efValue = value;
//        auto SICtrl = regulator->getCustomCtrl(typeName(Type::SI));
//        if(SICtrl != nullptr && !SICtrl->getValueStr().isEmpty()) {
//            setCtrlValue(Type::EDI,(SICtrl->getCurrentValue()/value)*100);
//        }
//    }
        break;
    case Type::SI:
    {
        double SIValue = 2/bodyValue.BSA()*bodyValue.VEPT()/6800*value;
        double SVValue = SIValue*bodyValue.BSA();
        setCtrlValue(Type::SI,SIValue);
        setCtrlValue(Type::SV,SVValue,1);
//        if(svvQueue.size() == 10) {
//            double avgSV = accumulate(svvQueue.begin(),svvQueue.end(),0)/svvQueue.size();
//            setCtrlValue(Type::SVV,fabs(((SVValue-avgSV)/avgSV)),1);
//            svvQueue.dequeue();
//        }
//        svvQueue.enqueue(SVValue);
        setCtrlValue(Type::EDI,(SIValue/efValue)*100);
        if(bodyValue.SBP != 0 && bodyValue.DBP != 0) {
            setCtrlValue(Type::LSW,0.0144*SVValue*(bodyValue.MAP()-bodyValue.LAP));
            double lswi = 0.0144*SIValue*(bodyValue.MAP()-bodyValue.LAP);
            setCtrlValue(Type::LSWI,lswi,1);
            auto InoCtrl = regulator->getCustomCtrl(typeName(Type::Ino));
            if(InoCtrl != nullptr && bodyValue.age != 0) {
                if (!InoCtrl->getValueStr().isEmpty()) {
                    double con = 0;
                    if(lswi > 52.8) {
                        con = fabs(lswi/52.8-1);
                    }
                    else {
                        con = -fabs(52.8/lswi-1);
                    }
                    setCtrlValue(Type::Vol,con*100-InoCtrl->getCurrentValue());
                }
            }
            setCtrlValue(Type::SSVR,80*(bodyValue.MAP()-bodyValue.CVP)/SVValue,1);
            double ssvri = 80*(bodyValue.MAP()-bodyValue.CVP)/SIValue;
            setCtrlValue(Type::SSVRI,ssvri,1);
            if(ssvri > 137.8) {
                setCtrlValue(Type::Vas,fabs(ssvri/137.8-1)*100);
            }
            else {
                setCtrlValue(Type::Vas,-fabs(137.8/ssvri-1)*100);
            }
        }
    }
        break;
    case Type::CI:
    {
        double CIValue = 2/bodyValue.BSA()*bodyValue.VEPT()/6800*value/10;
        setCtrlValue(Type::CI,CIValue,1);
        double COValue = CIValue*bodyValue.BSA();
        setCtrlValue(Type::CO,COValue,1);
        if(CIValue > 4.5) {
            setCtrlValue(Type::HRV,fabs(CIValue/4.5-1)*100);
        }
        else {
            setCtrlValue(Type::HRV,-fabs(4.5/CIValue-1)*100);
        }
        if(bodyValue.SBP != 0 && bodyValue.DBP != 0) {
            setCtrlValue(Type::SVR,(bodyValue.MAP()-bodyValue.CVP)/COValue*80);
            setCtrlValue(Type::SVRI,80*(bodyValue.MAP()-bodyValue.CVP)/CIValue);
            setCtrlValue(Type::LCW,0.0144*(bodyValue.MAP()-bodyValue.LAP-2)*COValue,1);
            setCtrlValue(Type::LCWI,0.0144*(bodyValue.MAP()-bodyValue.LAP)*CIValue,2);
        }
    }
        break;
    case Type::RR:
//        setCtrlValue(Type::RR,value/10);
        break;
    case Type::BEEP:
//        QApplication::beep();
        break;
    }
}

void EnterSystemWidget::setBPValue(const QString &sbp, const QString &dbp)
{
    sbpLineEdit->setText(sbp);
    dbpLineEdit->setText(dbp);
    auto BPCtrl = regulator->getCustomCtrl("SBP/DBP");
    auto MAPCtrl = regulator->getCustomCtrl("MAP");
    if(BPCtrl != nullptr && MAPCtrl != nullptr) {
        if(!sbp.isEmpty() && !dbp.isEmpty()) {
            bodyValue.SBP = sbp.toInt();
            bodyValue.DBP = dbp.toInt();
            BPCtrl->setValues(bodyValue.SBP,bodyValue.DBP);
            MAPCtrl->setValue(bodyValue.MAP());
        }
        else {
            BPCtrl->clear();
            MAPCtrl->clear();
        }
    }
}

void EnterSystemWidget::changeMode(const int &id)
{
    if(id == 0) {
        recordBtn->setVisible(true);
    }
    else if(id == 1) {
        if (!rPos.isEmpty() && QMessageBox::question(this,tr("提示"),
                         tr("已记录体位，确定切换为单体位吗？")) == QMessageBox::No){
            manyBtn->setChecked(true);
            return;
        }
        rPos.clear();
        recordBtn->setVisible(false);
    }
}

void EnterSystemWidget::recordPosition()
{
    if (isStartCheck()) {
        emit recordValue();
        rPos = posGroup.checkedButton()->text();
        DataManagement::getInstance().recordPosition(rPos);
    }
}

void EnterSystemWidget::changePosition(int id)
{
    if (!rPos.isEmpty() && posGroup.button(id)->text() != rPos) {
        recordBtn->hide();
    }
    else if(posGroup.button(id)->text() == rPos) {
        recordBtn->show();
    }
}

void EnterSystemWidget::createReport()
{
    if (isStartCheck()) {
        auto &instance = DataManagement::getInstance();
        if (IdCheck::getCurrentConsumables() <= 0) {
            QMessageBox::warning(this,tr("警告！"),tr("有效验证码已使用完，请联系厂家！"));
            return;
        }
        if (patternGroup.checkedId() == 0 && !instance.isRecordPos()) {
            QMessageBox::information(this,tr("提示"),tr("多体位模式需要记录体位！"));
            return;
        }
        WaitingDialog waiting = WaitingDialog(tr("报告生成中···"), this);
        connect(&instance,&DataManagement::clear,&waiting,&WaitingDialog::close);
        instance.saveReport(posGroup.checkedButton()->text(),!rPos.isEmpty());
        waiting.exec();
        emit createdReport();
        instance.reportPreview(instance.getNewReportName());
    }
}

void EnterSystemWidget::clearUiSlot()
{
    rPos.clear();
    // 清空界面个人信息
    nameLineEdit->clear();
    numLineEdit->clear();
    sbpLineEdit->clear();
    dbpLineEdit->clear();
    infoDialog->clear();
    // 清空血压对话窗
    bpDialog->clear();
    // 清空波形图
    ecgDraw->clear();
    diffDraw->clear();
    admitDraw->clear();
    // 清空九宫格
    sudokuDraw->clear();
    // 重置人体信息
    bodyValue = BodyValue();
    // 清空检测的数据
    foreach (auto customCtrl, regulator->getAllCustomCtrls()) {
        customCtrl->clear();
    }
}

void EnterSystemWidget::setCtrlValue(const Type &type, const double &value, const int &digit)
{
    auto customCtrl = regulator->getCustomCtrl(typeName(type));
    if(customCtrl != nullptr) {
        customCtrl->setValue(value,digit);
    }
}

bool EnterSystemWidget::isStartCheck()
{
    if (!DataManagement::getInstance().getTebco()->isWorking()) {
        QMessageBox::information(this,tr("提示"),tr("未启动检测！"));
        return false;
    }
    return true;
}
