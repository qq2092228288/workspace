#ifndef ENTERPASSWORD_H
#define ENTERPASSWORD_H

#include <QDialog>
#include <QPushButton>
#include <QLineEdit>
#include <QMessageBox>
#include <QHBoxLayout>
#include <QKeyEvent>

class EnterPassword : public QDialog
{
    Q_OBJECT
public:
    explicit EnterPassword(QWidget *parent = nullptr);
    void keyPressEvent(QKeyEvent *event);
private slots:
    void confirmSlot();
signals:
    void passwordTrue(bool);
private:
    QLineEdit *passwordLineEdit;
    QPushButton *confirmBtn;
    QString password;
};

#endif // ENTERPASSWORD_H
