#ifndef SYSTEMCONFIGDIALOGUI_H
#define SYSTEMCONFIGDIALOGUI_H

#include <QDialog>
#include <QCoreApplication>
#include <QFile>
#include <QLabel>
#include <QPushButton>
#include <QGridLayout>
#include <QLineEdit>
#include <QCheckBox>
#include <QRadioButton>
#include <QButtonGroup>
#include <QFrame>

#include "reportset.h"

QT_BEGIN_NAMESPACE

class SystemConfigDialogUi : public QDialog
{
public:
    QGridLayout *mainLayout;

    QLabel *trendChartTitleLabel;
    QLineEdit *trendChartTitleLineEdit;

    QCheckBox *samePageCheckBox;

    QButtonGroup *modeButtonGroup;
    QRadioButton *hypertensionButton;
    QRadioButton *internalMedicineButton;
    QRadioButton *intensiveCareUnitButton;
    QRadioButton *physicalExaminationButton;

    QPushButton *confirmButton;

    void setupUi(QDialog *Dialog)
    {
        if (Dialog->objectName().isEmpty())
            Dialog->setObjectName(QString::fromUtf8("Dialog"));
        Dialog->resize(300, 200);
        Dialog->setWindowFlag(Qt::WindowContextHelpButtonHint, false);
        QFile qssFile(":/qss/systemconfidialogui.qss");
        if (qssFile.open(QIODevice::ReadOnly))
            Dialog->setStyleSheet(qssFile.readAll());

        mainLayout = new QGridLayout(Dialog);
        mainLayout->setObjectName(QString::fromUtf8("mainLayout"));

        QFrame *hline1 = new QFrame(Dialog);
        hline1->setObjectName(QString::fromUtf8("hline1"));
        hline1->setFrameShape(QFrame::HLine);
        hline1->setFrameShadow(QFrame::Sunken);

        mainLayout->addWidget(hline1, 1, 0, 1, 2);

        trendChartTitleLabel = new QLabel(Dialog);
        trendChartTitleLabel->setObjectName(QString::fromUtf8("trendChartTitleLabel"));

        mainLayout->addWidget(trendChartTitleLabel, 2, 0);

        trendChartTitleLineEdit = new QLineEdit(Dialog);
        trendChartTitleLineEdit->setObjectName(QString::fromUtf8("trendChartTitleLineEdit"));

        mainLayout->addWidget(trendChartTitleLineEdit, 2, 1);

        QFrame *hline2 = new QFrame(Dialog);
        hline2->setObjectName(QString::fromUtf8("hline2"));
        hline2->setFrameShape(QFrame::HLine);
        hline2->setFrameShadow(QFrame::Sunken);

        mainLayout->addWidget(hline2, 3, 0, 1, 2);

        samePageCheckBox = new QCheckBox(Dialog);
        samePageCheckBox->setObjectName(QString::fromUtf8("samePageCheckBox"));

        mainLayout->addWidget(samePageCheckBox, 4, 0);

        QFrame *hline3 = new QFrame(Dialog);
        hline3->setObjectName(QString::fromUtf8("hline3"));
        hline3->setFrameShape(QFrame::HLine);
        hline3->setFrameShadow(QFrame::Sunken);

        mainLayout->addWidget(hline3, 5, 0, 1, 2);

        modeButtonGroup = new QButtonGroup(Dialog);
        modeButtonGroup->setObjectName(QString::fromUtf8("modeButtonGroup"));

        hypertensionButton = new QRadioButton(Dialog);
        hypertensionButton->setObjectName(QString::fromUtf8("hypertensionButton"));
        modeButtonGroup->addButton(hypertensionButton, Check_Mode::Hypertension);

        mainLayout->addWidget(hypertensionButton, 6, 0);

        internalMedicineButton = new QRadioButton(Dialog);
        internalMedicineButton->setObjectName(QString::fromUtf8("internalMedicineButton"));
        modeButtonGroup->addButton(internalMedicineButton, Check_Mode::InternalMedicine);

        mainLayout->addWidget(internalMedicineButton, 6, 1);

        intensiveCareUnitButton = new QRadioButton(Dialog);
        intensiveCareUnitButton->setObjectName(QString::fromUtf8("intensiveCareUnitButton"));
        modeButtonGroup->addButton(intensiveCareUnitButton, Check_Mode::IntensiveCareUnit);

        mainLayout->addWidget(intensiveCareUnitButton, 7, 0);

        physicalExaminationButton = new QRadioButton(Dialog);
        physicalExaminationButton->setObjectName(QString::fromUtf8("physicalExaminationButton"));
        modeButtonGroup->addButton(physicalExaminationButton, Check_Mode::PhysicalExamination);

        mainLayout->addWidget(physicalExaminationButton, 7, 1);

        QFrame *hline4 = new QFrame(Dialog);
        hline4->setObjectName(QString::fromUtf8("hline4"));
        hline4->setFrameShape(QFrame::HLine);
        hline4->setFrameShadow(QFrame::Sunken);

        mainLayout->addWidget(hline4, 8, 0, 1, 2);

        confirmButton = new QPushButton(Dialog);
        confirmButton->setObjectName(QString::fromUtf8("confirmButton"));

        mainLayout->addWidget(confirmButton, 9, 0, 1, 2);

        retranslateUi(Dialog);
    }
    void retranslateUi(QDialog *Dialog)
    {
        Dialog->setWindowTitle(QCoreApplication::applicationName() + QString::fromUtf8(" - 系统配置"));
        trendChartTitleLabel->setText(QString::fromUtf8("趋势图页标题:"));
        samePageCheckBox->setText(QString::fromUtf8("双体位同页"));
        samePageCheckBox->setToolTip(QString::fromStdString("若勾选，且为双体位，则检测数据在同一页报告中体现"));
        hypertensionButton->setText(QString::fromUtf8("高血压模式"));
        internalMedicineButton->setText(QString::fromUtf8("内科模式"));
        intensiveCareUnitButton->setText(QString::fromUtf8("重症模式"));
        physicalExaminationButton->setText(QString::fromUtf8("体检模式"));
        confirmButton->setText(QString::fromUtf8("确认"));
    }
};

namespace Ui {
    class SystemConfigDialog : public SystemConfigDialogUi {};
}

QT_END_NAMESPACE

#endif // SYSTEMCONFIGDIALOGUI_H
