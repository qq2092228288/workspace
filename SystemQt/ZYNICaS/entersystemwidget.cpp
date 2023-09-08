#include "entersystemwidget.h"
#include "datamanagement.h"
#include <threadservice.h>
#include "datacalculation.h"
#include "isicurvewidget.h"
#include <QPrintPreviewDialog>
#include "reportpainter.h"


#include <iostream>
#include <numeric>
#include <math.h>
using namespace std;

EnterSystemWidget::EnterSystemWidget(QWidget *parent)
    : QWidget{parent}
{
    DataManagement &instance = DataManagement::getInstance();
    setMinimumSize(1600*instance.wZoom(), 900*instance.hZoom());
    setWindowTitle(tr("芫泽无创血流动力学检测"));
    //样式表
    int fsize = 20*instance.zoom()+1;         //字体大小
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
    connect(timer, &QTimer::timeout, this, &EnterSystemWidget::timeoutSlot);
    timer->start(1000);

    initInfoModule();
    initBPModule();
    initPosModule();
    initOscModule();
    initDataModule();
    initReportModule();
    signalsAndSlots();

    mainLayout->setColumnStretch(0,3);
    mainLayout->setColumnStretch(1,7);

    instance.setBodyValue(&bodyValue);
}

EnterSystemWidget::~EnterSystemWidget()
{
    emit startDemoMode(false);
    timer->stop();
    delete regulator;
    delete ecgDraw;
    delete diffDraw;
    delete admitDraw;
    delete infoDialog;
    delete bpDialog;
    delete trendChartsWidget;
    delete auxArgDialog;
//    delete sudokuDraw;
    delete sudokuWidget;
    //    qDebug()<<"~EnterSystemWidget()";
}

void EnterSystemWidget::trendChartLayout()
{
    trendChartsWidget->show();
    trendChartsWidget->hide();
}

void EnterSystemWidget::showEvent(QShowEvent *event)
{
    auto &instance = DataManagement::getInstance();
    if (instance.getHospitalInfo()->cMode == Check_Mode::PhysicalExamination) {
        instance.getRegulator()->connectTrendChart(false);
        trendChartBtn->hide();
    }
    else {
        instance.getRegulator()->connectTrendChart(true);
        trendChartBtn->show();
    }
    infoDialog->showHb(instance.getHospitalInfo()->cMode == Check_Mode::IntensiveCareUnit);
    event->accept();
}

void EnterSystemWidget::closeEvent(QCloseEvent *event)
{
    event->accept();
    emit widgetClose();
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
    pldGroupBox = new QGroupBox(tr("PLR-被动抬腿试验"), this);
    manyBtn = new QRadioButton(tr("多体位"), this);
    singleBtn = new QRadioButton(tr("单体位"), this);
    halfLieBtn = new QRadioButton(tr("半卧"), this);
    lieBtn = new QRadioButton(tr("平躺"), this);
    legLiftBtn = new QRadioButton(tr("抬腿"), this);
    recordBtn = new QPushButton(tr("记录体位"), this);
    startupTestBtn = new QPushButton(tr("启动\n试验"), this);
    plrtWidget = new PlrtTableWidget(this);

    QGridLayout *gLayout = new QGridLayout(pldGroupBox);
    firstColLayout->addWidget(pldGroupBox);
    gLayout->addWidget(manyBtn, 0, 0);
    gLayout->addWidget(singleBtn, 0, 1);
    gLayout->addWidget(halfLieBtn, 1, 0);
    gLayout->addWidget(lieBtn, 1, 1);
    gLayout->addWidget(legLiftBtn, 1, 2);
    gLayout->addWidget(recordBtn, 1, 3);
    gLayout->addWidget(startupTestBtn, 0, 5, 2, 1, Qt::AlignRight);
    gLayout->setColumnStretch(0, 1);
    gLayout->setColumnStretch(1, 1);
    gLayout->setColumnStretch(2, 1);
    gLayout->setColumnStretch(3, 1);
    gLayout->setColumnStretch(4, 2);
    gLayout->setColumnStretch(5, 1);
    startupTestBtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    patternGroup.addButton(manyBtn, 0);
    patternGroup.addButton(singleBtn, 1);
    posGroup.addButton(halfLieBtn, 0);
    posGroup.addButton(lieBtn, 1);
    posGroup.addButton(legLiftBtn, 2);


    manyBtn->setChecked(true);
    halfLieBtn->setChecked(true);
}

