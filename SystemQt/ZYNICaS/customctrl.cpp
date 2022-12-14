#include "customctrl.h"
#include <math.h>
#include <QRandomGenerator>
#include "datamanagement.h"

QString typeName(const uchar &type)
{
    switch (type) {
    case Type::HR:
        return "HR";
        break;
    case Type::VET:
        return "VET";
        break;
    case Type::PEP:
        return "PEP";
        break;
    case Type::TFC:
        return "TFC";
        break;
    case Type::EPCI:
        return "EPCI";
        break;
    case Type::ISI:
        return "ISI";
        break;
    case Type::EF:
        return "EF";
        break;
    case Type::SI:
        return "SI";
        break;
    case Type::CI:
        return "CI";
        break;
    case Type::RR:
        return "RR";
        break;
    case Type::BEEP:
        return "BEEP";
        break;
    case Type::Reserved:
        return "Reserved";
        break;
    case Type::CO:
        return "CO";
        break;
    case Type::SV:
        return "SV";
        break;
    case Type::HRV:
        return "HRV";
        break;
    case Type::EDI:
        return "EDI";
        break;
    case Type::Vol:
        return "Vol";
        break;
    case Type::SVR:
        return "SVR";
        break;
    case Type::SSVR:
        return "SSVR";
        break;
    case Type::SSVRI:
        return "SSVRI";
        break;
    case Type::SVRI:
        return "SVRI";
        break;
    case Type::Vas:
        return "Vas";
        break;
    case Type::LVET:
        return "LVET";
        break;
    case Type::LSW:
        return "LSW";
        break;
    case Type::LSWI:
        return "LSWI";
        break;
    case Type::LCW:
        return "LCW";
        break;
    case Type::LCWI:
        return "LCWI";
        break;
    case Type::STR:
        return "STR";
        break;
    case Type::Ino:
        return "Ino";
        break;
    case Type::SBP:
        return "SBP";
        break;
    case Type::DBP:
        return "DBP";
        break;
    case Type::MAP:
        return "MAP";
        break;
    }
    return nullptr;
}
QString typeName(const Type &type)
{
    return typeName((uchar)type);
}

CustomCtrl::CustomCtrl(CustomCtrlRegulator *reg, Argument arg, QWidget *parent)
    :QWidget{parent}
{
    this->close();
    auto &instance = DataManagement::getInstance();
//    setFixedSize(280*instance.wZoom(),270*instance.hZoom());

    dialog.setFixedSize(240*instance.wZoom(),120*instance.hZoom());
    dialog.setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
    //样式表
    dialog.setStyleSheet(instance.dialogQss());
    timer = new QTimer(this);
    comboBox = new QComboBox(&dialog);
    confirmBtn = new QPushButton(tr("确定"),&dialog);
    comboBox->setFixedWidth(100*instance.wZoom());
    QHBoxLayout *layout = new QHBoxLayout(&dialog);
    dialog.setWindowTitle(tr("选择参数"));
    dialog.setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
    dialog.setLayout(layout);
    layout->addWidget(comboBox);
    layout->addStretch();
    layout->addWidget(confirmBtn);

    connect(timer,&QTimer::timeout,this,&CustomCtrl::timeoutSlot);
    connect(confirmBtn,&QPushButton::clicked,this,&CustomCtrl::confirmSlot);

    QVBoxLayout *vLayout = new QVBoxLayout(this);
    regulator = reg;
    regulator->addCustomCtrl(this);
    frame = new QFrame(this);
    vLayout->addWidget(frame);

    mainLayout = new QVBoxLayout(frame);
    mainLabel = new QLabel(this);
    secondaryLabel = new QLabel(this);
    valueEdit = new QLineEdit(this);
    scopeLabel = new QLabel(this);
    unitLabel = new QLabel(this);

    valueEdit->setEnabled(false);
    valueEdit->setAlignment(Qt::AlignHCenter);
    valueEdit->setPlaceholderText("--");

    mainLayout->addWidget(mainLabel,0,Qt::AlignLeft);
    mainLayout->addWidget(secondaryLabel,0,Qt::AlignLeft);
    mainLayout->addWidget(valueEdit);
    mainLayout->addWidget(scopeLabel,0,Qt::AlignCenter);
    mainLayout->addWidget(unitLabel,0,Qt::AlignCenter);

    mainLabel->setText(arg.en);
    secondaryLabel->setText(arg.cn);
    scopeLabel->setText(QString("%1~%2").arg(arg.min).arg(arg.max));
    unitLabel->setText(arg.unit);
    aitems.minValue = arg.min;
    aitems.maxValue = arg.max;
    aitems.dataName = arg.en;
    aitems.dataUnit = arg.unit;

    //样式表
    int fsize = 10*instance.zoom();
    int msize = 35*instance.zoom();
    int ssize = 25*instance.zoom();
    int vsize = 50*instance.zoom();
    int scsize = 18*instance.zoom();
    int usize = 16*instance.zoom();
    frame->setStyleSheet(QString("QFrame{background-color:#363636;border-radius: %1px;}").arg(fsize));
    mainLabel->setStyleSheet(QString("QLabel{font-size: %1px;color: #ffffff;font-weight:bold;}").arg(msize));
    secondaryLabel->setStyleSheet(QString("QLabel{font-size: %1px;color: #ffffff;}").arg(ssize));
    valueEdit->setStyleSheet(QString("QLineEdit{font-size: %1px;color: #00ff7f;font-weight:bold;"
                             "background:transparent;border-style:outset;}").arg(vsize));
    scopeLabel->setStyleSheet(QString("QLabel{font-size: %1px;color: #ffffff;font-weight:normal;}").arg(scsize));
    unitLabel->setStyleSheet(QString("QLabel{font-size: %1px;color: #ffffff;font-weight:normal;}").arg(usize));
}

