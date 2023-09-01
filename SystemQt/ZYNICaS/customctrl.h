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
#include <QThread>
#include <QQueue>
#include <numeric>

#include "trendchart.h"
#include "selectitemdialog.h"

enum Type {
    HR = 0,
    VET = 1,
    PEP = 2,
    TFC = 3,
    EPCI = 4,
    ISI = 5,
    EF = 6,
    SI = 7,
    CI = 8,
    RR = 9,
    BEEP = 10,
    Reserved = 15,
    CO = 16,
    SV = 17,
    HRV = 18,
    EDI = 19,
    Vol = 20,
    SVR = 21,
    SSVR = 22,
    SSVRI = 23,
    SVRI = 24,
    Vas = 25,
    LVET = 26,
    LSW = 27,
    LSWI = 28,
    LCW = 29,
    LCWI = 30,
    STR = 31,
    Ino = 32,
    SBP = 33,
    DBP = 34,
    MAP = 35,
    CVP = 36,
    LAP = 37,
    Dz = 38,
    Pos = 39,
    DO2 = 40,
    SVV = 41
};

QString typeName(const uchar &type);

QString typeName(const Type &type);

// 范围，记录值，当前值，名称，单位
struct ArgItems
{
    double minValue = 0;
    double maxValue = 0;
    double recordValue = 0;
    double currentValue = 0;
    QString dataName;
    QString dataName_cn;
    QString dataUnit;
    QList<double> values;
//    VQueue vqueue = VQueue(5);
    void clear()
    {
        recordValue = 0;
        currentValue = 0;
        values.clear();
//        vqueue.clear();
    }
};

struct Argument;

class CustomCtrl : public QWidget
{
    Q_OBJECT
public:
    explicit CustomCtrl(Argument arg, QWidget *parent = nullptr);
    virtual ~CustomCtrl();
    // 数据波动
    void startTimer(qreal accuracy);
    void stopTimer();
//    void smoothTransitionTimer(bool isStart);
protected:
    void mouseDoubleClickEvent(QMouseEvent *);
public:
    ArgItems getArgItems();
    ArgItems getDbpArgItems();
    double getRecordValue();
    double getCurrentValue();
    double getMinValue() const;
    double getMaxValue() const;
    QString getName() const;
    QString getValueStr() const;
    TrendChart *getTrendChart();
    int getDigit() const;
public slots:
    void clear();
    void setValue(const double &value);
    void setValue(const double &value, const QString &name);
    void setValues(const double &value, const double &value1);
    void valueWarning(bool warning);
    void recordValueSlot();
protected slots:
    void getChangeText(const QString &text);
    void timeoutSlot();
//    void oldAndNewValueTimerSlot();
signals:
    void currentValue(qreal);
    void nameAndValue(const QString &, const double &rVal, const double &cVal);
    // 当前名字，交换显示的名字
    void changeName(const QString &, const QString &);
private:
    QTimer *timer;
    SelectItemDialog *m_pDialog;
    QFrame *frame;
    QVBoxLayout *mainLayout;
    QLabel *mainLabel;      //主标签
    QLabel *secondaryLabel; //副标签
    QLineEdit *valueEdit;   //值
    QLabel *scopeLabel;     //范围
    QLabel *unitLabel;      //单位
    ArgItems aitems;
    ArgItems dbpaitems;
    int digit;
    // 精度
    qreal m_accuracy = 1;
    TrendChart *m_pTrendChart;
    // 数据平稳过渡
//    QList<qreal> oldAndNewValue;
//    QTimer *oldAndNewValueTimer;
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
    CustomCtrl *getCustomCtrl(const QString &cname);
    QList<CustomCtrl *> getAllCustomCtrls();
    QStringList getSaveNames(bool trendChart);
    QStringList getAllNames();
    QStringList getCurrentNames(bool trendChart);
    void saveNames(const QString &fileName, const QStringList &list);
    QList<CustomCtrl *> getSaveCustomCtrls(bool trendChart);
    void changeCurrentNames(const QString &current, const QString &change, bool trendChart);
    QList<qreal> getRecordValues();
    QList<qreal> getCurrentValues();
    void connectTrendChart(bool con);
signals:
    void openError(const QString &);
private:
    QList<CustomCtrl *> allCustomCtrls;
    QStringList currentNames;
    QStringList currentTrendChartNames;
};

#endif // CUSTOMCTRL_H
