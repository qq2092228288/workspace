#include "selectitemdialog.h"
#include "datamanagement.h"

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
    emit currentText(comboBox->currentText());
    this->close();
}

void SelectItemDialog::showEvent(QShowEvent *event)
{
    comboBox->clear();
    auto regulator = DataManagement::getInstance().getRegulator();
    QStringList list = regulator->getAllNames();
    if (m_trendChart) {
        list.removeOne("SBP/DBP");
        list.removeOne("MAP");
        list.removeOne("DO2");
    }
    foreach (const QString &name, list) {
        if(-1 == regulator->getSaveNames(m_trendChart).indexOf(name)) {
            comboBox->addItem(name);
        }
    }
    event->accept();
}
