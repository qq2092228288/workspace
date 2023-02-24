#ifndef SURPLUSBASEARGSDIALOG_H
#define SURPLUSBASEARGSDIALOG_H

#include <QDialog>
#include <QShowEvent>
#include <QCloseEvent>
#include <QGridLayout>

#include "baseargwidget.h"

class SurplusBaseArgsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SurplusBaseArgsDialog(QList<BaseArgWidget *> &widgets, QWidget *parent = nullptr);
protected:
    void showEvent(QShowEvent *event);
    void closeEvent(QCloseEvent *event);
signals:
private:
    QGridLayout *mainLayout;
    QList<BaseArgWidget *> &m_aWidgets;
    QList<BaseArgWidget *> m_pShowInThisWidget;
};

#endif // SURPLUSBASEARGSDIALOG_H
