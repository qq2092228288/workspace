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
#include "datatype.h"

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
    QVector<double> values;
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
    void setWarning();
    void valueWarning(int warning);
    void recordValueSlot();
protected slots:
    void getChangeText(const QString &text);
    void timeoutSlot();
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