void CustomCtrl::setDoubleDataCtrl(Argument arg)
{
    Q_UNUSED(arg);
}

void CustomCtrl::startTimer(qreal accuracy)
{
    m_accuracy = accuracy;
    if (!timer->isActive()) {
        timer->start(2000);
    }
}

void CustomCtrl::stopTimer()
{
    if (timer->isActive()) {
        timer->stop();
    }
}

void CustomCtrl::mouseDoubleClickEvent(QMouseEvent *)
{
    comboBox->clear();
    foreach (const QString &name, regulator->getAllNames()) {
        if(-1 == regulator->getSaveNames().indexOf(name)) {
            comboBox->addItem(name);
        }
    }
    dialog.exec();
}

AItems CustomCtrl::getArgItems()
{
    return aitems;
}

AItems CustomCtrl::getDbpArgItems()
{
    return AItems();
}

double CustomCtrl::getRecordValue()
{
    return aitems.recordValue;
}

double CustomCtrl::getCurrentValue()
{
    return aitems.currentValue;
}

double CustomCtrl::getMinValue() const
{
    return aitems.minValue;
}

double CustomCtrl::getMaxValue() const
{
    return aitems.maxValue;
}

QString CustomCtrl::getName() const
{
    return aitems.dataName;
}

QString CustomCtrl::getValueStr() const
{
    return valueEdit->text();
}

void CustomCtrl::clear()
{
    aitems.recordValue = 0;
    aitems.currentValue = 0;
    valueEdit->clear();
    valueWarning(false);
}

void CustomCtrl::setValue(const double &value, int digit)
{
    double aboutValue = (int)(value*std::pow(10,digit))/(double)std::pow(10,digit);
    valueEdit->setText(QString::number(aboutValue,'f',digit));
    aitems.currentValue = valueEdit->text().toDouble();
    valueWarning(aitems.currentValue < aitems.minValue || aitems.currentValue > aitems.maxValue);
    emit currentValue(aboutValue);
}

void CustomCtrl::setValues(const double &value, const double &value1)
{
    Q_UNUSED(value);
    Q_UNUSED(value1);
}

void CustomCtrl::valueWarning(bool warning)
{
    auto &instance = DataManagement::getInstance();
    int fsize = 10*instance.zoom();
    int vsize = 50*instance.zoom();
    if (warning) {
        valueEdit->setStyleSheet(QString("QLineEdit{font-size:%1px;color:#ffd700;font-weight:bold;"
                    "background:transparent;border-width:0;border-style:outset;}").arg(vsize));
        frame->setStyleSheet(QString("QFrame{background-color:#ff6347;border-radius:%1px;}").arg(fsize));
    }
    else {
        valueEdit->setStyleSheet(QString("QLineEdit{font-size:%1px;color:#00ff7f;font-weight:bold;"
                    "background:transparent;border-width:0;border-style:outset;}").arg(vsize));
        frame->setStyleSheet(QString("QFrame{background-color:#363636;border-radius:%1px;}").arg(fsize));
    }
}