void EnterSystemWidget::initOscModule()
{
    ecgGroupBox = new QGroupBox(tr("阻抗心电图"), this);
    diffGroupBox = new QGroupBox(tr("心血流图"), this);
    admitGroupBox = new QGroupBox(tr("心阻抗图"), this);
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

    ThreadService::getInstance()->objectMoveToThread(ecgDraw);
    ThreadService::getInstance()->objectMoveToThread(diffDraw);
    ThreadService::getInstance()->objectMoveToThread(admitDraw);
}

void EnterSystemWidget::initDataModule()
{
    auto args = DataManagement::getInstance().getArgs();
    SuBArg suBArg;
    suBArg.cvp = args.CVP;
    suBArg.lap = args.LAP;
    suBArg.map = ArgScope(args.find(Type::MAP).min, args.find(Type::MAP).max);
    suBArg.si = ArgScope(args.find(Type::SI).min, args.find(Type::SI).max);
    suBArg.ssvri = ArgScope(args.find(Type::SSVRI).min, args.find(Type::SSVRI).max);
    suBArg.lswi = ArgScope(args.find(Type::LSWI).min, args.find(Type::LSWI).max);

    dataGroupBox = new QGroupBox(this);
    dataGLayout = new QGridLayout(dataGroupBox);
    regulator = new CustomCtrlRegulator;
    dataGroupBox->setStyleSheet("QGroupBox::title{subcontrol-position: bottom center;}");
    secColLayout->addWidget(dataGroupBox);
    sudokuWidget = new SudokuWidget(suBArg);

    QList<CustomCtrl *> customCtrls = regulator->getSaveCustomCtrls(false);
    for (int num = 0; num < 11; ++num) {
        dataGLayout->addWidget(customCtrls.at(num), num/4, num%4);
        customCtrls.at(num)->show();
    }
    dataGLayout->addWidget(sudokuWidget, 2, 3);
    ThreadService::getInstance()->objectMoveToThread(regulator);
    DataManagement::getInstance().setRegulator(regulator);
}

