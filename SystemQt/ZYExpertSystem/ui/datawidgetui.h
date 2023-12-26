#ifndef DATAWIDGETUI_H
#define DATAWIDGETUI_H

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
#include <QStackedWidget>
#include <QFile>
#include <QCoreApplication>

#include "datanfilterwidgetns.h"
#include "nicasdatafilterwidget.h"

QT_BEGIN_NAMESPACE

using namespace DatanFilterWidgetNs;

class DataWidgetUi : public QWidget
{
public:
    QHBoxLayout *mainLayout;
    QVBoxLayout *filterLayout;

    QGroupBox *operationGroupBox;
    QHBoxLayout *hLayout;
    QPushButton *backButton;
    QPushButton *exportXlsxButton;
    QPushButton *resetButton;
    QPushButton *confirmButton;

    QGroupBox *baseGroupBox;
    QGridLayout *baseLayout;

    GeneralFilter *deviceIdFilter;
    GeneralFilter *primaryPlaceFilter;
    GeneralFilter *secondaryPlaceFilter;
    TimeFilter *timeFilter;
    PositionFilter *positionFilter;

    QGroupBox *infoGroupBox;
    QGridLayout *infoLayout;
    GeneralFilter *patientNameFilter;
    SexFilter *sexFilter;
    GeneralFilter *medicalRecordNumberFilter;
    DataFilter *ageFilter;
    DataFilter *heightFilter;
    DataFilter *weightFilter;
    DataFilter *bsaFilter;

    QGroupBox *dataGroupBox;
    QVBoxLayout *dataLayout;

    NicasDataFilterWidget *nicasDataFilterWidget;

    QTableView *tableView;

