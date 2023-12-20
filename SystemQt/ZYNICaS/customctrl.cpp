#include "customctrl.h"
#include <math.h>
#include <QRandomGenerator>
#include "datamanagement.h"

CustomCtrl::CustomCtrl(Argument arg, QWidget *parent)
    : QWidget{parent}
{
    this->close();
    auto &instance = DataManagement::getInstance();

    m_pDialog = new SelectItemDialog(false);

    // timer = new QTimer(this);
//    oldAndNewValueTimer = new QTimer(this);

    // connect(timer, &QTimer::timeout,this,&CustomCtrl::timeoutSlot);
//    connect(oldAndNewValueTimer, &QTimer::timeout, this, &CustomCtrl::oldAndNewValueTimerSlot);
    connect(this, &CustomCtrl::currentValue, this, [=]() {
        auto rv = getRecordValue();
        auto cv = getCurrentValue();
        if (cv != 0) {
            if (rv == 0) rv = cv;
            emit nameAndValue(getType(), rv, cv);
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
    m_type = arg.type;

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

    connect(m_pDialog, &SelectItemDialog::currentType, this, &CustomCtrl::getChangeType);
    m_pTrendChart = new TrendChart(this);
    m_pTrendChart->hide();
}

CustomCtrl::~CustomCtrl()
{
    // if (timer->isActive()){
    //     timer->stop();
    // }
//    if (oldAndNewValueTimer->isActive()) {
//        oldAndNewValueTimer->stop();
//    }
    delete m_pTrendChart;
    delete m_pDialog;
//    qDebug()<<aitems.dataName<<"~CustomCtrl()";
}

// void CustomCtrl::startTimer(qreal accuracy)
// {
//     m_accuracy = accuracy;
//     if (!timer->isActive()) {
//         timer->start(2000);
//     }
// }

// void CustomCtrl::stopTimer()
// {
//     if (timer->isActive()) {
//         timer->stop();
//     }
// }

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

Type CustomCtrl::getType() const
{
    return m_type;
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
    valueWarning(0);
}

void CustomCtrl::setValue(const double &value)
{
    aitems.values.append(value);
    valueEdit->setText(QString::number(value, 'f', digit));
    aitems.currentValue = valueEdit->text().toDouble();
    setWarning();
    emit currentValue(value);
}

void CustomCtrl::setValue(const double &value, const Type &type)
{
    if (type == getType()) {
        valueEdit->setText(QString::number(value, 'f', digit));
        aitems.currentValue = valueEdit->text().toDouble();
        setWarning();
        emit currentValue(value);
    }
}

void CustomCtrl::setValues(const double &value, const double &value1)
{
    valueEdit->setText(QString("%1/%2").arg(value).arg(value1));
    aitems.currentValue = value;
    dbpaitems.currentValue = value1;
    valueWarning(aitems.currentValue < aitems.minValue || aitems.currentValue > aitems.maxValue ||
                 dbpaitems.currentValue < dbpaitems.minValue || dbpaitems.currentValue > dbpaitems.maxValue ? 2 : 0);
}

void CustomCtrl::setWarning()
{
    if (aitems.currentValue < aitems.minValue) {
        valueWarning(1);
    }
    else if (aitems.currentValue > aitems.maxValue) {
        valueWarning(2);
    }
    else {
        valueWarning(0);
    }
}

void CustomCtrl::valueWarning(int warning)
{
    if (Type::DO2 == getType()) return;
    auto &instance = DataManagement::getInstance();
    int fsize = 10*instance.zoom();
    int vsize = 50*instance.zoom();
    if (2 == warning) {
        valueEdit->setStyleSheet(QString("QLineEdit{font-size:%1px;color:#ffffff;font-weight:bold;"
                    "background:transparent;border-width:0;border-style:outset;}").arg(vsize));
        frame->setStyleSheet(QString("QFrame{background-color:#FF6347;border-radius:%1px;}").arg(fsize));
    }
    else if (1 == warning) {
        valueEdit->setStyleSheet(QString("QLineEdit{font-size:%1px;color:#ffffff;font-weight:bold;"
                    "background:transparent;border-width:0;border-style:outset;}").arg(vsize));
        frame->setStyleSheet(QString("QFrame{background-color:#FFC107;border-radius:%1px;}").arg(fsize));
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
}

void CustomCtrl::getChangeType(const Type &type)
{
    emit changeType(m_type, type);
    auto &instance = DataManagement::getInstance();
    instance.getRegulator()->saveTypes(DataManagement::getInstance().getPaths().showItems(),
                                       instance.getRegulator()->getCurrentNames(false));
}

void CustomCtrl::timeoutSlot()
{
    if (aitems.currentValue != 0) {
        qreal temp = aitems.currentValue + QRandomGenerator::global()->bounded(-2,2)/m_accuracy;
        valueEdit->setText(QString::number(temp));
        if (temp < aitems.minValue) {
            valueWarning(1);
        }
        else if (temp > aitems.maxValue) {
            valueWarning(2);
        }
        else {
            valueWarning(0);
        }
    }
}

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
}

void CustomCtrlRegulator::addCustomCtrl(CustomCtrl *ctrl)
{
    allCustomCtrls.append(ctrl);
}

void CustomCtrlRegulator::addCustomCtrl(QList<CustomCtrl *> ctrls)
{
    allCustomCtrls.append(ctrls);
}

CustomCtrl *CustomCtrlRegulator::getCustomCtrl(const Type &type)
{
    foreach (auto customCtrl, allCustomCtrls) {
        if(customCtrl->getType() == type) {
            return customCtrl;
        }
    }
    return nullptr;
}

QList<CustomCtrl *> CustomCtrlRegulator::getAllCustomCtrls()
{
    return allCustomCtrls;
}

std::vector<Type> CustomCtrlRegulator::getSaveTypes(bool trendChart)
{
    QJsonArray array;
    if (trendChart) {
        QFile file(DataManagement::getInstance().getPaths().trendCharts());
        if (file.open(QFile::ReadWrite | QFile::Text)) {
            QTextStream stream(&file);
            stream.setCodec(QTextCodec::codecForName("utf-8"));
            if (!stream.atEnd()) {
                array = QJsonDocument::fromJson(stream.readLine().toUtf8()).array();
            }
            if (array.count() != 12 || !isSubset(array, false)) {
                array = QJsonArray{ Type::CO, Type::CI, Type::SV, Type::SI, Type::HRV, Type::Vol,
                                    Type::SSVRI, Type::SVRI, Type::Vas, Type::ISI, Type::Ino, Type::HR };
                stream<<QString(QJsonDocument(array).toJson(QJsonDocument::Compact));
            }
            file.close();
        }
    }
    else {
        QFile file(DataManagement::getInstance().getPaths().showItems());
        if (file.open(QFile::ReadWrite | QFile::Text)) {
            QTextStream stream(&file);
            stream.setCodec(QTextCodec::codecForName("utf-8"));
            if (!stream.atEnd()) {
                array = QJsonDocument::fromJson(stream.readLine().toUtf8()).array();
            }
            if (array.count() != 11 || !isSubset(array, true)) {
                array = QJsonArray{ Type::HR, Type::TFC, Type::CO, Type::CI, Type::SV, Type::SI,
                                    Type::HRV, Type::ISI, Type::Ino, Type::SSVRI, Type::SBP };
                stream<<QString(QJsonDocument(array).toJson(QJsonDocument::Compact));
            }
            file.close();
        }
    }
    if (trendChart) {
        currentTrendChartTypes = arrayToVector(array);
    }
    else {
        currentTypes = arrayToVector(array);
    }
    return arrayToVector(array);
}


std::vector<Type> CustomCtrlRegulator::getAllTypes()
{
    std::vector<Type> types;
    foreach (auto custromCtrl, allCustomCtrls) {
        types.push_back(custromCtrl->getType());
    }
    return types;
}

std::vector<Type> CustomCtrlRegulator::getCurrentNames(bool trendChart)
{
    if (trendChart) {
        return currentTrendChartTypes;
    }
    return currentTypes;
}

void CustomCtrlRegulator::saveTypes(const QString &fileName, const std::vector<Type> &types)
{
    QFile file(fileName);
    if(!file.open(QFile::WriteOnly)){
        emit openError(file.fileName());
    }
    QTextStream in(&file);
    QJsonArray array;
    foreach (auto type, types) {
        array<<type;
    }
    in<<QString(QJsonDocument(array).toJson(QJsonDocument::Compact));
    file.close();
}

QList<CustomCtrl *> CustomCtrlRegulator::getSaveCustomCtrls(bool trendChart)
{
    QList<CustomCtrl *> customCtrls;
    auto types = getSaveTypes(trendChart);
    foreach (auto type, types) {
        foreach (auto customCtrl, allCustomCtrls) {
            if(type == customCtrl->getType()) {
                customCtrls.append(customCtrl);
                break;
            }
        }
    }
    return customCtrls;
}

void CustomCtrlRegulator::changeCurrentTypes(const Type &current, const Type &change, bool trendChart)
{
    if (trendChart) {
        for (int i = 0; i < currentTrendChartTypes.size(); ++i) {
            if (currentTrendChartTypes.at(i) == current) {
                currentTrendChartTypes[i] = change;
                break;
            }
        }
    }
    else {
        for (int i = 0; i < currentTypes.size(); ++i) {
            if (currentTypes.at(i) == current) {
                currentTypes[i] = change;
                break;
            }
        }
    }
}

QList<qreal> CustomCtrlRegulator::getRecordValues()
{
    QList<qreal> list;
    foreach (auto customCtrl, allCustomCtrls) {
        if (customCtrl->getType() != Type::SBP) {
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
        if (customCtrl->getType() != Type::SBP) {
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

bool CustomCtrlRegulator::isSubset(const QJsonArray &array, bool showItem) const
{
    std::vector<Type> atypes {
        Type::CO, Type::CI, Type::SV, Type::SI, Type::TFC, Type::EDI,
        Type::SVV, Type::Vol, Type::SVR, Type::SSVR, Type::SSVRI, Type::SVRI, Type::Vas,
        Type::PEP, Type::LVET, Type::LSW, Type::LSWI, Type::LCW,Type::LCWI, Type::STR,
        Type::EPCI, Type::ISI, Type::Ino, Type::HR, Type::HRV
    };
    if (showItem) {
        atypes.push_back(Type::DO2);
        atypes.push_back(Type::SBP);
        atypes.push_back(Type::MAP);
    }
    std::vector<Type> types;
    foreach (auto value, array) {
        types.push_back(Type(value.toInt(-1)));
    }
    std::sort(atypes.begin(), atypes.end());
    std::sort(types.begin(), types.end());
    int i = 0, j = 0;
    auto m = atypes.size();
    auto n = types.size();
    if (0 == n || m < n) {
        return false;
    }
    while (i < n && j < m) {
        if (atypes[j] < types[i]) {
            ++j;
        }
        else if (atypes[j] == types[i]) {
            ++j;
            ++i;
        }
        else {
            return false;
        }
    }
    if (i < n) {
        return false;
    }
    return true;
}

std::vector<Type> CustomCtrlRegulator::arrayToVector(const QJsonArray &array) const
{
    std::vector<Type> vector;
    foreach (auto value, array) {
        vector.push_back(Type(value.toInt(-1)));
    }
    return vector;
}
