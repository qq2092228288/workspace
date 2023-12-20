#ifndef DATANWIDGETUI_H
#define DATANWIDGETUI_H

#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include <QTableView>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QRadioButton>
#include <QGroupBox>
#include <QButtonGroup>
#include <QCheckBox>
#include <QFile>

QT_BEGIN_NAMESPACE

class DatanWidgetUi : public QWidget
{
public:
    QGridLayout *mainLayout;

    void setupUi(QWidget *Widget)
    {
        if (Widget->objectName().isEmpty())
            Widget->setObjectName(QString::fromUtf8("Widget"));

        QFile qssFile(":/qss/datanwidgetui.qss");
        if (qssFile.open(QIODevice::ReadOnly))
            Widget->setStyleSheet(qssFile.readAll());


    }
    void retranslateUi(QWidget *Widget)
    {

    }
};

namespace Ui {
    class DatanWidget : public DatanWidgetUi {};
}

QT_END_NAMESPACE

#endif // DATANWIDGETUI_H