void EnterSystemWidget::initReportModule()
{
    operationGroupBox = new QGroupBox(this);
    backBtn = new QPushButton(tr("返回"), this);
    reportBtn = new QPushButton(tr("生成报告"),this);
//    plrtBtn = new QPushButton(tr("PLRT"), this);
    trendChartBtn = new QPushButton(tr("趋势图"),this);
//    sudokuBtn = new QPushButton(tr("血压靶向分析图"),this);
    auxArgBtn = new QPushButton(tr("辅助参数"),this);
    trendChartsWidget = new TrendChartsWidget;
    auxArgDialog = new AuxArgDialog;
//    sudokuDraw = new DrawSudoku;
    reportJson = new ReportDataJson(this);

    QHBoxLayout *hLayout = new QHBoxLayout(operationGroupBox);

    firstColLayout->addWidget(operationGroupBox);
    hLayout->addWidget(backBtn);
    hLayout->addWidget(reportBtn);
    hLayout->addStretch();
//    hLayout->addWidget(plrtBtn);
    hLayout->addWidget(trendChartBtn);
//    hLayout->addWidget(sudokuBtn);
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
        // 设置报告数据
        reportJson->setPatientInfo(bVal.age, bVal.hb, bVal.height, bVal.weight, bVal.sex, bVal.name, bVal.id);
    });
    connect(infoDialog, &InfoEditDialog::updateUi, this, [=](){
        rPos.clear();
//        recordDataMap.clear();
//        baseData = BaseData();
        svValues.clear();
        // 清空波形图
        ecgDraw->clear();
        diffDraw->clear();
        admitDraw->clear();
        // 清空九宫格
//        sudokuDraw->clear();
        sudokuWidget->clear();
        // 清空plrt
        plrtWidget->clear();
        // 清空检测的数据
        foreach (auto customCtrl, regulator->getAllCustomCtrls()) {
            customCtrl->clear();
        }
    });
    // bp
    connect(mdiBtn, &QPushButton::clicked, this, &EnterSystemWidget::showBpDialogSlot);
    connect(bpDialog, &BPEditDialog::value, this, &EnterSystemWidget::setBPValue);
    // position
    connect(&patternGroup, &QButtonGroup::idClicked, this, &EnterSystemWidget::changeMode);
    connect(&posGroup, &QButtonGroup::idClicked, this, &EnterSystemWidget::changePosition);
    connect(recordBtn, &QPushButton::clicked, this, &EnterSystemWidget::recordPosition);
    connect(startupTestBtn, &QPushButton::clicked, this, &EnterSystemWidget::startupTestSlot);
    connect(plrtWidget, &PlrtTableWidget::editBp, this, &EnterSystemWidget::showBpDialogSlot);
    connect(plrtWidget, &PlrtTableWidget::status, this, [=](auto status){
        switch (status) {
        case TestS::Started:
            legLiftBtn->setChecked(true);
            emit posGroup.idClicked(posGroup.id(legLiftBtn));
            break;
        case TestS::Paused:
            // nothing
            break;
        case TestS::Stopped:
            foreach (auto btn, posGroup.buttons()) {
                if (btn->text() == rPos) {
                    btn->setChecked(true);
                    emit posGroup.idClicked(posGroup.id(btn));
                    break;
                }
            }
            break;
        case TestS::Completed:
            createReport();
            break;
        default:
            break;
        }
    });
    // data
    foreach (auto customCtrl, regulator->getAllCustomCtrls()) {
        connect(customCtrl, &CustomCtrl::changeName, this, &EnterSystemWidget::changeShow);
        connect(this, &EnterSystemWidget::recordValue, customCtrl, &CustomCtrl::recordValueSlot);
        if (customCtrl->getName() == "SI") {
            connect(customCtrl, &CustomCtrl::currentValue, this, [=](qreal si){
                sudokuWidget->setPoint(bodyValue.MAP(), si, SignType(posGroup.checkedId()), rPos.isEmpty());
            });
        }
        connect(customCtrl, &CustomCtrl::nameAndValue, plrtWidget, &PlrtTableWidget::setData);
    }
    // osc
    connect(instance.getTebco(), &ZyTebco::ecgValue, ecgDraw, &DrawWaveforms::addValue);
    connect(instance.getTebco(), &ZyTebco::diffValue, diffDraw, &DrawWaveforms::addValue);
    connect(instance.getTebco(), &ZyTebco::admitValue, admitDraw, &DrawWaveforms::addValue);
    // report
    connect(backBtn, &QPushButton::clicked, this, &EnterSystemWidget::close);
    connect(reportBtn, &QPushButton::clicked, this, &EnterSystemWidget::createReport);
    connect(trendChartBtn, &QPushButton::clicked, trendChartsWidget, &TrendChartsWidget::widgetShow);
    connect(auxArgBtn, &QPushButton::clicked, auxArgDialog, &AuxArgDialog::exec);
    connect(auxArgDialog, &AuxArgDialog::value, this, [=](int cvp, int lap){
        bodyValue.CVP = cvp;
        bodyValue.LAP = lap;
        DataManagement::getInstance().getArgs().CVP = cvp;
        DataManagement::getInstance().getArgs().LAP = lap;
    });
    // report data
    connect(instance.getTebco(), &ZyTebco::waveform, reportJson, &ReportDataJson::appendWaveformData);
    connect(instance.getTebco(), &ZyTebco::data, reportJson, &ReportDataJson::appendData);
}

