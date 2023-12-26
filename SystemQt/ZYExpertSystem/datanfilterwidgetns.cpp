#include "datanfilterwidgetns.h"
#include "reportdataname.h"
#include <QFile>
#include <QMessageBox>

namespace DatanFilterWidgetNs {

BaseFilter::BaseFilter(bool check, QWidget *parent)
    : QWidget{parent}
    , m_check{check}
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
    mainLayout->addStretch();

    radioGroup->addButton(allButton, FilterType::All);
    radioGroup->addButton(partButton, FilterType::Part);

    // mainCheckBox->setFixedWidth(70);

    connect(radioGroup, &QButtonGroup::idClicked, this, &BaseFilter::radioButtonClicked);
    connect(mainCheckBox, &QCheckBox::stateChanged, this, &BaseFilter::checked);

    init();
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

QString BaseFilter::text() const
{
    return mainCheckBox->text();
}

void BaseFilter::init()
{
    mainCheckBox->setChecked(m_check);
    allButton->setChecked(true);
}

void BaseFilter::hideFilterCriteria()
{
    hideLayout(aLayout);
    hideLayout(pLayout);
}

void BaseFilter::showFilterCriteria()
{
    showLayout(aLayout);
    showLayout(pLayout);
}

void BaseFilter::hideLayout(QLayout *layout)
{
    for (int i = 0; i < layout->count(); ++i) {
        auto item = layout->itemAt(i);
        if (item->widget() != nullptr) {
            item->widget()->hide();
        }
        else if (item->layout() != nullptr) {
            hideLayout(item->layout());
        }
    }
}

void BaseFilter::showLayout(QLayout *layout)
{
    for (int i = 0; i < layout->count(); ++i) {
        auto item = layout->itemAt(i);
        if (item->widget() != nullptr) {
            item->widget()->show();
        }
        else if (item->layout() != nullptr) {
            hideLayout(item->layout());
        }
    }
}

void BaseFilter::allLayoutAdd(QLayout *layout)
{
    aLayout->addLayout(layout);
}

void BaseFilter::partLayoutAdd(QLayout *layout)
{
    pLayout->addLayout(layout);
}

PositionFilter::PositionFilter(QWidget *parent)
    : BaseFilter{true, parent}
{
    auto lLayout = new QHBoxLayout;
    label = new QLabel(QString::fromUtf8("仅第一体位"), this);

    lLayout->addWidget(label);
    partLayoutAdd(lLayout);

    setMain(QString::fromUtf8("体位"));
}

void PositionFilter::radioButtonClicked(int id)
{
    Q_UNUSED(id)
}

DataFilter::DataFilter(QWidget *parent)
    : BaseFilter{true, parent}
{
    auto mmLayout = new QHBoxLayout;
    firstLineEdit = new QLineEdit(this);
    label = new QLabel("-", this);
    secondLineEdit = new QLineEdit(this);
    unitLabel = new QLabel(this);

    mmLayout->addWidget(firstLineEdit);
    mmLayout->addWidget(label);
    mmLayout->addWidget(secondLineEdit);
    mmLayout->addWidget(unitLabel);
    partLayoutAdd(mmLayout);

    firstLineEdit->setFixedWidth(60);
    secondLineEdit->setFixedWidth(60);

    QRegExp exp("^[1-9][0-9]{0,1}|[1,2][0-9]{2}|300$");
    setRegExp(exp);

    init();
}

double DataFilter::first() const
{
    return firstLineEdit->text().toDouble();
}

double DataFilter::second() const
{
    return secondLineEdit->text().toDouble();
}

void DataFilter::setUnit(const QString &unit)
{
    unitLabel->setText(unit);
}

QString DataFilter::unit() const
{
    return unitLabel->text();
}

void DataFilter::init()
{
    firstLineEdit->clear();
    secondLineEdit->clear();
    BaseFilter::init();
    selectAll(true);
}

void DataFilter::setRegExp(const QRegExp &exp)
{
    auto validator = new QRegExpValidator(exp, this);
    firstLineEdit->setValidator(validator);
    secondLineEdit->setValidator(validator);
}

void DataFilter::selectAll(bool all)
{
    firstLineEdit->setEnabled(!all);
    secondLineEdit->setEnabled(!all);
}

void DataFilter::setData(const double &f, const double &s)
{
    firstLineEdit->setText(QString::number(f));
    secondLineEdit->setText(QString::number(s));
}

void DataFilter::radioButtonClicked(int id)
{
    selectAll(FilterType::All == id);
}

GeneralFilter::GeneralFilter(bool check, QWidget *parent)
    : BaseFilter{check, parent}
{
    auto lLayout = new QHBoxLayout;
    lineEdit = new QLineEdit(this);

    lLayout->addWidget(lineEdit);
    partLayoutAdd(lLayout);

    lineEdit->setFixedWidth(138);

    init();
}

void GeneralFilter::init()
{
    BaseFilter::init();
    lineEdit->setEnabled(false);
    lineEdit->clear();
}

void GeneralFilter::radioButtonClicked(int id)
{
    lineEdit->setEnabled(FilterType::Part == id);
}

SexFilter::SexFilter(QWidget *parent)
    : BaseFilter{true, parent}
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

