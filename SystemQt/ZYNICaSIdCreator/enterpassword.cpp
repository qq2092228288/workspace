#include "enterpassword.h"


EnterPassword::EnterPassword(QWidget *parent)
    : QDialog{parent}
{
    password = "zeyao2022";
    setWindowTitle(tr("输入密码"));
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    passwordLineEdit = new QLineEdit(this);
    confirmBtn = new QPushButton(tr("确定"));

    mainLayout->addWidget(passwordLineEdit);
    mainLayout->addWidget(confirmBtn);

    passwordLineEdit->setEchoMode(QLineEdit::Password);

    connect(confirmBtn, &QPushButton::clicked, this, &EnterPassword::confirmSlot);
}

void EnterPassword::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return){
        confirmSlot();
    }else {
        event->accept();
    }
}

void EnterPassword::confirmSlot()
{
    if (passwordLineEdit->text() == password) {
        emit passwordTrue(true);
        this->close();
    }
    else {
        QMessageBox::warning(this, tr("错误"), tr("密码不正确！！！"));
    }
}
