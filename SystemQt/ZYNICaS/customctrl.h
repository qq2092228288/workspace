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
    Type getType() const;
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
    void getChangeType(const Type &type);
    void timeoutSlot();
signals:
    void currentValue(qreal);
    void nameAndValue(const QString &, const double &rVal, const double &cVal);
    // 当前类型，交换显示的类型
    void changeType(const Type &, const Type &);
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
    Type m_type;
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
    CustomCtrl *getCustomCtrl(const Type &type);
    QList<CustomCtrl *> getAllCustomCtrls();
    std::vector<Type> getSaveTypes(bool trendChart);
    std::vector<Type> getAllTypes();
    std::vector<Type> getCurrentNames(bool trendChart);
    void saveTypes(const QString &fileName, const std::vector<Type> &types);
    QList<CustomCtrl *> getSaveCustomCtrls(bool trendChart);
    void changeCurrentTypes(const Type &current, const Type &change, bool trendChart);
    QList<qreal> getRecordValues();
    QList<qreal> getCurrentValues();
    void connectTrendChart(bool con);
signals:
    void openError(const QString &);
private:
    bool isSubset(const QJsonArray &array, bool showItem) const;
    std::vector<Type> arrayToVector(const QJsonArray &array) const;
private:
    QList<CustomCtrl *> allCustomCtrls;
    std::vector<Type> currentTypes;
    std::vector<Type> currentTrendChartTypes;
};

#endif // CUSTOMCTRL_H
