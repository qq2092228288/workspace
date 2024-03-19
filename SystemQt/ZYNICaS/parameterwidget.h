#ifndef PARAMETERWIDGET_H
#define PARAMETERWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QLabel>
#include <QLineEdit>
#include <QGridLayout>
#include <QJsonObject>

struct ParamJson
{
    ParamJson(const QJsonObject &object);
    QString en;
    QString cn;
    QString unit;
    int digit;
    double min;
    double max;
};

class BaseWidget : public QWidget
{
    Q_OBJECT
public:
    explicit BaseWidget(const QString &en, const QString &cn, const QString &range,
                        const QString &unit, QWidget *parent = nullptr);
    static QString intercept(double value, int digit);
    enum Level {
        Low,
        Normal,
        High
    };
    Q_ENUM(Level)
public slots:
    void clear();
signals:
protected:
    void setValue(const Level &lvl, const double &val, const int &digit);
    void setdata(const Level &lvl, const QString &val);
    void paintEvent(QPaintEvent *event) override;
private:
    const QString en;
    const QString cn;
    const QString range;
    const QString unit;
    Level level;
    QString value;
};

class ParameterWidget : public BaseWidget
{
    Q_OBJECT
public:
    explicit ParameterWidget(const ParamJson &param, QWidget *parent = nullptr);
public slots:
    void newdata(const double &value);
signals:
private:
    const ParamJson param;
};

class SbpDbpWidget : public BaseWidget
{
    Q_OBJECT
public:
    explicit SbpDbpWidget(const ParamJson &sbpJson, const ParamJson &dbpJson, QWidget *parent = nullptr);
public slots:
    void newdata(const double &sbp, const double &dbp);
signals:
private:
    const ParamJson sbpJson;
    const ParamJson dbpJson;
};

#endif // PARAMETERWIDGET_H
