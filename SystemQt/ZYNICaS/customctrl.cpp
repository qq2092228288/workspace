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
    case Type::CVP:
        return "CVP";
        break;
    case Type::LAP:
        return "LAP";
        break;
    case Type::Dz:
        return "Dz";
        break;
    case Type::DO2:
        return "DO2";
        break;
    case Type::SVV:
        return "SVV";
        break;
    default:
        break;
    }
    return nullptr;
}
QString typeName(const Type &type)
{
    return typeName((uchar)type);
}

CustomCtrl::CustomCtrl(Argument arg, QWidget *parent)
    : QWidget{parent}
{
    this->close();
    auto &instance = DataManagement::getInstance();

    m_pDialog = new SelectItemDialog(false);

    timer = new QTimer(this);
//    oldAndNewValueTimer = new QTimer(this);

    connect(timer, &QTimer::timeout,this,&CustomCtrl::timeoutSlot);
//    connect(oldAndNewValueTimer, &QTimer::timeout, this, &CustomCtrl::oldAndNewValueTimerSlot);
    connect(this, &CustomCtrl::currentValue, this, [=]() {
        auto rv = getRecordValue();
        auto cv = getCurrentValue();
        if (cv != 0) {
            if (rv == 0) rv = cv;
            emit nameAndValue(getName(), rv, cv);
        }
    });

    QVBoxLayout *vLayout = new QVBoxLayout(this);
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

    mainLayout->addWidget(mainLabel, 0, Qt::AlignLeft);
    mainLayout->addWidget(secondaryLabel, 0, Qt::AlignLeft);
    mainLayout->addWidget(valueEdit);
    mainLayout->addWidget(scopeLabel, 0, Qt::AlignCenter);
    mainLayout->addWidget(unitLabel, 0, Qt::AlignCenter);


    aitems.minValue = arg.min;
    aitems.maxValue = arg.max;
    aitems.dataName = (arg.dbpen.isEmpty() ? arg.en : arg.en + "/" + arg.dbpen);
    aitems.dataName_cn = (arg.dbpcn.isEmpty() ? arg.cn : arg.cn + "/" +arg.dbpcn);
    aitems.dataUnit = arg.unit;
    digit = arg.digit;

    mainLabel->setText(aitems.dataName);
    secondaryLabel->setText(aitems.dataName_cn);
    scopeLabel->setText(QString("%1~%2").arg(arg.min).arg(arg.max));
    unitLabel->setText(arg.unit);

    if (arg.dbpmin != 0 && arg.dbpmax != 0) {
        dbpaitems = aitems;
        dbpaitems.minValue = arg.dbpmin;
        dbpaitems.maxValue = arg.dbpmax;

        scopeLabel->setText(QString("%1~%2/%3~%4").arg(aitems.minValue).arg(aitems.maxValue)
                            .arg(dbpaitems.minValue).arg(dbpaitems.maxValue));
    }
    if (arg.min == arg.max) {
        scopeLabel->setText("-");
    }

    //样式表
    int fsize = 10*instance.zoom();
    int msize = 35*instance.zoom();
    int ssize = 25*instance.zoom();
    int vsize = 50*instance.zoom();
    int scsize = 18*instance.zoom();
    int usize = 16*instance.zoom();
    frame->setStyleSheet(QString("QFrame{background-color:#363636;border-radius:%1px;}").arg(fsize));
    mainLabel->setStyleSheet(QString("QLabel{font-size: %1px;color: #ffffff;font-weight:bold;}").arg(msize));
    secondaryLabel->setStyleSheet(QString("QLabel{font-size: %1px;color: #ffffff;}").arg(ssize));
    valueEdit->setStyleSheet(QString("QLineEdit{font-size:%1px;color:#00ff7f;font-weight:bold;"
                                     "background:transparent;border-width:0;border-style:outset;}").arg(vsize));
    scopeLabel->setStyleSheet(QString("QLabel{font-size: %1px;color: #ffffff;font-weight:normal;}").arg(scsize));
    unitLabel->setStyleSheet(QString("QLabel{font-size: %1px;color: #ffffff;font-weight:normal;}").arg(usize));

    connect(m_pDialog, &SelectItemDialog::currentText, this, &CustomCtrl::getChangeText);
    m_pTrendChart = new TrendChart(this);
    m_pTrendChart->hide();
}

