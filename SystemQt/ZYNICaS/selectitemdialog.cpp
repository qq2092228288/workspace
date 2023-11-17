#include "selectitemdialog.h"
#include "datamanagement.h"
#include "reportset.h"

SelectItemDialog::SelectItemDialog(bool trendChart, QWidget *parent)
    : QDialog{parent},m_trendChart{trendChart}
{
    auto &instance = DataManagement::getInstance();

    setMinimumSize(240*instance.wZoom(), 120*instance.hZoom());
    setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
    //样式表
    setStyleSheet(instance.dialogQss(1.5));
    comboBox = new QComboBox(this);
    confirmBtn = new QPushButton(tr("确定"), this);
    comboBox->setFixedWidth(120*instance.wZoom());
    QHBoxLayout *layout = new QHBoxLayout(this);
    setWindowTitle(tr("选择参数"));
    setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
    setLayout(layout);
    layout->addWidget(comboBox);
    layout->addStretch();
    layout->addWidget(confirmBtn);

    connect(confirmBtn, &QPushButton::clicked, this, &SelectItemDialog::confirmSlot);
}

void SelectItemDialog::confirmSlot()
{
    auto ctrls = DataManagement::getInstance().getRegulator()->getAllCustomCtrls();
    foreach (auto ctrl, ctrls) {
        if (comboBox->currentText() == ctrl->getName()) {
            emit currentType(ctrl->getType());
            break;
        }
    }
    this->close();
}

void SelectItemDialog::showEvent(QShowEvent *event)
{
    comboBox->clear();
    auto regulator = DataManagement::getInstance().getRegulator();
    auto types = regulator->getAllTypes();
    if (m_trendChart) {
        for (auto it = types.begin(); it != types.end(); ++it) {
            if (Type::SBP == *it || Type::MAP == *it || Type::DO2 == *it) {
                it = types.erase(it);
            }
        }
    }
    foreach (auto type, types) {
        auto stypes = regulator->getSaveTypes(m_trendChart);
        if (stypes.cend() == std::find(stypes.cbegin(), stypes.cend(), type)) {
            auto ctrl = regulator->getCustomCtrl(type);
            if (ctrl != nullptr) {
                comboBox->addItem(ctrl->getName());
            }
        }
    }
    event->accept();
}