void CustomCtrl::recordValueSlot()
{
    aitems.recordValue = aitems.currentValue;
//    emit recordValue(aitems.recordValue);
}

void CustomCtrl::confirmSlot()
{
    emit changeName(aitems.dataName, comboBox->currentText());
    regulator->saveNames();
    dialog.hide();
}

void CustomCtrl::timeoutSlot()
{
    if (aitems.currentValue != 0) {
        qreal temp = aitems.currentValue + QRandomGenerator::global()->bounded(-2,2)/m_accuracy;
        valueEdit->setText(QString::number(temp));
        valueWarning(temp < aitems.minValue || temp > aitems.maxValue);
    }
}

BpCustomCtrl::BpCustomCtrl(CustomCtrlRegulator *reg, Argument arg, QWidget *parent)
    : CustomCtrl{reg,arg,parent}
{

}

void BpCustomCtrl::setDoubleDataCtrl(Argument arg)
{
    dbpaitems.minValue = arg.min;
    dbpaitems.maxValue = arg.max;
    scopeLabel->setText(QString("%1~%2/%3~%4")
                        .arg(aitems.minValue)
                        .arg(aitems.maxValue)
                        .arg(dbpaitems.minValue)
                        .arg(dbpaitems.maxValue));
}

AItems BpCustomCtrl::getDbpArgItems()
{
    return dbpaitems;
}

void BpCustomCtrl::setValues(const double &value, const double &value1)
{
    valueEdit->setText(QString("%1/%2").arg(value).arg(value1));
    aitems.currentValue = value;
    dbpaitems.currentValue = value1;
    valueWarning(aitems.currentValue < aitems.minValue || aitems.currentValue > aitems.maxValue ||
                 dbpaitems.currentValue < dbpaitems.minValue || dbpaitems.currentValue > dbpaitems.maxValue);
}

void BpCustomCtrl::clear()
{
    aitems.recordValue = 0;
    aitems.currentValue = 0;
    dbpaitems.recordValue = 0;
    dbpaitems.currentValue = 0;
    valueEdit->clear();
    valueWarning(false);
}

void BpCustomCtrl::recordValueSlot()
{
    aitems.recordValue = aitems.currentValue;
    dbpaitems.recordValue = dbpaitems.currentValue;
//    emit recordValue(aitems.recordValue);
}

CustomCtrlRegulator::CustomCtrlRegulator(QObject *parent)
    : QObject{parent}
{
    fileName = DataManagement::getInstance().getPaths().showItems();
    Args args = DataManagement::getInstance().getArgs();
    CO_Widget = new CustomCtrl(this,args.CO);
    CI_Widget = new CustomCtrl(this,args.CI);
    SV_Widget = new CustomCtrl(this,args.SV);
    SI_Widget = new CustomCtrl(this,args.SI);
    HRV_Widget = new CustomCtrl(this,args.HRV);

    TFC_Widget = new CustomCtrl(this,args.TFC);
    EDI_Widget = new CustomCtrl(this,args.EDI);
    Vol_Widget = new CustomCtrl(this,args.Vol);

    SVR_Widget = new CustomCtrl(this,args.SVR);
    SSVR_Widget = new CustomCtrl(this,args.SSVR);
    SSVRI_Widget = new CustomCtrl(this,args.SSVRI);
    SVRI_Widget = new CustomCtrl(this,args.SVRI);
    Vas_Widget = new CustomCtrl(this,args.Vas);

    PEP_Widget = new CustomCtrl(this,args.PEP);
    LVET_Widget = new CustomCtrl(this,args.LVET);
    LSW_Widget = new CustomCtrl(this,args.LSW);
    LSWI_Widget = new CustomCtrl(this,args.LSWI);
    LCW_Widget = new CustomCtrl(this,args.LCW);
    LCWI_Widget = new CustomCtrl(this,args.LCWI);
    STR_Widget = new CustomCtrl(this,args.STR);
    EPCI_Widget = new CustomCtrl(this,args.EPCI);
    ISI_Widget = new CustomCtrl(this,args.ISI);
    Ino_Widget = new CustomCtrl(this,args.Ino);

    HR_Widget = new CustomCtrl(this,args.HR);
    BP_Widget = new BpCustomCtrl(this,args.BP);
    BP_Widget->setDoubleDataCtrl(args.DBP);
    MAP_Widget = new CustomCtrl(this,args.MAP);
}