void EnterSystemWidget::timeoutSlot()
{
    dataGroupBox->setTitle(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
}

void EnterSystemWidget::changeShow(const QString &current, const QString &change)
{
    CustomCtrl *cuCtrl = regulator->getCustomCtrl(current);
    CustomCtrl *chCtrl = regulator->getCustomCtrl(change);
    for (int index = 0; index < dataGLayout->count(); ++index) {
        auto widget = dataGLayout->itemAtPosition(index/4, index%4)->widget();
        if (widget == cuCtrl && widget != nullptr) {
            cuCtrl->hide();
            dataGLayout->removeWidget(cuCtrl);
            dataGLayout->addWidget(chCtrl, index/4, index%4);
            chCtrl->show();
            regulator->changeCurrentNames(current, change, false);
            break;
        }
    }
}

void EnterSystemWidget::setData(const uchar &type, const short &value)
{
    switch (type) {
    case Type::HR:
        setCtrlValue(Type::HR, DatCa::cHr(value));
        break;
    case Type::VET:
        setCtrlValue(Type::LVET, DatCa::cVet(value));
        break;
    case Type::PEP:
        setCtrlValue(Type::PEP, DatCa::cPep(value));
        setCtrlValue(Type::STR, DatCa::cStr(value, regulator->getCustomCtrl(typeName(Type::LVET))->getCurrentValue()));
        break;
    case Type::TFC:
        setCtrlValue(Type::TFC, DatCa::cTfc(value));
        break;
    case Type::EPCI:
        setCtrlValue(Type::EPCI, DatCa::cEpci(value));
        break;
    case Type::ISI:
        setCtrlValue(Type::ISI, DatCa::cIsi(value));
        setCtrlValue(Type::Ino, DatCa::cIno(DatCa::cIsi(value), bodyValue.sex, bodyValue.age));
        break;
    case Type::EF:
        efValue = DatCa::cEf(value);
        break;
    case Type::SI:
    {
        double si = DatCa::cSi(value, bodyValue.BSA(), bodyValue.VEPT());
        double sv = DatCa::cSv(si, bodyValue.BSA());
        setCtrlValue(Type::SI, si);
        setCtrlValue(Type::SV, sv);
        setCtrlValue(Type::EDI, DatCa::cEdi(si, efValue));
        if(bodyValue.SBP != 0 && bodyValue.DBP != 0) {
            setCtrlValue(Type::LSW, DatCa::cLsw(sv, bodyValue.MAP(), bodyValue.LAP));
            double lswi = DatCa::cLswi(si, bodyValue.MAP(), bodyValue.LAP);
            setCtrlValue(Type::LSWI, lswi);
            setCtrlValue(Type::Vol, DatCa::cVol(lswi, regulator->getCustomCtrl(typeName(Type::Ino))->getCurrentValue()));
            setCtrlValue(Type::SSVR, DatCa::cSsvr(sv, bodyValue.MAP(), bodyValue.CVP));
            double ssvri = DatCa::cSsvri(si, bodyValue.MAP(), bodyValue.CVP);
            setCtrlValue(Type::SSVRI, ssvri);
            setCtrlValue(Type::Vas, DatCa::cVas(ssvri));
        }
        setCtrlValue(Type::SVV, DatCa::cSvv(sv, svValues));
        svValues.append(sv);
        if (svValues.size() > 25) {
            svValues.removeFirst();
        }
    }
        break;
    case Type::CI:
    {
        double ci = DatCa::cCi(value, bodyValue.BSA(), bodyValue.VEPT());
        setCtrlValue(Type::CI, ci);
        double co = DatCa::cCo(ci, bodyValue.BSA());
        setCtrlValue(Type::CO, co);
        setCtrlValue(Type::HRV, DatCa::cHrv(ci));
        if(bodyValue.SBP != 0 && bodyValue.DBP != 0) {
            setCtrlValue(Type::SVR, DatCa::cSvr(co, bodyValue.MAP(), bodyValue.CVP));
            setCtrlValue(Type::SVRI, DatCa::cSvri(ci, bodyValue.MAP(), bodyValue.CVP));
            setCtrlValue(Type::LCW, DatCa::cLcw(co, bodyValue.MAP(), bodyValue.LAP));
            setCtrlValue(Type::LCWI, DatCa::cLcwi(ci, bodyValue.MAP(), bodyValue.LAP));
        }
        if (bodyValue.hb != 0) {
            setCtrlValue(Type::DO2, DatCa::cDo2(co, bodyValue.hb));
        }
    }
        break;
    case Type::RR:
//        setCtrlValue(Type::RR,DatCa::cRr(value));
        break;
    case Type::BEEP:
//        QApplication::beep();
        break;
    }
//    recordDataMap.insert(type, value);
}

void EnterSystemWidget::showBpDialogSlot()
{
    if (isStartCheck()) {
        bpDialog->exec();
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
            BPCtrl->setValues(bodyValue.SBP, bodyValue.DBP);
            MAPCtrl->setValue(bodyValue.MAP(), MAPCtrl->getName());
            auto SICtrl = regulator->getCustomCtrl(typeName(Type::SI));
            if (SICtrl != nullptr) {
                sudokuWidget->setPoint(bodyValue.MAP(), SICtrl->getCurrentValue(),
                                       SignType(posGroup.checkedId()), rPos.isEmpty());
            }
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
                         tr("已记录体位，确定切换为单体位吗？")) == QMessageBox::No) {
            manyBtn->setChecked(true);
            return;
        }
//        baseData = BaseData();
        reportJson->clear();
        rPos.clear();
        recordBtn->setVisible(false);
    }
}

void EnterSystemWidget::recordPosition()
{
    if (isStartCheck() && detectedData()) {
        emit recordValue();
        rPos = posGroup.checkedButton()->text();
        auto &instance = DataManagement::getInstance();
        reportJson->appendPosition(bodyValue.SBP, bodyValue.DBP, bodyValue.CVP, bodyValue.LAP, posGroup.checkedId() + 1);
        instance.getTebco()->clearMap();
        QMessageBox::information(this, tr("提示"), tr("已记录体位。"));
    }
}

