#ifndef CUSTOMCTRL_H
#define CUSTOMCTRL_H

#include <QWidget>
#include <QLabel>
#include <QGridLayout>
#include <QFile>
#include <QFrame>
#include <QSlider>
#include <QDialog>
#include <QComboBox>
#include <QPushButton>
#include <QGroupBox>
#include <QLineEdit>
#include <QFile>
#include <QTextStream>
#include <QDebug>

enum Type {
    HR = 0, VET, PEP, TFC, EPCI, ISI, EF, SI, CI, RR, BEEP, Reserved = 15,
    CO, SV, HRV, EDI, Vol, SVR, SSVR, SSVRI, SVRI, Vas, LVET, LSW, LSWI,
    LCW, LCWI, STR, Ino, SBP, DBP, MAP
};
QString typeName(const uchar &type);
QString typeName(const Type &type);

// 范围，记录值，当前值，名称，单位
typedef struct ArgItems
{
    double minValue = 0;
    double maxValue = 0;
    double recordValue = 0;
    double currentValue = 0;
    QString dataName;
    QString dataUnit;
}AItems;

struct Argument;
class CustomCtrlRegulator;

class CustomCtrl : public QWidget
{
    Q_OBJECT
public:
    explicit CustomCtrl(CustomCtrlRegulator *reg, Argument arg, QWidget *parent = nullptr);
    // 用于舒张压
    virtual void setDoubleDataCtrl(Argument arg);
    // 数据波动
    void startTimer(qreal accuracy);
    void stopTimer();
protected:
    void mouseDoubleClickEvent(QMouseEvent *);
public:
    AItems getArgItems();
    virtual AItems getDbpArgItems();
    double getRecordValue();
    double getCurrentValue();
    double getMinValue() const;
    double getMaxValue() const;
    QString getName() const;
    QString getValueStr() const;
public slots:
    virtual void clear();
    void setValue(const double &value, int digit = 0);
    virtual void setValues(const double &value, const double &value1);
    void valueWarning(bool warning);
    virtual void recordValueSlot();
protected slots:
//    void confirmSlot();
    void timeoutSlot();
signals:
//    void recordValue(qreal);
    void currentValue(qreal);
    // 当前名字，交换显示的名字
    void changeName(const QString &, const QString &);
protected:
    QTimer *timer;
//    QDialog dialog;
//    QComboBox *comboBox;
//    QPushButton *confirmBtn;
    CustomCtrlRegulator *regulator;
    QFrame *frame;
    QVBoxLayout *mainLayout;
    QLabel *mainLabel;      //主标签
    QLabel *secondaryLabel; //副标签
    QLineEdit *valueEdit;   //值
    QLabel *scopeLabel;     //范围
    QLabel *unitLabel;      //单位
    AItems aitems;
    // 精度
    qreal m_accuracy = 1;
};

class BpCustomCtrl : public CustomCtrl
{
    Q_OBJECT
public:
    explicit BpCustomCtrl(CustomCtrlRegulator *reg, Argument arg, QWidget *parent = nullptr);
    void setDoubleDataCtrl(Argument arg);
    AItems getDbpArgItems();
    void setValues(const double &value, const double &value1);
public slots:
    void clear();
    void recordValueSlot();
private:
    AItems dbpaitems;
};

class CustomCtrlRegulator : public QObject
{
    Q_OBJECT
public:
    explicit CustomCtrlRegulator(QObject *parent = nullptr);
    virtual ~CustomCtrlRegulator();
public:
    void addCustomCtrl(CustomCtrl *ctrl);
    void addCustomCtrl(QList<CustomCtrl *> ctrls);
    CustomCtrl *getCustomCtrl(const QString &name);
    QList<CustomCtrl *> getAllCustomCtrls();
    QStringList getSaveNames();
    QStringList getAllNames();
    QStringList getCurrentNames();
    void saveNames();
    QList<CustomCtrl *> getSaveCustomCtrls();
    void changeCurrentNames(const QString &current, const QString &change);
    QList<qreal> getRecordValues();
    QList<qreal> getCurrentValues();
signals:
    void openError(const QString &);
private:
    QList<CustomCtrl *> allCustomCtrls;
    QStringList currentNames;
    QString fileName;

    //心排量
    CustomCtrl *CO_Widget;
    CustomCtrl *CI_Widget;
    CustomCtrl *SV_Widget;
    CustomCtrl *SI_Widget;
//    CustomCtrl *HRV_Widget;
    //前负荷容量
    CustomCtrl *TFC_Widget;
//    CustomCtrl *EDI_Widget;
//    CustomCtrl *Vol_Widget;
    //后负荷容量
//    CustomCtrl *SVR_Widget;
//    CustomCtrl *SSVR_Widget;
//    CustomCtrl *SSVRI_Widget;
//    CustomCtrl *SVRI_Widget;
//    CustomCtrl *Vas_Widget;
    //心肌收缩力动力
    CustomCtrl *EF_Widget;
    CustomCtrl *PEP_Widget;
    CustomCtrl *LVET_Widget;
    CustomCtrl *EPCI_Widget;
    CustomCtrl *ISI_Widget;
//    CustomCtrl *LSW_Widget;
//    CustomCtrl *LSWI_Widget;
//    CustomCtrl *LCW_Widget;
//    CustomCtrl *LCWI_Widget;
//    CustomCtrl *STR_Widget;
//    CustomCtrl *Ino_Widget;
    //常规
    CustomCtrl *HR_Widget;
//    CustomCtrl *BP_Widget;
//    CustomCtrl *MAP_Widget;
};

#endif // CUSTOMCTRL_H
