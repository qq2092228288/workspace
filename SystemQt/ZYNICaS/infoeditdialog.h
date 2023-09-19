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
#include <QDoubleValidator>
#include <QGroupBox>

#include "personalinfodialog.h"

struct BodyValue
{
    QString id;
    QString name;
    int sex = 2;
    int age = 0;
    int height = 0;
    int weight = 0;
    float hb = 0;
    int SBP = 0;
    int DBP = 0;
    //default
    int CVP = 4;
    int LAP = 9;
    // 问诊
    int fhh = 0;
    int edh = 0;
    int ltsh = 0;
    int lthms = 0;
    int ptm = 0;
    int al = 0;
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
    void updateUi();
public slots:
    void clearSlot();
    void confirmSlot();
    void showHb(bool show);
    void showInquiry(bool show);
private slots:
    void stopCheck();
    void importPatientInfo(QString id, QString name, int sex, int age, int height, int weight,
                           int fhh, int edh, int ltsh, int lthms, int ptm, int al);
private:
    QGroupBox *baseInfoGroupBox;
    QLabel *nameLabel;
    QLabel *numLabel;
    QLabel *sexLabel;
    QLabel *ageLabel;
    QLabel *heightLabel;
    QLabel *weightLabel;
    QLabel *hbLabel;
    QLineEdit *nameLineEdit;
    QLineEdit *numLineEdit;
    QComboBox *sexComboBox;
    QLineEdit *ageLineEdit;
    QLineEdit *heightLineEdit;
    QLineEdit *weightLineEdit;
    QLineEdit *hbLineEdit;
    QGroupBox *inquiryGroupBox;
    QLabel *fhhLabel;
    QLabel *edhLabel;
    QLabel *ltshLabel;
    QLabel *lthmsLabel;
    QLabel *ptmLabel;
    QLabel *alLabel;
    QComboBox *fhhComboxBox;
    QComboBox *edhComboxBox;
    QComboBox *ltshComboxBox;
    QComboBox *lthmsComboxBox;
    QComboBox *ptmComboxBox;
    QComboBox *alComboxBox;
    QPushButton *importBtn;
    QPushButton *clearBtn;
    QPushButton *confirmBtn;
    PersonalInfoDialog *pInfoDialog;
};

#endif // INFOEDITDIALOG_H