void EnterSystemWidget::changePosition(int id)
{
    if (!rPos.isEmpty() && posGroup.button(id)->text() != rPos) {
        recordBtn->hide();
        foreach (auto btn, posGroup.buttons()) {
            if (btn->text() == rPos) {
                plrtWidget->setPic(PosType(posGroup.id(btn) + 1), PosType(id + 1));
            }
        }
    }
    else if(posGroup.button(id)->text() == rPos) {
        recordBtn->show();
    }
}

void EnterSystemWidget::createReport()
{
    if (isStartCheck() && detectedData()) {
        auto &instance = DataManagement::getInstance();
        if (patternGroup.checkedId() == 0 && rPos.isEmpty()) {
            QMessageBox::information(this, tr("提示"), tr("多体位模式需要记录体位！"));
            return;
        }
        if (!rPos.isEmpty() && !recordBtn->isHidden()) {
            QMessageBox::information(this, tr("提示"), tr("未改变体位！"));
            return;
        }
        // not physical examination model
        if (instance.getHospitalInfo()->cMode != Check_Mode::PhysicalExamination) {
            trendChartsWidget->saveTrendChartPic();
        }
        auto isiCtrl = regulator->getCustomCtrl(typeName(Type::ISI));
        auto svCtrl = regulator->getCustomCtrl(typeName(Type::SV));
        if (isiCtrl->getRecordValue() != 0 && svCtrl->getRecordValue() != 0) {
            IsiCurveWidget widget(this);
            widget.setIsi(isiCtrl->getRecordValue(), isiCtrl->getCurrentValue());
            widget.setSv(svCtrl->getRecordValue(), svCtrl->getCurrentValue());
            widget.grab().save(instance.getPaths().isiCurve());
        }
        reportJson->appendPosition(bodyValue.SBP, bodyValue.DBP, bodyValue.CVP, bodyValue.LAP, posGroup.checkedId() + 1);
        QDateTime curTime = QDateTime::fromString(reportJson->getReportTime(), "yyyyMMddhhmmss");
        reportJson->setReportConclusion(instance.reportCreated(!rPos.isEmpty()));
        emit createdReport(curTime.toMSecsSinceEpoch(), reportJson->getJsonString());

        QPrinter printer(QPrinter::ScreenResolution);
        printer.setPageSize(QPageSize(QSizeF(210, 297), QPageSize::Millimeter));
        QPrintPreviewDialog dialog(&printer, this);
        dialog.setWindowTitle("报告预览");
        dialog.setWindowFlags(Qt::Dialog | Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);

        connect(&dialog, &QPrintPreviewDialog::paintRequested, this, [=](QPrinter *printer) {
            auto info = DataManagement::getInstance().getHospitalInfo();
            ReportPainter painter(ReportStruct(info->pType,
                                               info->cMode,
                                               !info->samePage,
                                               reportJson->getJson()),
                                  printer);
        });
        dialog.exec();


        reportJson->clear();
        clearUiSlot();
        if (manyBtn->isChecked()) {
            recordBtn->show();
            halfLieBtn->setChecked(true);
        }
    }
}

void EnterSystemWidget::clearUiSlot()
{
    rPos.clear();
//    recordDataMap.clear();
//    baseData = BaseData();
    svValues.clear();
    // 清空界面个人信息
    numLineEdit->clear();
    nameLineEdit->clear();
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
//    sudokuDraw->clear();
    sudokuWidget->clear();
    // 重置人体信息
    bodyValue = BodyValue();
    // 清空plrt
    plrtWidget->clear();
    // 清空检测的数据
    foreach (auto customCtrl, regulator->getAllCustomCtrls()) {
        customCtrl->clear();
    }
}

void EnterSystemWidget::systemModeChanged(Check_Mode mode)
{
    switch (mode) {
    case Check_Mode::PhysicalExamination:
        singleBtn->show();
        startupTestBtn->hide();
        legLiftBtn->setEnabled(true);
        legLiftBtn->setToolTip("");
        break;
    default:
        manyBtn->setChecked(true);
        singleBtn->hide();
        recordBtn->show();
        startupTestBtn->show();
        legLiftBtn->setEnabled(false);
        legLiftBtn->setToolTip(tr("如需以“抬腿”体位作为第一体位，请在“系统配置”中选择“体检模式”。"));
        break;
    }
    if (mode == Check_Mode::InternalMedicine) {
        DataManagement::getInstance().getRegulator()->connectTrendChart(true);
        trendChartBtn->show();
    }
    else {
        DataManagement::getInstance().getRegulator()->connectTrendChart(false);
        trendChartBtn->hide();
    }
    clearUiSlot();
}

