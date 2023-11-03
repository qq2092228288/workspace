#include "logindialog.h"
#include "ui/logindialogui.h"
#include <QMessageBox>
#include <QTimer>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QTextCodec>
#include "mqttclient.h"
#include "singleton.h"

LoginDialog::LoginDialog(QWidget *parent)
    : QDialog{parent},
      ui{new Ui::LoginDialog},
      m_fileName{QCoreApplication::applicationDirPath() + "/userinfo.ini"}
{
    ui->setupUi(this);
    auto instance = MqttClient::getInstance();

    connect(instance, &MqttClient::stateChanged, this, [=](QMqttClient::ClientState state) {
        if (QMqttClient::Disconnected == state) {
            QMessageBox::warning(this, QString("警告"), QString("请检查当前网络是否可用！"));
            exit(0);
        }
        else if (QMqttClient::Connected == state) {
            ui->loginButton->setEnabled(true);
        }
    });
    connect(instance, &MqttClient::newVerion, this, &LoginDialog::newVersion);
    connect(instance, &MqttClient::loginStatus, this, &LoginDialog::loginStatus, Qt::QueuedConnection);
    connect(ui->loginButton, &QPushButton::clicked, this, &LoginDialog::loginButtonClicked);

    ui->loginButton->setEnabled(false);
    instance->connectToHost();
    readInfo();
    if (!ui->usernameLineEdit->text().isEmpty()) {
        ui->rememberPasswordCheckBox->setChecked(true);
    }
}

LoginDialog::~LoginDialog()
{
    delete ui;
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
    MqttClient::getInstance()->login(ui->usernameLineEdit->text(), ui->passwordLineEdit->text());
}

void LoginDialog::loginStatus(const QJsonObject &object)
{
    switch (Singleton::enumKeyToValue<UserStatus>(object.value(Singleton::enumName<UserStatus>()).toString())) {
    case UserStatus::passwordCorrect:
        MqttClient::getInstance()->getReports();
        emit successful();
        close();
        if (!ui->rememberPasswordCheckBox->isChecked()) {
            ui->usernameLineEdit->clear();
            ui->passwordLineEdit->clear();
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
