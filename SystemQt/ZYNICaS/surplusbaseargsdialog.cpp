#include "surplusbaseargsdialog.h"


SurplusBaseArgsDialog::SurplusBaseArgsDialog(QList<BaseArgWidget *> &widgets, QWidget *parent)
    : QDialog{parent},m_aWidgets{widgets}
{
    mainLayout = new QGridLayout(this);
}

void SurplusBaseArgsDialog::showEvent(QShowEvent *event)
{
    foreach (auto widget, m_aWidgets) {
        if (widget->isHidden()) {
            mainLayout->addWidget(widget);
            m_pShowInThisWidget<<widget;
        }
    }
    event->accept();
}

void SurplusBaseArgsDialog::closeEvent(QCloseEvent *event)
{
    event->accept();
    foreach (auto widget, m_pShowInThisWidget) {
        widget->hide();
    }
    m_pShowInThisWidget.clear();
}
