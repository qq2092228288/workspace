#include "parameterwidget.h"
#include "reportdataname.h"
#include "datacalculation.h"
#include "qmath.h"

ParamJson::ParamJson(const QJsonObject &object)
{
    en = object.find(ReportDataName::ekey(ReportDataName::en))->toString();
    cn = object.find(ReportDataName::ekey(ReportDataName::cn))->toString();
    unit = object.find(ReportDataName::ekey(ReportDataName::unit))->toString();
    digit = object.find(ReportDataName::ekey(ReportDataName::digit))->toInt();
    min = object.find(ReportDataName::ekey(ReportDataName::min))->toDouble();
    max = object.find(ReportDataName::ekey(ReportDataName::max))->toDouble();
}

BaseWidget::BaseWidget(const QString &en, const QString &cn, const QString &range,
                       const QString &unit, QWidget *parent)
    : QWidget{parent}
    , en{en}
    , cn{cn}
    , range{range}
    , unit{unit}
{
    clear();
}

QString BaseWidget::intercept(double value, int digit)
{
    return QString::number((static_cast<int>(value * qPow(10, digit))) / qPow(10, digit));
}

void BaseWidget::clear()
{
    level = Level::Normal;
    value = QString("--");
}

void BaseWidget::setValue(const Level &lvl, const double &val, const int &digit)
{
    if (val != DatCa::invalid()) {
        clear();
    }
    else {
        level = lvl;
        value = QString::number((static_cast<int>(val * qPow(10, digit))) / qPow(10, digit));
    }
}

void BaseWidget::setdata(const Level &lvl, const QString &val)
{
    level = lvl;
    value = val;
}

void BaseWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    if (Level::Low == level) {
        painter.setBrush(QColor(255, 193, 07));
        painter.setPen(QPen(QColor(255, 193, 07), 1));
    }
    else if (Level::High == level) {
        painter.setBrush(QColor(255, 105, 71));
        painter.setPen(QPen(QColor(255, 105, 71), 1));
    }
    else {
        painter.setBrush(QColor(64, 158, 255));
        painter.setPen(QPen(QColor(64, 158, 255), 1));
    }
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    auto rect = QRectF(QPointF(0, 0), QSizeF(width() - 1, height() - 1));
    painter.drawRoundedRect(rect, 60, 18, Qt::RelativeSize);

    auto f = font();
    f.setBold(true);
    f.setPixelSize(height() * 0.12);
    painter.setFont(f);
    painter.setPen(QPen(Qt::white, 1));
    painter.drawText(QRectF(width() * 0.1, height() * 0.05, width(), height()), en);
    f.setPixelSize(rect.height() * 0.08);
    painter.setFont(f);
    painter.drawText(QRectF(width() * 0.1, height() * 0.2, width(), height()), cn);
    f.setBold(false);
    f.setPixelSize(height() * 0.06);
    painter.setFont(f);
    painter.drawText(QRectF(0, height() * 0.78, width(), height() * 0.22), Qt::AlignHCenter, range);
    painter.drawText(QRectF(0, height() * 0.86, width(), height() * 0.16), Qt::AlignHCenter, unit);
    if (Level::Normal == level) {
        painter.setPen(QPen(QColor(0, 255, 127), 1));
    }
    else {
        painter.setPen(QPen(Qt::white, 1));
    }
    f.setPixelSize(height() * 0.25);
    painter.setFont(f);
    painter.drawText(rect, Qt::AlignCenter, value);
}

ParameterWidget::ParameterWidget(const ParamJson &param, QWidget *parent)
    : BaseWidget{param.en, param.cn,
                 param.min != param.max ? QString("%1~%2").arg(param.min).arg(param.max) : QString("-"),
                 param.unit, parent}
    , param{param}
{

}

void ParameterWidget::newdata(const double &value)
{
    if (value == DatCa::invalid()) {
        clear();
    }
    else if (param.min < param.max) {
        if (value < param.min) {
            setdata(Level::Low, intercept(value, param.digit));
        }
        else if (value > param.max) {
            setdata(Level::High, intercept(value, param.digit));
        }
        else {
            setdata(Level::Normal, intercept(value, param.digit));
        }
    }
    else {
        setdata(Level::Normal, QString::number(value));
    }
}

SbpDbpWidget::SbpDbpWidget(const ParamJson &sbpJson, const ParamJson &dbpJson, QWidget *parent)
    : BaseWidget{QString("%1/%2").arg(sbpJson.en, dbpJson.en),
                 QString("%1/%2").arg(sbpJson.cn, dbpJson.cn),
                 QString("%1~%2/%3~%4").arg(sbpJson.min).arg(sbpJson.max).arg(dbpJson.min).arg(dbpJson.max),
                 sbpJson.unit, parent}
    , sbpJson{sbpJson}, dbpJson{dbpJson}
{

}

void SbpDbpWidget::newdata(const double &sbp, const double &dbp)
{
    if (sbp < sbpJson.min || sbp > sbpJson.max || dbp < dbpJson.min || dbp > dbpJson.max) {
        setdata(Level::High, QString("%1/%2").arg(sbp).arg(dbp));
    }
    else {
        setdata(Level::Normal, QString("%1/%2").arg(sbp).arg(dbp));
    }
}
