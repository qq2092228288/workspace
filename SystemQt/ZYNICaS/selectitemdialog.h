#ifndef SELECTITEMDIALOG_H
#define SELECTITEMDIALOG_H

#include <QDialog>
#include <QComboBox>
#include <QPushButton>
#include <QShowEvent>
#include <QFileInfo>
#include <QFile>
#include "datatype.h"

class SelectItemDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SelectItemDialog(bool trendChart, QWidget *parent = nullptr);
    void confirmSlot();
protected:
    void showEvent(QShowEvent *event);
signals:
    void currentType(Type);
private:
    bool m_trendChart;
    QComboBox *comboBox;
    QPushButton *confirmBtn;
};

#endif // SELECTITEMDIALOG_H
