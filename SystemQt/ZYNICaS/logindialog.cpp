#include "logindialog.h"
#include "datamanagement.h"

LoginDialog::LoginDialog(QWidget *parent)
    : QDialog{parent},
      exitApp{true}
{
    setWindowTitle(title(false));
    auto &instance = DataManagement::getInstance();
    setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
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

    readDeviceInfo();
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

void LoginDialog::serverMessage(const QString &message)
{
    QMessageBox::warning(this, tr("警告！"), message);
}

void LoginDialog::sltMessageReceived(const QString &message)
{
    QMessageBox::warning(this, tr("提示！"), message);
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
    writeDeviceInfo();
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

void LoginDialog::readDeviceInfo()
{
    auto &instance = DataManagement::getInstance();
    QFile file(instance.getPaths().userAccountInfo());
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
                if (name == "deviceId") {
                    deviceIdEdit->setText(value);
                }
                else if (name == "password") {
//                    passwordEdit->setText(decryption(value));
                    passwordEdit->setText(value);
                }
            }
        }
        file.close();
    }
}

void LoginDialog::writeDeviceInfo()
{
    QFile file(DataManagement::getInstance().getPaths().userAccountInfo());
    if(file.open(QFile::WriteOnly)){
        QTextStream in(&file);
        in.setCodec(QTextCodec::codecForName("utf-8"));
        in<<QString("deviceId=\"%1\"\n").arg(deviceIdEdit->text());
        in<<QString("password=\"%1\"\n").arg(passwordEdit->text());
        file.close();
    }
}
#if 0
QString LoginDialog::encryption(const QString &password)
{
    QByteArray macArray = DataManagement::getInstance().getMac().replace(":", "").toLatin1();
    QByteArray pwArray = password.toLatin1();
    QString ep;
    auto macCount = macArray.count();
    auto pwCount = pwArray.count();
    for (int i = 0; i < pwCount; ++i) {
        ep.append(char(pwArray[i] ^ macArray[i%macCount]));
    }
    return ep;
}

QString LoginDialog::decryption(const QString &epstring)
{
    QByteArray macArray = DataManagement::getInstance().getMac().replace(":", "").toLatin1();
    QByteArray epArray = epstring.toLatin1();
    QString password;
    auto macCount = macArray.count();
    for (int i = 0; i < epArray.count(); ++i) {
        password.append(char(epArray[i] ^ macArray[i%macCount]));
    }
    return password;
}
#endif
