#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QObject>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QGridLayout>
#include <QRegExpValidator>
#include <QMessageBox>
#include <QSharedPointer>
#include <QKeyEvent>
#include <QFile>
#include <QTextStream>

class LoginDialog : public QDialog
{
    Q_OBJECT
public:
    LoginDialog(QWidget *parent = nullptr);
    QString deviceId() const;
    QString password() const;
    bool exitApplication() const;
public slots:
    void loginSucceeded(bool succeeded);
    void serverConnected();
private slots:
    void loginSlot();
signals:
    void deviceIdAndPassword(QString deviceId, QString password);
protected:
    virtual void keyPressEvent(QKeyEvent *e);
private:
    QString title(bool connected);
    void readDeviceInfo();
    void writeDeviceInfo();
    QString encryption(const QString &password);
    QString decryption(const QString &epstring);
private:
    bool exitApp;
    QLabel *logoLabel;
    QLineEdit *deviceIdEdit;
    QLineEdit *passwordEdit;
    QPushButton *loginBtn;
};
typedef QSharedPointer<LoginDialog> LoginDialog_PTR;

#endif // LOGINDIALOG_H