CustomCtrl::~CustomCtrl()
{
    if (timer->isActive()){
        timer->stop();
    }
//    if (oldAndNewValueTimer->isActive()) {
//        oldAndNewValueTimer->stop();
//    }
    delete m_pTrendChart;
    delete m_pDialog;
//    qDebug()<<aitems.dataName<<"~CustomCtrl()";
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

//void CustomCtrl::smoothTransitionTimer(bool isStart)
//{
//    if (getName() == "MAP" || getName() == "SBP/DBP") {
//        return;
//    }
//    if (isStart) {
//        if (!oldAndNewValueTimer->isActive()) {
//            oldAndNewValueTimer->start(2500);
//        }
//    }
//    else {
//        if (oldAndNewValueTimer->isActive()) {
//            oldAndNewValueTimer->stop();
//        }
//    }
//}

void CustomCtrl::mouseDoubleClickEvent(QMouseEvent *)
{
    m_pDialog->exec();
}

ArgItems CustomCtrl::getArgItems()
{
    return aitems;
}

ArgItems CustomCtrl::getDbpArgItems()
{
    return dbpaitems;
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

TrendChart *CustomCtrl::getTrendChart()
{
    return m_pTrendChart;
}

int CustomCtrl::getDigit() const
{
    return digit;
}

void CustomCtrl::clear()
{
    aitems.clear();
    dbpaitems.clear();
    valueEdit->clear();
    m_pTrendChart->clear();
//    oldAndNewValue.clear();
    valueWarning(false);
}

void CustomCtrl::setValue(const double &value)
{
//    aitems.vqueue.enqueue(value);
    aitems.values.append(value);
    valueEdit->setText(QString::number(value, 'f', digit));
    aitems.currentValue = valueEdit->text().toDouble();
    valueWarning(aitems.currentValue < aitems.minValue || aitems.currentValue > aitems.maxValue);
    emit currentValue(value);
//    oldAndNewValue.append(value);
//    if (oldAndNewValue.size() > 2) {
//        oldAndNewValue.removeFirst();
//    }
//    else {
//        valueEdit->setText(QString::number(value, 'f', digit));
//        aitems.currentValue = valueEdit->text().toDouble();
//        valueWarning(aitems.currentValue < aitems.minValue || aitems.currentValue > aitems.maxValue);
//        emit currentValue(value);
    //    }
}

void CustomCtrl::setValue(const double &value, const QString &name)
{
    if (name == getName()) {
        valueEdit->setText(QString::number(value, 'f', digit));
        aitems.currentValue = valueEdit->text().toDouble();
        valueWarning(aitems.currentValue < aitems.minValue || aitems.currentValue > aitems.maxValue);
        emit currentValue(value);
    }
}

void CustomCtrl::setValues(const double &value, const double &value1)
{
    valueEdit->setText(QString("%1/%2").arg(value).arg(value1));
    aitems.currentValue = value;
    dbpaitems.currentValue = value1;
    valueWarning(aitems.currentValue < aitems.minValue || aitems.currentValue > aitems.maxValue ||
                 dbpaitems.currentValue < dbpaitems.minValue || dbpaitems.currentValue > dbpaitems.maxValue);
}

void CustomCtrl::valueWarning(bool warning)
{
    if (getName() == "DO2") return;
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
    dbpaitems.recordValue = dbpaitems.currentValue;
//    aitems.vqueue.clear();
//    dbpaitems.vqueue.clear();
    //    emit recordValue(aitems.recordValue);
}

void CustomCtrl::getChangeText(const QString &text)
{
    emit changeName(aitems.dataName, text);
    auto &instance = DataManagement::getInstance();
    instance.getRegulator()->saveNames(DataManagement::getInstance().getPaths().showItems(),
                                       instance.getRegulator()->getCurrentNames(false));
}

void CustomCtrl::timeoutSlot()
{
    if (aitems.currentValue != 0) {
        qreal temp = aitems.currentValue + QRandomGenerator::global()->bounded(-2,2)/m_accuracy;
        valueEdit->setText(QString::number(temp));
        valueWarning(temp < aitems.minValue || temp > aitems.maxValue);
    }
}

//void CustomCtrl::oldAndNewValueTimerSlot()
//{
//    if (oldAndNewValue.size() == 2) {
//        qreal oldValue = oldAndNewValue.at(0);
//        qreal newValue = oldAndNewValue.at(1);
//        if ((oldValue < newValue && newValue/oldValue > 1.1) || (oldValue > newValue && oldValue/newValue > 1.1)) {
//            oldValue += (newValue - oldValue)*0.3;
//        }
//        else {
//            oldValue = newValue;
//        }
//        oldAndNewValue.replace(0, oldValue);
//    }
//    if (!oldAndNewValue.isEmpty()) {
//        valueEdit->setText(QString::number(oldAndNewValue.at(0), 'f', digit));
//        aitems.currentValue = valueEdit->text().toDouble();
//        valueWarning(aitems.currentValue < aitems.minValue || aitems.currentValue > aitems.maxValue);
//        emit currentValue(aitems.currentValue);
//    }
//}

CustomCtrlRegulator::CustomCtrlRegulator(QObject *parent)
    : QObject{parent}
{
    QList<Argument> args = DataManagement::getInstance().getArgs().arguments;
    foreach (Argument argument, args) {
        CustomCtrl *widget = new CustomCtrl(argument);
        this->addCustomCtrl(widget);
    }
}

CustomCtrlRegulator::~CustomCtrlRegulator()
{
    qDeleteAll(allCustomCtrls.begin(), allCustomCtrls.end());
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

CustomCtrl *CustomCtrlRegulator::getCustomCtrl(const QString &cname)
{
    foreach (auto customCtrl, allCustomCtrls) {
        if(customCtrl->getName() == cname) {
            return customCtrl;
        }
    }
    return nullptr;
}

QList<CustomCtrl *> CustomCtrlRegulator::getAllCustomCtrls()
{
    return allCustomCtrls;
}

QStringList CustomCtrlRegulator::getSaveNames(bool trendChart)
{
    QStringList list;
    if (trendChart) {
        QFile file(DataManagement::getInstance().getPaths().trendCharts());
        if (file.open(QFile::ReadWrite | QFile::Text)) {
            QTextStream stream(&file);
            stream.setCodec(QTextCodec::codecForName("utf-8"));
            while (!stream.atEnd()) {
                list<<stream.readLine();
            }
            if (list.size() != 12) {
                list.clear();
                list<<"CO"<<"CI"<<"SV"<<"SI"<<"HRV"<<"Vol"<<"SSVRI"<<"Vas"<<"ISI"<<"Ino"<<"HR"<<"DO2";
                foreach (QString str, list) {
                    stream<<QString("%1\n").arg(str);
                }
            }
        }
    }
    else {
        QFile file(DataManagement::getInstance().getPaths().showItems());
        if (file.open(QFile::ReadWrite | QFile::Text)) {
            QTextStream stream(&file);
            stream.setCodec(QTextCodec::codecForName("utf-8"));
            while (!stream.atEnd()) {
                list<<stream.readLine();
            }
            if (list.size() != 11) {
                list.clear();
                list<<"HR"<<"TFC"<<"CO"<<"CI"<<"SV"<<"SI"<<"HRV"<<"ISI"<<"Ino"<<"SSVRI"<<"SBP/DBP";
                foreach (QString str, list) {
                    stream<<QString("%1\n").arg(str);
                }
            }
        }
    }
    if (trendChart) {
        currentTrendChartNames = list;
    }
    else {
        currentNames = list;
    }
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

QStringList CustomCtrlRegulator::getCurrentNames(bool trendChart)
{
    if (trendChart) {
        return currentTrendChartNames;
    }
    return currentNames;
}

void CustomCtrlRegulator::saveNames(const QString &fileName, const QStringList &list)
{
    QFile file(fileName);
    if(!file.open(QFile::WriteOnly)){
        emit openError(file.fileName());
    }
    QTextStream in(&file);
    foreach (auto name, list) {
        in<<QString("%1\n").arg(name);
    }
    file.close();
}

QList<CustomCtrl *> CustomCtrlRegulator::getSaveCustomCtrls(bool trendChart)
{
    QList<CustomCtrl *> customCtrls;
    QStringList list = getSaveNames(trendChart);
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

void CustomCtrlRegulator::changeCurrentNames(const QString &current, const QString &change, bool trendChart)
{
    if (trendChart) {
        currentTrendChartNames[currentTrendChartNames.indexOf(current)] = change;
    }
    else {
        currentNames[currentNames.indexOf(current)] = change;
    }

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

void CustomCtrlRegulator::connectTrendChart(bool con)
{
    foreach (auto customCtrl, allCustomCtrls) {
        if (con) {
            connect(customCtrl, &CustomCtrl::currentValue, customCtrl->getTrendChart(), &TrendChart::addValue,
                    Qt::ConnectionType(Qt::AutoConnection|Qt::UniqueConnection));
        }
        else {
            disconnect(customCtrl, &CustomCtrl::currentValue, customCtrl->getTrendChart(), &TrendChart::addValue);
        }
    }
}
