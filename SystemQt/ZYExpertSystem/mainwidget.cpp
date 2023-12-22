#include "mainwidget.h"
#include "ui/mainwidgetui.h"
#include "reportdialog.h"
#include "datawidget.h"
#include "systemconfigdialog.h"
#include "tcpclientsocket.h"

MainWidget::MainWidget(QWidget *parent)
    : QWidget{parent},
      ui{new Ui::MainWidget}
{
    ui->setupUi(this);

    reportDialog = new ReportDialog(this);
    dataWidget = new DataWidget;
    configDialog = new SystemConfigDialog(this);

    connect(ui->showReportButton, &QPushButton::clicked, this, &MainWidget::showReportButtonClicked);
    connect(ui->dataFilteringButton, &QPushButton::clicked, this, &MainWidget::hide);
    connect(ui->dataFilteringButton, &QPushButton::clicked, dataWidget, &DataWidget::showMaximized);
    connect(dataWidget, &DataWidget::hidden, this, &MainWidget::show);
    connect(ui->systemConfigButton, &QPushButton::clicked, configDialog, &SystemConfigDialog::exec);
    connect(ui->exitButton, &QPushButton::clicked, this, &MainWidget::close);
}

MainWidget::~MainWidget()
{
    delete ui;
    delete dataWidget;
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
