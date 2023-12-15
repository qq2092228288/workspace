#include "datanfilterwidgetns.h"
#include "reportdataname.h"
#include <QFile>
#include <QMessageBox>

namespace DatanFilterWidgetNs {

BaseFilter::BaseFilter(QWidget *parent)
    : QWidget{parent}
{
    QFile qssFile(":/qss/datanwidgetui.qss");
    if (qssFile.open(QIODevice::ReadOnly))
        setStyleSheet(qssFile.readAll());
    setContentsMargins(0, 0, 0, 0);

    mainLayout = new QHBoxLayout(this);
    aLayout = new QHBoxLayout;
    pLayout = new QHBoxLayout;
    mainCheckBox = new QCheckBox(this);
    radioGroup = new QButtonGroup(this);
    allButton = new QRadioButton(QString::fromUtf8("全选"), this);
    partButton = new QRadioButton(QString::fromUtf8("指定"), this);

    aLayout->addWidget(allButton);
    pLayout->addWidget(partButton);
    mainLayout->addWidget(mainCheckBox);
    mainLayout->addLayout(aLayout);
    mainLayout->addLayout(pLayout);

    radioGroup->addButton(allButton, FilterType::All);
    radioGroup->addButton(partButton, FilterType::Part);

    mainCheckBox->setFixedWidth(50);

    mainCheckBox->setChecked(true);
    allButton->setChecked(true);

    connect(radioGroup, &QButtonGroup::idClicked, this, &BaseFilter::radioButtonClicked);
    connect(mainCheckBox, &QCheckBox::stateChanged, this, &BaseFilter::checked);
}

void BaseFilter::setMain(const QString &text, const QString &tip)
{
    mainCheckBox->setText(text);
    mainCheckBox->setToolTip(tip);
}

bool BaseFilter::isChecked() const
{
    return mainCheckBox->isChecked();
}

bool BaseFilter::isAll() const
{
    return allButton->isChecked();
}

void BaseFilter::allLayoutAdd(QLayout *layout)
{
    aLayout->addLayout(layout);
}

void BaseFilter::partLayoutAdd(QLayout *layout)
{
    pLayout->addLayout(layout);
}

DataFilter::DataFilter(const QString &text, const QString &unit, QWidget *parent)
    : BaseFilter{parent}
{
    auto mmLayout = new QHBoxLayout;
    firstLineEdit = new QLineEdit(this);
    label = new QLabel("-", this);
    secondLineEdit = new QLineEdit(this);

    mmLayout->addWidget(firstLineEdit);
    mmLayout->addWidget(label);
    mmLayout->addWidget(secondLineEdit);
    partLayoutAdd(mmLayout);

    firstLineEdit->setFixedWidth(60);
    secondLineEdit->setFixedWidth(60);

    auto validator = new QRegExpValidator(QRegExp("^[1-9][0-9]{0,1}|[1,2][0-9]{2}|300$"), this);
    firstLineEdit->setValidator(validator);
    secondLineEdit->setValidator(validator);

    setMain(text, unit);

    selectAll(true);
}

void DataFilter::setRegExp(const QRegExp &exp)
{
    auto validator = new QRegExpValidator(exp, this);
    firstLineEdit->setValidator(validator);
    secondLineEdit->setValidator(validator);
}

void DataFilter::radioButtonClicked(int id)
{
    selectAll(FilterType::All == id);
}

void DataFilter::selectAll(bool all)
{
    firstLineEdit->setEnabled(!all);
    secondLineEdit->setEnabled(!all);
}

StrFilter::StrFilter(const QString &text, QWidget *parent)
    : BaseFilter{parent}
{
    auto lLayout = new QHBoxLayout;
    lineEdit = new QLineEdit(this);

    lLayout->addWidget(lineEdit);
    partLayoutAdd(lLayout);

    lineEdit->setFixedWidth(138);

    setMain(text);
    lineEdit->setEnabled(false);
}

void StrFilter::radioButtonClicked(int id)
{
    lineEdit->setEnabled(FilterType::Part == id);
}

SexFilter::SexFilter(QWidget *parent)
    : BaseFilter{parent}
{
    auto gLayout = new QHBoxLayout;
    genderGroup = new QButtonGroup(this);
    maleButton = new QRadioButton(QString::fromUtf8("男"), this);
    femaleButton = new QRadioButton(QString::fromUtf8("女"), this);

    gLayout->addWidget(maleButton);
    gLayout->addWidget(femaleButton);
    partLayoutAdd(gLayout);

    genderGroup->addButton(maleButton, 0);
    genderGroup->addButton(femaleButton, 1);

    setMain(QString::fromUtf8("性别"));
    maleButton->setChecked(true);
    selectAll(true);
}

void SexFilter::radioButtonClicked(int id)
{
    selectAll(FilterType::All == id);
}

void SexFilter::selectAll(bool all)
{
    maleButton->setEnabled(!all);
    femaleButton->setEnabled(!all);
}

TimeFilter::TimeFilter(QWidget *parent)
    : BaseFilter{parent}
{
    dialog = new ScopeCalendarDialog(this);
    setMain(QString::fromUtf8("时间"));
}

void TimeFilter::radioButtonClicked(int id)
{
    if (1 == id) {
        dialog->exec();
    }
}

NicasFilter::NicasFilter(const QJsonObject &object, QWidget *parent)
    : BaseFilter{parent}
{
    m_cn = object.value(ReportDataName::ekey(ReportDataName::cn)).toString();
    m_en = object.value(ReportDataName::ekey(ReportDataName::en)).toString();
    m_unit = object.value(ReportDataName::ekey(ReportDataName::unit)).toString();
    m_min = object.value(ReportDataName::ekey(ReportDataName::min)).toDouble();
    m_max = object.value(ReportDataName::ekey(ReportDataName::max)).toDouble();
    m_digit = object.value(ReportDataName::ekey(ReportDataName::digit)).toDouble();
    m_type = object.value(ReportDataName::ekey(ReportDataName::type)).toDouble();

    group = new QButtonGroup(this);
    auto lnhLayout = new QHBoxLayout;
    lowCheckBox = new QCheckBox("L", this);
    normalCheckBox = new QCheckBox("N", this);
    highCheckBox = new QCheckBox("H", this);

    lnhLayout->addWidget(lowCheckBox);
    lnhLayout->addWidget(normalCheckBox);
    lnhLayout->addWidget(highCheckBox);
    allLayoutAdd(lnhLayout);

    auto mmLayout = new QHBoxLayout;
    firstLineEdit = new QLineEdit(this);
    label = new QLabel("-", this);
    secondLineEdit = new QLineEdit(this);

    mmLayout->addWidget(firstLineEdit);
    mmLayout->addWidget(label);
    mmLayout->addWidget(secondLineEdit);
    partLayoutAdd(mmLayout);

    group->setExclusive(false);
    group->addButton(lowCheckBox, ScopeType::Low);
    group->addButton(normalCheckBox, ScopeType::Normal);
    group->addButton(highCheckBox, ScopeType::High);

    firstLineEdit->setFixedWidth(60);
    secondLineEdit->setFixedWidth(60);

    auto validator = new QRegExpValidator(QRegExp("^(-?\\d{0,5})(\\.\\d{0,3})?$"), this);
    firstLineEdit->setValidator(validator);
    secondLineEdit->setValidator(validator);

    lowCheckBox->setChecked(true);
    normalCheckBox->setChecked(true);
    highCheckBox->setChecked(true);

    auto tip = QString("%1 (%2)").arg(m_cn, m_unit);
    setMain(m_en, tip);
    firstLineEdit->setText(QString::number(m_min));
    secondLineEdit->setText(QString::number(m_max));

    selectAll(true);

    connect(group, &QButtonGroup::idClicked, this, &NicasFilter::checkBoxClicked);
}

NicasFilter::ScopeType NicasFilter::stype() const
{
    if (isAll()) {
        return ScopeType(static_cast<char>(lowCheckBox->isChecked()) |
                         static_cast<char>(normalCheckBox->isChecked()) << 1 |
                         static_cast<char>(highCheckBox->isChecked()) << 2);
    }
    return ScopeType::Scope;
}

QString NicasFilter::cn() const
{
    return m_cn;
}

QString NicasFilter::en() const
{
    return m_en;
}

QString NicasFilter::unit() const
{
    return m_unit;
}

double NicasFilter::min() const
{
    return m_min;
}

double NicasFilter::max() const
{
    return m_max;
}

int NicasFilter::digit() const
{
    return m_digit;
}

int NicasFilter::type() const
{
    return m_type;
}

void NicasFilter::radioButtonClicked(int id)
{
    selectAll(FilterType::All == id);
}

void NicasFilter::checkBoxClicked(int id)
{
    if (!(lowCheckBox->isChecked() || normalCheckBox->isChecked() || highCheckBox->isChecked())) {
        QMessageBox::warning(this, tr("提示"), QString::fromUtf8("至少选中一项！"));
        group->button(id)->setChecked(true);
    }
}

void NicasFilter::selectAll(bool all)
{
    lowCheckBox->setEnabled(all);
    normalCheckBox->setEnabled(all);
    highCheckBox->setEnabled(all);
    firstLineEdit->setEnabled(!all);
    secondLineEdit->setEnabled(!all);
}

}
