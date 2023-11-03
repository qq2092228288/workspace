#include "mainwidget.h"
#include "ui/mainwidgetui.h"
#include "reportdialog.h"
#include "reportconfigdialog.h"

MainWidget::MainWidget(QWidget *parent)
    : QWidget{parent},
      ui{new Ui::MainWidget}
{
    ui->setupUi(this);

    reportDialog = new ReportDialog(this);
    configDialog = new ReportConfigDialog(this);

    connect(ui->showReportButton, &QPushButton::clicked, this, &MainWidget::showReportButtonClicked);
    connect(ui->configButton, &QPushButton::clicked, configDialog, &ReportConfigDialog::exec);
    connect(ui->exitButton, &QPushButton::clicked, this, &MainWidget::close);
}

MainWidget::~MainWidget()
{
    delete ui;
}

void MainWidget::showReportButtonClicked()
{
    reportDialog->setConfig(configDialog->config());
    reportDialog->exec();
}
