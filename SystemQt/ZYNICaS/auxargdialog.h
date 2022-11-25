#ifndef AUXARGDIALOG_H
#define AUXARGDIALOG_H

#include <QDialog>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QGridLayout>
#include <QMessageBox>
#include <QFile>
#include <QIntValidator>
#include <QKeyEvent>

class AuxArgDialog : public QDialog
{
    Q_OBJECT
public:
    explicit AuxArgDialog(QWidget *parent = nullptr);
    void keyPressEvent(QKeyEvent *event);
    void closeEvent(QCloseEvent *event);
    int getCvp();
    int getLap();
signals:
    //CVP,LAP
    void value(int, int);
private:
    QLabel *cvpLabel;               //CVP
    QLabel *lapLabel;               //LAP
    QLineEdit *cvpLineEdit;
    QLineEdit *lapLineEdit;
    QPushButton *confirmBtn;
};

#endif // AUXARGDIALOG_H