CustomCtrlRegulator::~CustomCtrlRegulator()
{
    qDeleteAll(allCustomCtrls.begin(),allCustomCtrls.end());
//    qDebug()<<"~CustomCtrlRegulator()";
}

void CustomCtrlRegulator::addCustomCtrl(CustomCtrl *ctrl)
{
    allCustomCtrls.append(ctrl);
}

void CustomCtrlRegulator::addCustomCtrl(QList<CustomCtrl *> ctrls)
{
    allCustomCtrls.append(ctrls);
}

CustomCtrl *CustomCtrlRegulator::getCustomCtrl(const QString &name)
{
    foreach (auto customCtrl, allCustomCtrls) {
        if(customCtrl->getName() == name) {
            return customCtrl;
        }
    }
    return nullptr;
}

QList<CustomCtrl *> CustomCtrlRegulator::getAllCustomCtrls()
{
    return allCustomCtrls;
}

QStringList CustomCtrlRegulator::getSaveNames()
{
    QStringList list;
    QFile file(fileName);
    QFileInfo fileInfo(fileName);
    if(!fileInfo.isFile())
    {
        file.open(QFile::WriteOnly);
        QTextStream in(&file);
        in.setCodec(QTextCodec::codecForName("utf-8"));
        QStringList temp;
        temp<<"HR"<<"TFC"<<"CO"<<"CI"<<"SV"<<"SI"<<"EDI"<<"HRV"<<"ISI"<<"Ino"<<"SSVRI"<<"SBP/DBP";
        foreach (QString str, temp) {
            in<<QString("%1\n").arg(str);
        }
        file.close();
    }
    if(!file.open(QFile::ReadOnly | QFile::Text)){
        emit openError(file.fileName());
        qDebug()<<file.fileName();
        return list;
    }
    QTextStream out(&file);
    while (!out.atEnd()) {
        list<<out.readLine();
    }
    file.close();
    currentNames = list;
    return list;
}

QStringList CustomCtrlRegulator::getAllNames()
{
    QStringList list;
    foreach (auto custromCtrl, allCustomCtrls) {
        list<<custromCtrl->getName();
    }
    return list;
}

QStringList CustomCtrlRegulator::getCurrentNames()
{
    return currentNames;
}

void CustomCtrlRegulator::saveNames()
{
    QFile file(fileName);
    if(!file.open(QFile::WriteOnly)){
        emit openError(file.fileName());
    }
    QTextStream in(&file);
    foreach (auto name, currentNames) {
        in<<QString("%1\n").arg(name);
    }
    file.close();
}

QList<CustomCtrl *> CustomCtrlRegulator::getSaveCustomCtrls()
{
    QList<CustomCtrl *> customCtrls;
    QStringList list = getSaveNames();
    foreach (const QString &name, list) {
        foreach (auto customCtrl, allCustomCtrls) {
            if(name == customCtrl->getName()) {
                customCtrls.append(customCtrl);
                break;
            }
        }
    }
    return customCtrls;
}

void CustomCtrlRegulator::changeCurrentNames(const QString &current, const QString &change)
{
    currentNames[currentNames.indexOf(current)] = change;
}

QList<qreal> CustomCtrlRegulator::getRecordValues()
{
    QList<qreal> list;
    foreach (auto customCtrl, allCustomCtrls) {
        if (customCtrl->getName() != "SBP/DBP") {
            list<<customCtrl->getRecordValue();
        }
        else {
            list<<customCtrl->getRecordValue();
            list<<customCtrl->getDbpArgItems().recordValue;
        }
    }
    return list;
}

QList<qreal> CustomCtrlRegulator::getCurrentValues()
{
    QList<qreal> list;
    foreach (auto customCtrl, allCustomCtrls) {
        if (customCtrl->getName() != "SBP/DBP") {
            list<<customCtrl->getCurrentValue();
        }
        else {
            list<<customCtrl->getCurrentValue();
            list<<customCtrl->getDbpArgItems().currentValue;
        }
    }
    return list;
}
