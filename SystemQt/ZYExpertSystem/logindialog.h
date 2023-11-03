#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QJsonObject>
#include "updateappdialog.h"

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT
public:
    explicit LoginDialog(QWidget *parent = nullptr);
    virtual ~LoginDialog();
signals:
    void successful();
private slots:
    void newVersion(const QJsonObject &object);
    void loginButtonClicked();
    void loginStatus(const QJsonObject &object);
private:
    void readInfo();
    void writeInfo();
private:
    Ui::LoginDialog *ui;
    QString m_fileName;
};

#endif // LOGINDIALOG_H
