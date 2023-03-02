#include "baseargwidgetsinst.h"
#include "datamanagement.h"
#include "surplusbaseargsdialog.h"

BaseArgWidgetsInst::BaseArgWidgetsInst(QObject *parent)
    : QObject{parent}
{
    appendWidget("心输出量", "CO", "L/min", 3.5, 9.0, 1);
    appendWidget("心脏指数", "CI", "L/min·m²", 2.0, 5.0, 1);
    appendWidget("氧输送", "DO2", "-", "mL/min", 0);
    appendWidget("心搏量", "SV", "mL/beat", 50, 120, 1);
    appendWidget("心搏指数", "SI", "mL/beat·m²", 35, 65, 0);
    appendWidget("心率变异性", "HRV", "%", -50, 50, 0);
    appendWidget("胸液传导性", "TFC", "1/Ω", 0.025, 0.045, 3);
    appendWidget("舒张末期指数", "EDI", "mL/beat·m²", 54, 130, 0);
    appendWidget("每搏输出变异性", "SVV", "%", 0, 10, 0);
    appendWidget("血管容积", "Vol", "%", -50, 50, 0);
    appendWidget("系统阻力", "SVR", "dyn·s·m²/cm^5", 489, 2262, 0);
    appendWidget("每搏外周阻力", "SSVR", "dyn·s/cm^5", 119.6, 429.2, 1);
    appendWidget("每搏外周阻力指数", "SSVRI", "dyn·s/cm^5/beat/m²", 99.7, 185.1, 1);
    appendWidget("系统阻力指数", "SVRI", "dyn·s·m²/cm^5", 1056, 4000, 0);
    appendWidget("血管顺应性", "Vas", "%", -50, 50, 0);
    appendWidget("射血前期", "PEP", "ms", 50, 120, 0);
    appendWidget("左室射血时间", "LVET", "ms", 250, 350, 0);
    appendWidget("左心室每搏做功", "LSW", "g·m", 20, 90, 0);
    appendWidget("每搏做功指数", "LSWI", "g·m/beat/m²", 39.3, 73.0, 1);
    appendWidget("左心室做功", "LCW", "kg·m", 5.4, 10.0, 1);
    appendWidget("左心室做功指数", "LCWI", "kg·m/m²", 1.81, 7.06, 2);
    appendWidget("收缩时间比", "STR", "-", 0, 0.4, 1);
    appendWidget("射血期收缩指数", "EPCI", "1/s", 0.038, 0.076, 3);
    appendWidget("变力状态指数", "ISI", "1/s²", 0.90, 1.70, 2);
    appendWidget("收缩变力性", "Ino", "%", -50, 50, 0);
    appendWidget("心率", "HR", "bpm", 60, 100, 0);
    appendWidget("收缩压/舒张压", "SBP/DBP", tr("90~140/60~90"), "mmHg", 0);
    appendWidget("平均动脉压", "MAP", "mmHg", 70, 105, 0);
}

BaseArgWidgetsInst::~BaseArgWidgetsInst()
{
    qDeleteAll(m_pWidgets.begin(), m_pWidgets.end());
}

QList<BaseArgWidget *> *BaseArgWidgetsInst::getBaseArgWidgetList()
{
    return &m_pWidgets;
}

void BaseArgWidgetsInst::clearArgWidgets()
{
    foreach (auto widget, m_pWidgets) {
        widget->clearArgValue();
    }
}

void BaseArgWidgetsInst::appendWidget(const QString &secondary, const QString &main, const QString &unit,
                                      const QString &scope, const int &digit)
{
    auto widget = new BaseArgWidget(main, secondary, scope, unit, digit, DataManagement::getInstance().zoom());
    connect(widget, &BaseArgWidget::mouseDoubleClicked, this, &BaseArgWidgetsInst::mouseDoubleClickedWidget);
    m_pWidgets<<widget;
}

void BaseArgWidgetsInst::appendWidget(const QString &secondary, const QString &main, const QString &unit,
                                      const qreal &min, const qreal &max, const int &digit)
{
    appendWidget(main, secondary, tr("%1~%2").arg(min).arg(max), unit, digit);
}

void BaseArgWidgetsInst::mouseDoubleClickedWidget()
{
    SurplusBaseArgsDialog dialog(m_pWidgets);
    dialog.exec();
}