    init();
}

void SexFilter::init()
{
    BaseFilter::init();
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
    : BaseFilter{true, parent}
{
    dialog = new ScopeCalendarDialog(this);
    setMain(QString::fromUtf8("时间"));
    connect(dialog, &ScopeCalendarDialog::scopeTime, this, &TimeFilter::scopeTime);

    init();
}

void TimeFilter::init()
{
    BaseFilter::init();
    m_start = 0;
    m_end = 0;
}

void TimeFilter::radioButtonClicked(int id)
{
    if (1 == id) {
        dialog->exec();
    }
}

void TimeFilter::scopeTime(qint64 start, qint64 end)
{
    m_start = start;
    m_end = end;
}

NicasFilter::NicasFilter(const QJsonObject &object, QWidget *parent)
    : DataFilter{parent}
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
    belowCheckBox = new QCheckBox("低于", this);
    normalCheckBox = new QCheckBox("正常", this);
    aboveCheckBox = new QCheckBox("高于", this);

    lnhLayout->addWidget(belowCheckBox);
    lnhLayout->addWidget(normalCheckBox);
    lnhLayout->addWidget(aboveCheckBox);
    allLayoutAdd(lnhLayout);

    group->setExclusive(false);
    group->addButton(belowCheckBox, ScopeType::Low);
    group->addButton(normalCheckBox, ScopeType::Normal);
    group->addButton(aboveCheckBox, ScopeType::High);

    setRegExp(QRegExp("^(-?\\d{0,5})(\\.\\d{0,3})?$"));

    setMain(QString("%1/%2").arg(m_cn, m_en));
    setUnit(m_unit);

    connect(group, &QButtonGroup::idClicked, this, &NicasFilter::checkBoxClicked);

    init();
}

NicasFilter::ScopeType NicasFilter::stype() const
{
    if (isAll()) {
        return ScopeType(static_cast<char>(belowCheckBox->isChecked()) |
                         static_cast<char>(normalCheckBox->isChecked()) << 1 |
                         static_cast<char>(aboveCheckBox->isChecked()) << 2);
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

void NicasFilter::init()
{
    DataFilter::init();
    selectAll(true);
    setData(m_min, m_max);
    belowCheckBox->setChecked(true);
    normalCheckBox->setChecked(true);
    aboveCheckBox->setChecked(true);
}

void NicasFilter::radioButtonClicked(int id)
{
    selectAll(FilterType::All == id);
}

void NicasFilter::checkBoxClicked(int id)
{
    if (!(belowCheckBox->isChecked() || normalCheckBox->isChecked() || aboveCheckBox->isChecked())) {
        QMessageBox::warning(this, tr("提示"), QString::fromUtf8("至少选中一项！"));
        group->button(id)->setChecked(true);
    }
}

void NicasFilter::selectAll(bool all)
{
    DataFilter::selectAll(all);
    belowCheckBox->setEnabled(all);
    normalCheckBox->setEnabled(all);
    aboveCheckBox->setEnabled(all);
}

}
