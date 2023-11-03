#ifndef REPORTCONFIGDIALOGUI_H
#define REPORTCONFIGDIALOGUI_H

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

#include "reportset.h"

QT_BEGIN_NAMESPACE

class ReportConfigDialogUi : public QDialog
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
        QFile qssFile(":/qss/reportconfidialogui.qss");
        if (qssFile.open(QIODevice::ReadOnly))
            Dialog->setStyleSheet(qssFile.readAll());

        mainLayout = new QGridLayout(Dialog);
        mainLayout->setObjectName(QString::fromUtf8("mainLayout"));

        trendChartTitleLabel = new QLabel(Dialog);
        trendChartTitleLabel->setObjectName(QString::fromUtf8("trendChartTitleLabel"));

        mainLayout->addWidget(trendChartTitleLabel, 0, 0);

        trendChartTitleLineEdit = new QLineEdit(Dialog);
        trendChartTitleLineEdit->setObjectName(QString::fromUtf8("trendChartTitleLineEdit"));

        mainLayout->addWidget(trendChartTitleLineEdit, 0, 1);

        samePageCheckBox = new QCheckBox(Dialog);
        samePageCheckBox->setObjectName(QString::fromUtf8("samePageCheckBox"));

        mainLayout->addWidget(samePageCheckBox, 1, 0);

        modeButtonGroup = new QButtonGroup(Dialog);
        modeButtonGroup->setObjectName(QString::fromUtf8("modeButtonGroup"));

        hypertensionButton = new QRadioButton(Dialog);
        hypertensionButton->setObjectName(QString::fromUtf8("hypertensionButton"));
        modeButtonGroup->addButton(hypertensionButton, Check_Mode::Hypertension);

        mainLayout->addWidget(hypertensionButton, 2, 0);

        internalMedicineButton = new QRadioButton(Dialog);
        internalMedicineButton->setObjectName(QString::fromUtf8("internalMedicineButton"));
        modeButtonGroup->addButton(internalMedicineButton, Check_Mode::InternalMedicine);

        mainLayout->addWidget(internalMedicineButton, 2, 1);

        intensiveCareUnitButton = new QRadioButton(Dialog);
        intensiveCareUnitButton->setObjectName(QString::fromUtf8("intensiveCareUnitButton"));
        modeButtonGroup->addButton(intensiveCareUnitButton, Check_Mode::IntensiveCareUnit);

        mainLayout->addWidget(intensiveCareUnitButton, 3, 0);

        physicalExaminationButton = new QRadioButton(Dialog);
        physicalExaminationButton->setObjectName(QString::fromUtf8("physicalExaminationButton"));
        modeButtonGroup->addButton(physicalExaminationButton, Check_Mode::PhysicalExamination);

        mainLayout->addWidget(physicalExaminationButton, 3, 1);

        confirmButton = new QPushButton(Dialog);
        confirmButton->setObjectName(QString::fromUtf8("confirmButton"));

        mainLayout->addWidget(confirmButton, 4, 0, 1, 2);

        retranslateUi(Dialog);
    }
    void retranslateUi(QDialog *Dialog)
    {
        Dialog->setWindowTitle(QCoreApplication::applicationName() + QString::fromUtf8(" - 报告配置"));
        trendChartTitleLabel->setText(QString::fromUtf8("趋势图页标题:"));
        samePageCheckBox->setText(QString::fromUtf8("双体位同页"));
        hypertensionButton->setText(QString::fromUtf8("高血压模式"));
        internalMedicineButton->setText(QString::fromUtf8("内科模式"));
        intensiveCareUnitButton->setText(QString::fromUtf8("重症模式"));
        physicalExaminationButton->setText(QString::fromUtf8("体检模式"));
        confirmButton->setText(QString::fromUtf8("确认"));
    }
};

namespace Ui {
    class ReportConfigDialog : public ReportConfigDialogUi {};
}

QT_END_NAMESPACE

#endif // REPORTCONFIGDIALOGUI_H
