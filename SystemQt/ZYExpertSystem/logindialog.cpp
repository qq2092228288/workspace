#include "logindialog.h"
#include "ui/logindialogui.h"
#include <QMessageBox>
#include <QTimer>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QStandardPaths>
#include <QTextStream>
#include <QTextCodec>
#include "updateappdialog.h"
#include "singleton.h"
#include "tcpclientsocket.h"

LoginDialog::LoginDialog(QWidget *parent)
    : QDialog{parent}
    , ui{new Ui::LoginDialog}
{
    auto path = QStandardPaths::standardLocations(QStandardPaths::AppDataLocation).at(0);
    if (!QFileInfo::exists(path)) {
        QDir dir(path);
        dir.mkpath(path);
    }
    m_fileName = path + "/userinfo.ini";

    ui->setupUi(this);
    ui->loginButton->setEnabled(false);
    ui->latestReportButton->setChecked(true);
    readInfo();
    if (!ui->usernameLineEdit->text().isEmpty()) {
        ui->rememberPasswordCheckBox->setChecked(true);
    }
    connect(ui->loginButton, &QPushButton::clicked, this, &LoginDialog::loginButtonClicked);
    auto instance = TcpClientSocket::getInstance();
    connect(instance, &TcpClientSocket::stateChanged, this, &LoginDialog::stateChanged);
    connect(instance, &TcpClientSocket::versionInfo, this, &LoginDialog::newVersion);
    connect(instance, &TcpClientSocket::userInfo, this, &LoginDialog::loginStatus);
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::stateChanged(QAbstractSocket::SocketState state)
{
    switch (state) {
    case QAbstractSocket::UnconnectedState:
        ui->loginButton->setEnabled(false);
        QMessageBox::warning(this, QString("警告"), QString("连接服务器失败，请检查当前网络是否可用！"));
        exit(0);
        break;
    case QAbstractSocket::ConnectedState:
    {
        TcpClientSocket::getInstance()->getSoftwareInfo();
        ui->loginButton->setEnabled(true);
    }
        break;
    default:
        break;
    }
}

void LoginDialog::newVersion(const QJsonObject &object)
{
    UpdateAppDialog dialog(object);
    connect(&dialog, &UpdateAppDialog::installNewApp, &dialog, &UpdateAppDialog::close);
    connect(&dialog, &UpdateAppDialog::installNewApp, this, &LoginDialog::close);
    dialog.exec();
}

void LoginDialog::loginButtonClicked()
{
    auto username = ui->usernameLineEdit->text();
    auto password = ui->passwordLineEdit->text();
    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, QString("警告"), QString("账号和密码不能为空！"));
        return;
    }
    TcpClientSocket::getInstance()->login(ui->usernameLineEdit->text(), ui->passwordLineEdit->text());
}

void LoginDialog::loginStatus(const QJsonObject &object)
{
    switch (Singleton::enumKeyToValue<UserStatus>(object.value(Singleton::enumName<UserStatus>()).toString())) {
    case UserStatus::passwordCorrect:
        emit successful();
        close();
        if (!ui->rememberPasswordCheckBox->isChecked()) {
            ui->usernameLineEdit->clear();
            ui->passwordLineEdit->clear();
        }
        if (ui->latestReportButton->isChecked()) {
            TcpClientSocket::getInstance()->getNewReports();
        }
        else {
            TcpClientSocket::getInstance()->getReports();
        }
        writeInfo();
        break;
    case UserStatus::passwordError:
        QMessageBox::warning(this, QString("警告"), QString("密码错误！"));
        break;
    default:
        break;
    }
}

void LoginDialog::readInfo()
{
    QFile file(m_fileName);
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream out(&file);
        out.setCodec(QTextCodec::codecForName("utf-8"));
        QRegExp nameExp("(.*)(?:=)");
        QRegExp valueExp("(?:\")(.*)(?:\")");
        while (!out.atEnd()) {
            QString strLine = out.readLine();
            if (strLine.indexOf(nameExp) >= 0 && strLine.indexOf(valueExp) >= 0) {
                QString name = nameExp.cap(1);
                QString value = valueExp.cap(1);
                if (name == "username") {
                    ui->usernameLineEdit->setText(value);
                }
                else if (name == "password") {
                    ui->passwordLineEdit->setText(value);
                }
            }
        }
        file.close();
    }
}

void LoginDialog::writeInfo()
{
    QFile file(m_fileName);
    if(file.open(QFile::WriteOnly)){
        QTextStream in(&file);
        in.setCodec(QTextCodec::codecForName("utf-8"));
        in<<QString("username=\"%1\"\n").arg(ui->usernameLineEdit->text());
        in<<QString("password=\"%1\"\n").arg(ui->passwordLineEdit->text());
        file.close();
    }
}
