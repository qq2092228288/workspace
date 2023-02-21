#ifndef INFOEDITDIALOG_H
#define INFOEDITDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QGridLayout>
#include <QPushButton>
#include <QMessageBox>
#include <QFile>
#include <QValidator>
#include <QKeyEvent>
#include <QtMath>

#include "personalinfodialog.h"

struct BodyValue
{
    QString id;
    QString name;
    int sex = 0;
    int age = 0;
    int height = 0;
    int weight = 0;
    int SBP = 0;
    int DBP = 0;
    //default
    int CVP = 4;
    int LAP = 9;
    //体表面积
    double BSA() const;

    double VEPT() const;

    int MAP() const;
};

class InfoEditDialog : public QDialog
{
    Q_OBJECT
public:
    explicit InfoEditDialog(QWidget *parent = nullptr);
    void keyPressEvent(QKeyEvent *event);
    void clear();
signals:
    //性别，年龄，身高，体重
    void bodyValue(const BodyValue &);
public slots:
    void clearSlot();
    void confirmSlot();
private slots:
    void stopCheck();
    void importPatientInfo(QString id, QString name, int sex, int age, int height, int weight);
private:
    QLabel *nameLabel;
    QLabel *numLabel;
    QLabel *sexLabel;
    QLabel *ageLabel;
    QLabel *heightLabel;
    QLabel *weightLabel;
    QLineEdit *nameLineEdit;
    QLineEdit *numLineEdit;
    QComboBox *sexComboBox;
    QLineEdit *ageLineEdit;
    QLineEdit *heightLineEdit;
    QLineEdit *weightLineEdit;
    QPushButton *importBtn;
    QPushButton *clearBtn;
    QPushButton *confirmBtn;
    PersonalInfoDialog *pInfoDialog;
};

#endif // INFOEDITDIALOG_H
