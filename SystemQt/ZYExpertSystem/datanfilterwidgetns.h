#ifndef DATANFILTERWIDGETNS_H
#define DATANFILTERWIDGETNS_H

#include <QWidget>
#include <QCheckBox>
#include <QRadioButton>
#include <QLineEdit>
#include <QLabel>
#include <QHBoxLayout>
#include <QButtonGroup>
#include <QRegExpValidator>

#include "scopecalendardialog.h"

namespace DatanFilterWidgetNs {

class BaseFilter : public QWidget
{
    Q_OBJECT
public:
    explicit BaseFilter(QWidget *parent = nullptr);
    enum FilterType
    {
        All     = 0,
        Part    = 1
    };
    Q_ENUM(FilterType)
public:
    void setMain(const QString &text, const QString &tip = nullptr);
    bool isChecked() const;
    bool isAll() const;
signals:
    void checked(bool);
protected:
    void allLayoutAdd(QLayout *layout);
    void partLayoutAdd(QLayout *layout);
protected slots:
    virtual void radioButtonClicked(int id) = 0;
private:
    QHBoxLayout *mainLayout;
    QHBoxLayout *aLayout;
    QHBoxLayout *pLayout;
    QCheckBox *mainCheckBox;
    QButtonGroup *radioGroup;
    QRadioButton *allButton;
    QRadioButton *partButton;
};

class DataFilter : public BaseFilter
{
    Q_OBJECT
public:
    explicit DataFilter(const QString &text, const QString &unit = nullptr, QWidget *parent = nullptr);
protected:
    void setRegExp(const QRegExp &exp);
private slots:
    virtual void radioButtonClicked(int id) override;
private:
    void selectAll(bool all);
private:
    QLineEdit *firstLineEdit;
    QLabel *label;
    QLineEdit *secondLineEdit;
};

class StrFilter : public BaseFilter
{
    Q_OBJECT
public:
    explicit StrFilter(const QString &text, QWidget *parent = nullptr);
private slots:
    virtual void radioButtonClicked(int id) override;
private:
    QLineEdit *lineEdit;
};

class SexFilter : public BaseFilter
{
    Q_OBJECT
public:
    explicit SexFilter(QWidget *parent = nullptr);
private slots:
    virtual void radioButtonClicked(int id) override;
private:
    void selectAll(bool all);
private:
    QButtonGroup *genderGroup;
    QRadioButton *maleButton;
    QRadioButton *femaleButton;
};

class TimeFilter : public BaseFilter
{
    Q_OBJECT
public:
    explicit TimeFilter(QWidget *parent = nullptr);
private slots:
    virtual void radioButtonClicked(int id) override;
private:
    ScopeCalendarDialog *dialog;
};

class NicasFilter : public BaseFilter
{
    Q_OBJECT
public:
    explicit NicasFilter(const QJsonObject &object, QWidget *parent = nullptr);
    enum ScopeType
    {
        DeselectAll     = 0x00,
        Low             = 0x01,
        Normal          = 0x02,
        High            = 0x04,
        LowAndNormal    = Low | Normal,
        LowAndHigh      = Low | High,
        NormalAndHigh   = Normal | High,
        SelectAll       = Low | Normal | High,
        Scope           = 0xFF
    };
    Q_ENUM(ScopeType)
public:
    ScopeType stype() const;
    QString cn() const;
    QString en() const;
    QString unit() const;
    double min() const;
    double max() const;
    int digit() const;
    int type() const;
private slots:
    virtual void radioButtonClicked(int id) override;
    void checkBoxClicked(int id);
private:
    void selectAll(bool all);
private:
    QButtonGroup *group;
    QCheckBox *lowCheckBox;
    QCheckBox *normalCheckBox;
    QCheckBox *highCheckBox;
    QLineEdit *firstLineEdit;
    QLabel *label;
    QLineEdit *secondLineEdit;

    QString m_cn;
    QString m_en;
    QString m_unit;
    double m_min;
    double m_max;
    double m_digit;
    int m_type;
};


}

#endif // DATANFILTERWIDGETNS_H
