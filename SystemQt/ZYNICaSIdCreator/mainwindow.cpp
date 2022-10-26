#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "idcheck.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle(tr("使用码生成器"));
    setFixedSize(500,200);
    ui->countLineEdit->setFixedWidth(70);
    ui->macLineEdit->setFixedWidth(130);


    dialog = new EnterPassword(this);

    ui->countLineEdit->setValidator(new QRegExpValidator(QRegExp("^([1-4]{0,1}[0-9]{0,4}|(50000))$"),this));
    ui->macLineEdit->setValidator(new QRegExpValidator(QRegExp("^[0-9a-zA-Z:-]{17}$"),this));
    ui->countLineEdit->setPlaceholderText("0~50000");
    ui->macLineEdit->setPlaceholderText("00:00:00:00:00:00");
    ui->countLineEdit->setAlignment(Qt::AlignCenter);
    ui->macLineEdit->setAlignment(Qt::AlignCenter);
    ui->confirmBtn->setEnabled(false);

    connect(ui->clearBtn, &QPushButton::clicked, this, &MainWindow::clearSlot);
    connect(ui->confirmBtn, &QPushButton::clicked, this, &MainWindow::confirmSlot);
    connect(ui->copyBtn, &QPushButton::clicked, this, &MainWindow::copySlot);
    connect(ui->actionpassword, &QAction::triggered, dialog, &EnterPassword::exec);
    connect(dialog, &EnterPassword::passwordTrue, ui->confirmBtn, &QPushButton::setEnabled);
    connect(dialog, &EnterPassword::passwordTrue, ui->actionpassword, [=](){
        ui->actionpassword->setEnabled(false);
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::copySlot()
{
    QApplication::clipboard()->setText(ui->useCodeLineEdit->text());
}

void MainWindow::confirmSlot()
{
    if (ui->countLineEdit->text().isEmpty()) {
        QMessageBox::information(this,tr("提示"),tr("请输入数量！"));
        return;
    }
    if (ui->macLineEdit->text().isEmpty()) {
        QMessageBox::information(this,tr("提示"),tr("请输入MAC地址！"));
        return;
    }
    if (ui->macLineEdit->text().indexOf(QRegExp("^[0-9a-zA-Z:-]{17}$")) == -1) {
        QMessageBox::information(this,tr("提示"),tr("MAC地址格式不正确！"));
        return;
    }
    ui->useCodeLineEdit->setText(IdCheck::getCode(ui->countLineEdit->text().toUShort(),
                                                  ui->macLineEdit->text()).toHex());
}

void MainWindow::clearSlot()
{
    ui->countLineEdit->clear();
    ui->macLineEdit->clear();
    ui->useCodeLineEdit->clear();
}