    void setupUi(QWidget *Widget)
    {
        if (Widget->objectName().isEmpty())
            Widget->setObjectName(QString::fromUtf8("Widget"));

        QFile qssFile(":/qss/datanwidgetui.qss");
        if (qssFile.open(QIODevice::ReadOnly))
            Widget->setStyleSheet(qssFile.readAll());

        mainLayout = new QHBoxLayout(Widget);
        mainLayout->setObjectName(QString::fromUtf8("mainLayout"));

        filterLayout = new QVBoxLayout();
        filterLayout->setObjectName(QString::fromUtf8("filterLayout"));

        mainLayout->addLayout(filterLayout, 1);

        operationGroupBox = new QGroupBox(Widget);
        operationGroupBox->setObjectName(QString::fromUtf8("operationGroupBox"));

        filterLayout->addWidget(operationGroupBox);

        hLayout = new QHBoxLayout(operationGroupBox);
        hLayout->setObjectName(QString::fromUtf8("hLayout"));

        backButton = new QPushButton(Widget);
        backButton->setObjectName(QString::fromUtf8("backButton"));

        hLayout->addWidget(backButton);

        hLayout->addStretch();

        exportXlsxButton = new QPushButton(Widget);
        exportXlsxButton->setObjectName(QString::fromUtf8("exportXlsxButton"));

        hLayout->addWidget(exportXlsxButton);

        resetButton = new QPushButton(Widget);
        resetButton->setObjectName(QString::fromUtf8("resetButton"));

        hLayout->addWidget(resetButton);

        confirmButton = new QPushButton(Widget);
        confirmButton->setObjectName(QString::fromUtf8("confirmButton"));

        hLayout->addWidget(confirmButton);

        baseGroupBox = new QGroupBox(Widget);
        baseGroupBox->setObjectName(QString::fromUtf8("baseGroupBox"));

        filterLayout->addWidget(baseGroupBox);

        baseLayout = new QGridLayout(baseGroupBox);
        baseLayout->setObjectName(QString::fromUtf8("baseLayout"));

        infoGroupBox = new QGroupBox(Widget);
        infoGroupBox->setObjectName(QString::fromUtf8("infoGroupBox"));

        filterLayout->addWidget(infoGroupBox);

        infoLayout = new QGridLayout(infoGroupBox);
        infoLayout->setObjectName(QString::fromUtf8("infoLayout"));

        dataGroupBox = new QGroupBox(Widget);
        dataGroupBox->setObjectName(QString::fromUtf8("baseGroupBox"));

        filterLayout->addWidget(dataGroupBox);

        dataLayout = new QVBoxLayout(dataGroupBox);
        dataLayout->setObjectName(QString::fromUtf8("dataLayout"));

        deviceIdFilter = new GeneralFilter(false, Widget);
        deviceIdFilter->setObjectName(QString::fromUtf8("deviceIdFilter"));

        baseLayout->addWidget(deviceIdFilter, 0, 0, Qt::AlignLeft);

        primaryPlaceFilter = new GeneralFilter(false, Widget);
        primaryPlaceFilter->setObjectName(QString::fromUtf8("primaryPlaceFilter"));

        baseLayout->addWidget(primaryPlaceFilter, 2, 0, Qt::AlignLeft);

        secondaryPlaceFilter = new GeneralFilter(false, Widget);
        secondaryPlaceFilter->setObjectName(QString::fromUtf8("secondaryPlaceFilter"));

        baseLayout->addWidget(secondaryPlaceFilter, 2, 1, Qt::AlignLeft);

        timeFilter = new TimeFilter(Widget);
        timeFilter->setObjectName(QString::fromUtf8("timeFilter"));

        baseLayout->addWidget(timeFilter, 1, 0, Qt::AlignLeft);

        positionFilter = new PositionFilter(Widget);
        positionFilter->setObjectName(QString::fromUtf8("positionFilter"));

        baseLayout->addWidget(positionFilter, 1, 1, Qt::AlignLeft);

        patientNameFilter = new GeneralFilter(true, Widget);
        patientNameFilter->setObjectName(QString::fromUtf8("patientNameFilter"));

        infoLayout->addWidget(patientNameFilter, 0, 0, Qt::AlignLeft);

        sexFilter = new SexFilter(Widget);
        sexFilter->setObjectName(QString::fromUtf8("sexFilter"));

        infoLayout->addWidget(sexFilter, 0, 1, Qt::AlignLeft);

        medicalRecordNumberFilter = new GeneralFilter(false, Widget);
        medicalRecordNumberFilter->setObjectName(QString::fromUtf8("medicalRecordNumberFilter"));

        infoLayout->addWidget(medicalRecordNumberFilter, 1, 0, Qt::AlignLeft);

        ageFilter = new DataFilter(Widget);
        ageFilter->setObjectName(QString::fromUtf8("ageFilter"));

        infoLayout->addWidget(ageFilter, 2, 0, Qt::AlignLeft);

        heightFilter = new DataFilter(Widget);
        heightFilter->setObjectName(QString::fromUtf8("heightFilter"));

        infoLayout->addWidget(heightFilter, 2, 1, Qt::AlignLeft);

        weightFilter = new DataFilter(Widget);
        weightFilter->setObjectName(QString::fromUtf8("weightFilter"));

        infoLayout->addWidget(weightFilter, 3, 0, Qt::AlignLeft);

        bsaFilter = new DataFilter(Widget);
        bsaFilter->setObjectName(QString::fromUtf8("bsaFilter"));

        infoLayout->addWidget(bsaFilter, 3, 1, Qt::AlignLeft);

        nicasDataFilterWidget = new NicasDataFilterWidget(Widget);
        nicasDataFilterWidget->setObjectName(QString::fromUtf8("nicasDataFilterWidget"));

        dataLayout->addWidget(nicasDataFilterWidget);

        tableView = new QTableView(Widget);
        tableView->setObjectName(QString::fromUtf8("tableView"));

        mainLayout->addWidget(tableView, 10);

        retranslateUi(Widget);
    }
    void retranslateUi(QWidget *Widget)
    {
        Widget->setWindowTitle(QCoreApplication::applicationName() + QString::fromUtf8(" - 数据统计"));
        backButton->setText(QString::fromUtf8("返回"));
        exportXlsxButton->setText(QString::fromUtf8("导出"));
        resetButton->setText(QString::fromUtf8("重置"));
        confirmButton->setText(QString::fromUtf8("确认"));
        baseGroupBox->setTitle(QString::fromUtf8("基本"));
        infoGroupBox->setTitle(QString::fromUtf8("信息"));
        dataGroupBox->setTitle(QString::fromUtf8("数据"));
        deviceIdFilter->setMain(QString::fromUtf8("设备编号"));
        primaryPlaceFilter->setMain(QString::fromUtf8("医院"));
        secondaryPlaceFilter->setMain(QString::fromUtf8("科室"));
        patientNameFilter->setMain(QString::fromUtf8("姓名"));
        medicalRecordNumberFilter->setMain(QString::fromUtf8("编号"));
        ageFilter->setMain(QString::fromUtf8("年龄"));
        ageFilter->setUnit(QString::fromUtf8("岁"));
        heightFilter->setMain(QString::fromUtf8("身高"));
        heightFilter->setUnit(QString::fromUtf8("cm"));
        weightFilter->setMain(QString::fromUtf8("体重"));
        weightFilter->setUnit(QString::fromUtf8("kg"));
        bsaFilter->setMain(QString::fromUtf8("体表面积"));
        bsaFilter->setUnit(QString::fromUtf8("m²"));

    }
};

namespace Ui {
    class DataWidget : public DataWidgetUi {};
}

QT_END_NAMESPACE

#endif // DATAWIDGETUI_H
