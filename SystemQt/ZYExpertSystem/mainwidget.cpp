#include "mainwidget.h"
#include "ui/mainwidgetui.h"
#include "reportdialog.h"
#include "systemconfigdialog.h"
#include "tcpclientsocket.h"

MainWidget::MainWidget(QWidget *parent)
    : QWidget{parent},
      ui{new Ui::MainWidget}
{
    ui->setupUi(this);

    reportDialog = new ReportDialog(this);
    configDialog = new SystemConfigDialog(this);

    connect(ui->showReportButton, &QPushButton::clicked, this, &MainWidget::showReportButtonClicked);
    connect(ui->systemConfigButton, &QPushButton::clicked, configDialog, &SystemConfigDialog::exec);
    connect(ui->exitButton, &QPushButton::clicked, this, &MainWidget::close);
}

MainWidget::~MainWidget()
{
    delete ui;
}

void MainWidget::requestReports()
{
    auto instance = TcpClientSocket::getInstance();
    if (configDialog->getLatest()) {
        instance->getNewReports();
    }
    else {
        instance->getReports();
    }
}

void MainWidget::showReportButtonClicked()
{
    reportDialog->setConfig(configDialog->config());
    reportDialog->exec();
}
