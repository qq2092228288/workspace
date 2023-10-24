#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>

#include <iostream>
#include "zeyaotebcosdk.h"
#include <threadservice.h>
#include <QJsonDocument>
#include <QJsonArray>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    tebco = new ZeYaoTebcoSDK();
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::timeout);
    timer->start(1000);
    auto coms = QString::fromStdString(tebco->availablePorts());
    auto array = QJsonDocument::fromJson(coms.toUtf8()).array();
    foreach (auto com, array) {
        ui->comboBox->addItem(com.toString());
    }

//    ThreadService::getInstance()->objectMoveToThread(tebco);
//    connect(tebco, &ZeYaoTebcoSDK::sendData, this, &MainWindow::appendString);

}

MainWindow::~MainWindow()
{
    delete ui;
    delete tebco;
}


void MainWindow::on_pushButtonStart_clicked()
{
    auto boolean = tebco->start("test", "1234", ui->lineEditGender->text().toInt(),
                      ui->lineEditAge->text().toInt(),
                      ui->lineEditHeight->text().toInt(),
                      ui->lineEditWeight->text().toInt());
    ui->textBrowser->append(QString("启动检测：%1").arg(boolean));
}

void MainWindow::on_pushButtonEnd_clicked()
{
    auto boolean = tebco->end();
    ui->textBrowser->append(QString("结束检测：%1").arg(boolean));
}

void MainWindow::appendString(std::string str)
{
    ui->textBrowser->append(QString::fromStdString(str));
}

void MainWindow::timeout()
{
    auto str = tebco->readAll();
    if (!str.empty()) {
        ui->textBrowser->append(QString::fromStdString(str));
    }
}


void MainWindow::on_pushButtonOpen_clicked()
{
    auto info = QString::fromStdString(tebco->deviceInfo());
    if (!info.isEmpty()) {
        ui->textBrowser->append(info);
    }
    auto boolean = tebco->open(ui->comboBox->currentText().toStdString());
    ui->textBrowser->append(QString("打开串口：%1").arg(boolean));
}


void MainWindow::on_pushButtonClose_clicked()
{
    auto boolean = tebco->close();
    ui->textBrowser->append(QString("关闭串口：%1").arg(boolean));
}


void MainWindow::on_pushButtonLogin_clicked()
{
    tebco->login(ui->lineEditId->text().toStdString(), ui->lineEditPassword->text().toStdString());
}


void MainWindow::on_pushButtonBp_clicked()
{
    auto boolean = tebco->appendBpAndPostion(ui->lineEditSbp->text().toInt(), ui->lineEditDbp->text().toInt(),
                              ui->lineEditPosition->text().toInt());
    ui->textBrowser->append(QString("追加血压和体位：%1").arg(boolean));
}