void EnterSystemWidget::startupTestSlot()
{
    if (!isStartCheck()) {
        return;
    }
    else if (patternGroup.checkedId() == 1) {
        QMessageBox::warning(this, tr("警告"), tr("被动抬腿试验不适用于单体位！"));
        return;
    }
    else if (rPos.isEmpty()) {
        QMessageBox::warning(this, tr("警告"), tr("请先记录体位！"));
        return;
    }
    else if (rPos == legLiftBtn->text()) {
        QMessageBox::warning(this, tr("警告"), tr("PLRT抬腿试验第一体位为“半卧”或“平躺”！"));
        return;
    }
    plrtWidget->setCountDown(120);    // 设置试验时间
    plrtWidget->exec();
}

bool EnterSystemWidget::detectedData()
{
    if (0 == regulator->getCustomCtrl(typeName(Type::MAP))->getCurrentValue()) {
        QMessageBox::information(this, tr("提示"), tr("请先输入血压。"));
        return false;
    }
    else if (0 == regulator->getCustomCtrl(typeName(Type::SSVRI))->getCurrentValue()) {
        QMessageBox::information(this, tr("提示"), tr("数据检测中，请稍后。"));
        return false;
    }
    return true;
}

void EnterSystemWidget::setCtrlValue(const Type &type, const double &value)
{
    if (value == DatCa::invalid()) return;
    regulator->getCustomCtrl(typeName(type))->setValue(value);
}

bool EnterSystemWidget::isStartCheck()
{
    if (!DataManagement::getInstance().getTebco()->isWorking()) {
        QMessageBox::information(this,tr("提示"),tr("未启动检测！"));
        return false;
    }
    return true;
}

//void EnterSystemWidget::setBaseData()
//{
//    // 原始数据
//    if (patternGroup.checkedId() == 1 || !recordBtn->isHidden()) {
//        setTebcoData(baseData.firstPosture);
//    }
//    else {
//        setTebcoData(baseData.secondPosture);
//    }
//    // 个人信息
//    baseData.patient.patientName = bodyValue.name;
//    baseData.patient.medicalRecordNumber = bodyValue.id;
//    baseData.patient.sex = (bodyValue.sex == 0 ? QString("男") : QString("女"));
//    baseData.patient.age = QString::number(bodyValue.age);
//    baseData.patient.height = QString::number(bodyValue.height);
//    baseData.patient.weight = QString::number(bodyValue.weight);
//    baseData.patient.hb = QString::number(bodyValue.hb);
//    // 场所信息
//    auto hospitalInfo = DataManagement::getInstance().getHospitalInfo();
////    baseData.place.placeId = hospitalInfo->place2Name;
////    baseData.place.place1Id = hospitalInfo->place1Id;
////    baseData.place.place2Id = hospitalInfo->place2Id;
////    baseData.place.deviceId = hospitalInfo->deviceId;
//    baseData.place.primaryPlace = hospitalInfo->hospitalName;
//    baseData.place.secondaryPlace = hospitalInfo->roomName;
//    baseData.place.inspector = hospitalInfo->doctorName.isEmpty() ? "unknown" : hospitalInfo->doctorName;
//    baseData.place.mac = DataManagement::getInstance().getMac();
////    baseData.sudokuPix = sudokuDraw->grab();
////    baseData.sudokuPix = sudokuWidget->grab();
//}

//void EnterSystemWidget::setTebcoData(TebcoData &tebcoData)
//{
//    recordDataMap.insert(Type::SBP, bodyValue.SBP);
//    recordDataMap.insert(Type::DBP, bodyValue.DBP);
//    recordDataMap.insert(Type::CVP, bodyValue.CVP);
//    recordDataMap.insert(Type::LAP, bodyValue.LAP);
//    // 体位
//    recordDataMap.insert(Type::Pos, posGroup.checkedId() + 1);
//    // 原始的数据
//    tebcoData.data = recordDataMap;
//    // 保存时间
//    tebcoData.cTime = QDateTime::currentDateTime();
//    // 心阻抗图
////    tebcoData.dzPix = admitDraw->getView()->grab();
//}
