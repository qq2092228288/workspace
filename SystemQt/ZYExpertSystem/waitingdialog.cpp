#include "waitingdialog.h"
#include "ui/waitingdialogui.h"

WaitingDialog::WaitingDialog(QWidget *parent)
    : QDialog{parent}
    , ui{new Ui::WaitingDialog}
{
    ui->setupUi(this);
}

WaitingDialog::~WaitingDialog()
{
    delete ui;
}

void WaitingDialog::start(const int &total)
{
    if (total > 0) {
        m_total = total;
        m_current = 0;
        ui->movie->start();
        if (isHidden()) {
            show();
        }
    }
}

void WaitingDialog::tryStop()
{
    ui->textLabel->setText(QString("正在下载：%1%").arg(100 * (++m_current) / m_total, 3, 10, QLatin1Char(' ')));
    if (m_current == m_total) {
        m_total = 0;
        m_current = 0;
        ui->movie->stop();
        close();
    }
}
