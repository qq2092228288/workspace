#ifndef LOGINDIALOGUI_H
#define LOGINDIALOGUI_H

#include <QDialog>
#include <QCoreApplication>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QCheckBox>
#include <QAction>
#include <QRegExpValidator>
#include <QFile>

QT_BEGIN_NAMESPACE

class LoginDialogUi : public QDialog
{
public:
    QVBoxLayout *mainLayout;
    QLabel *logoLabel;
    QLineEdit *usernameLineEdit;
    QAction *usernameAction;
    QLineEdit *passwordLineEdit;
    QAction *passwordAction;
    QCheckBox *rememberPasswordCheckBox;
    QPushButton *loginButton;
    void setupUi(QDialog *Dialog)
    {
        if (Dialog->objectName().isEmpty())
            Dialog->setObjectName(QString::fromUtf8("Dialog"));
        Dialog->setFixedSize(420, 330);
        Dialog->setWindowFlag(Qt::WindowContextHelpButtonHint, false);

        QFile qssFile(":/qss/logindialogui.qss");
        if (qssFile.open(QIODevice::ReadOnly))
            Dialog->setStyleSheet(qssFile.readAll());

        mainLayout = new QVBoxLayout(Dialog);
        mainLayout->setObjectName(QString::fromUtf8("mainLayout"));

        logoLabel = new QLabel(Dialog);
        logoLabel->setObjectName(QString::fromUtf8("logoLabel"));

        mainLayout->addWidget(logoLabel, 0, Qt::AlignCenter);

        usernameLineEdit = new QLineEdit(Dialog);
        usernameLineEdit->setObjectName(QString::fromUtf8("usernameLineEdit"));
        usernameLineEdit->setFixedWidth(250);
        usernameLineEdit->setValidator(new QRegExpValidator(QRegExp("[a-zA-Z0-9]{32}"), Dialog));

        mainLayout->addWidget(usernameLineEdit, 0, Qt::AlignCenter);

        usernameAction = new QAction(usernameLineEdit);
        usernameAction->setObjectName(QString::fromUtf8("usernameAction"));

        usernameLineEdit->addAction(usernameAction, QLineEdit::LeadingPosition);

        passwordLineEdit = new QLineEdit(Dialog);
        passwordLineEdit->setObjectName(QString::fromUtf8("passwordLineEdit"));
        passwordLineEdit->setFixedWidth(250);
        passwordLineEdit->setEchoMode(QLineEdit::Password);

        mainLayout->addWidget(passwordLineEdit, 0, Qt::AlignCenter);

        passwordAction = new QAction(passwordLineEdit);
        passwordAction->setObjectName(QString::fromUtf8("passwordAction"));

        passwordLineEdit->addAction(passwordAction, QLineEdit::LeadingPosition);

        rememberPasswordCheckBox = new QCheckBox(Dialog);
        rememberPasswordCheckBox->setObjectName(QString::fromUtf8("rememberPasswordCheckBox"));
        rememberPasswordCheckBox->setFixedWidth(140);

        mainLayout->addWidget(rememberPasswordCheckBox, 0, Qt::AlignRight);

        loginButton = new QPushButton(Dialog);
        loginButton->setObjectName(QString::fromUtf8("loginButton"));
        loginButton->setFixedWidth(250);

        mainLayout->addWidget(loginButton, 0, Qt::AlignCenter);

        mainLayout->addStretch();

        retranslateUi(Dialog);
    }
    void retranslateUi(QDialog *Dialog)
    {
        Dialog->setWindowTitle(QCoreApplication::applicationName() + QString::fromUtf8(" - 登录"));
        logoLabel->setPixmap(QPixmap(":/images/logo.png").scaled(150, 150));
        usernameLineEdit->setPlaceholderText(QString::fromUtf8("账号"));
        usernameAction->setIcon(QIcon(":/images/username.svg"));
        passwordLineEdit->setPlaceholderText(QString::fromUtf8("密码"));
        passwordAction->setIcon(QIcon(":/images/password.svg"));
        rememberPasswordCheckBox->setText(QString::fromUtf8("记住密码"));
        loginButton->setText(QString::fromUtf8("登录"));
    }
};

namespace Ui {
    class LoginDialog : public LoginDialogUi {};
}

QT_END_NAMESPACE

#endif // LOGINDIALOGUI_H
