#include "nicasdatafilterwidget.h"
#include "reportset.h"


NicasDataFilterWidget::NicasDataFilterWidget(QWidget *parent)
    : QWidget{parent}
{
    auto mainLayout = new QVBoxLayout(this);
    auto rLayout = new QHBoxLayout;
    buttonGroup = new QButtonGroup(this);
    widget = new QStackedWidget(this);

    mainLayout->addLayout(rLayout);
    mainLayout->addWidget(widget);

    auto array = ReportParameters::array(Check_Mode::InternalMedicine);
    for (int index = 0; index < array.size(); ++index) {
        auto module = array.at(index).toObject();
        auto button = new QRadioButton(module.value(ReportDataName::ekey(ReportDataName::module)).toString(), this);
        rLayout->addWidget(button);
        buttonGroup->addButton(button, index);
        auto groupBox = new QGroupBox(widget);
        auto groupLayout = new QVBoxLayout;
        groupBox->setLayout(groupLayout);
        auto parameters = module.value(ReportDataName::ekey(ReportDataName::parameters)).toArray();
        for (int j = 0; j < parameters.size(); ++j) {
            auto parameter = parameters.at(j).toObject();
            auto filter = new DatanFilterWidgetNs::NicasFilter(parameter, this);
            groupLayout->addWidget(filter);
            // map.insert(Type(parameter.value(ReportDataName::ekey(ReportDataName::type)).toInt()), filter);
            m_filters.append(filter);
        }
        groupLayout->addStretch();
        widget->addWidget(groupBox);
    }
    rLayout->addStretch();
    connect(buttonGroup, &QButtonGroup::idClicked, widget, &QStackedWidget::setCurrentIndex);

    init();
}

QVector<DatanFilterWidgetNs::NicasFilter *> NicasDataFilterWidget::filters()
{
    return m_filters;
}

void NicasDataFilterWidget::init()
{
    foreach (auto parameter, m_filters) {
        parameter->init();
    }
    buttonGroup->button(0)->setChecked(true);
    emit buttonGroup->idClicked(0);
}
