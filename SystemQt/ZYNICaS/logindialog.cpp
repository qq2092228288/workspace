#include "logindialog.h"
#include "datamanagement.h"

LoginDialog::LoginDialog(QWidget *parent)
    : QDialog{parent},
      exitApp{true}
{
    setWindowTitle(title(false));
    auto &instance = DataManagement::getInstance();
    setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
//    setMinimumSize(200*instance.wZoom(), 200*instance.hZoom());
//    setMinimumWidth(300*instance.wZoom());
    setFixedSize(300*instance.wZoom(), 300*instance.hZoom());
    this->setStyleSheet(instance.dialogQss(1.3));

    logoLabel = new QLabel(this);
    deviceIdEdit = new QLineEdit(this);
    passwordEdit = new QLineEdit(this);
    loginBtn = new QPushButton("登 录", this);
    QGridLayout *gLayout = new QGridLayout(this);

    gLayout->addWidget(logoLabel, 0, 0, Qt::AlignCenter);
    gLayout->addWidget(deviceIdEdit, 1, 0);
    gLayout->addWidget(passwordEdit, 2, 0);
    gLayout->addWidget(loginBtn, 3, 0, Qt::AlignCenter);

    logoLabel->setPixmap(QPixmap(":/images/logo.png")
                         .scaled(100, 100, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    deviceIdEdit->setPlaceholderText(tr("设备编号"));
    deviceIdEdit->setValidator(new QRegExpValidator(QRegExp("[a-zA-Z0-9]{1,32}$")));
    deviceIdEdit->setAlignment(Qt::AlignCenter);
    passwordEdit->setPlaceholderText(tr("密码"));
    passwordEdit->setAlignment(Qt::AlignCenter);
    passwordEdit->setEchoMode(QLineEdit::Password);
    loginBtn->setEnabled(false);

    connect(loginBtn, &QPushButton::clicked, this, &LoginDialog::loginSlot);
}

QString LoginDialog::deviceId() const
{
    return deviceIdEdit->text();
}

QString LoginDialog::password() const
{
    return passwordEdit->text();
}

bool LoginDialog::exitApplication() const
{
    return exitApp;
}

void LoginDialog::loginSucceeded(bool succeeded)
{
    if (succeeded) {
        exitApp = false;
        if (!this->isHidden()) {
            this->close();
        }
    }
    else {
        QMessageBox::warning(this, tr("警告"), tr("获取服务器资源出错！"));
    }
}

void LoginDialog::serverConnected()
{
    loginBtn->setEnabled(true);
    setWindowTitle(title(true));
}

void LoginDialog::loginSlot()
{
    if (deviceIdEdit->text().isEmpty()) {
        QMessageBox::warning(this, tr("警告"), tr("请输入设备编号！"));
        return;
    }
    if (passwordEdit->text().isEmpty()) {
        QMessageBox::warning(this, tr("警告"), tr("请输入密码！"));
        return;
    }
    emit deviceIdAndPassword(deviceId(), password());
}

void LoginDialog::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return) {
        loginSlot();
    }
    e->accept();
}

QString LoginDialog::title(bool connected)
{
    QString title("无创血流动力力学检测系统");
    if (!connected) {
        return (title + QString("(服务器未连接)"));
    }
    return title;
}
