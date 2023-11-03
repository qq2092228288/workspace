#include "scopecalendardialog.h"
#include <QMessageBox>

ScopeCalendarDialog::ScopeCalendarDialog(QWidget *parent)
    : QDialog{parent}
{
    setWindowFlag(Qt::WindowContextHelpButtonHint, false);
    setWindowTitle("选择日期");
    resize(600, 300);
    label = new QLabel(this);
    startWidget = new QCalendarWidget(this);
    endWidget = new QCalendarWidget(this);
    confirmButton = new QPushButton("确定", this);
    cancelButton = new QPushButton("取消", this);

    auto vLayout = new QVBoxLayout(this);
    vLayout->addWidget(label, 0, Qt::AlignCenter);

    auto hLayout = new QHBoxLayout;
    vLayout->addLayout(hLayout);
    hLayout->addWidget(startWidget);
    hLayout->addWidget(endWidget);

    auto btnLayout = new QHBoxLayout;
    vLayout->addLayout(btnLayout);
    btnLayout->addStretch();
    btnLayout->addWidget(confirmButton);
    btnLayout->addWidget(cancelButton);

    connect(startWidget, &QCalendarWidget::clicked, this, &ScopeCalendarDialog::setLabel);
    connect(endWidget, &QCalendarWidget::clicked, this, &ScopeCalendarDialog::setLabel);
    connect(confirmButton, &QPushButton::clicked, this, &ScopeCalendarDialog::confirmSlot);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::close);
    connect(startWidget, &QCalendarWidget::currentPageChanged, this, [=](int year, int month){
        startWidget->setSelectedDate(QDate(year, month, 1));
        setLabel();
    });
    connect(endWidget, &QCalendarWidget::currentPageChanged, this, [=](int year, int month){
        endWidget->setSelectedDate(QDate(year, month, 1));
        setLabel();
    });
    setLabel();
}

void ScopeCalendarDialog::setLabel()
{
    label->setText(QString("%1  至  %2").arg(startWidget->selectedDate().toString("yyyy年MM月dd日"),
                                            endWidget->selectedDate().toString("yyyy年MM月dd日")));
}

void ScopeCalendarDialog::confirmSlot()
{
    auto start = startWidget->selectedDate().startOfDay();
    auto end = endWidget->selectedDate().endOfDay();
    if (start > end) {
        QMessageBox::warning(this, "警告", "起始时间应小于结束时间！");
    }
    else {
        emit scopeTime(start.toMSecsSinceEpoch(), end.toMSecsSinceEpoch());
        close();
    }
}
