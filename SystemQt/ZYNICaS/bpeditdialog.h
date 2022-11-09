#ifndef BPEDITDIALOG_H
#define BPEDITDIALOG_H

#include <QDialog>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QGridLayout>
#include <QMessageBox>
#include <QFile>
#include <QIntValidator>
#include <QKeyEvent>

class BPEditDialog : public QDialog
{
    Q_OBJECT
public:
    explicit BPEditDialog(QWidget *parent = nullptr);
    void keyPressEvent(QKeyEvent *event);
signals:
    //SBP,DBP
    void value(QString, QString);
public slots:
    void clear();
    void confirmSlot();
private:
    QLabel *sbpLabel;               //收缩压
    QLabel *dbpLabel;               //舒张压
    QLineEdit *sbpLineEdit;
    QLineEdit *dbpLineEdit;
    QPushButton *confirmBtn;
};

#endif // BPEDITDIALOG_H
