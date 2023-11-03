#ifndef REPORTDIALOGUI_H
#define REPORTDIALOGUI_H

#include <QDialog>
#include <QCoreApplication>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QTableView>
#include <QHeaderView>
#include <QLineEdit>
#include <QFile>

QT_BEGIN_NAMESPACE

class ReportDialogUi : public QDialog
{
public:
    QVBoxLayout *mainLayout;
    QHBoxLayout *searchLayout;
    QComboBox *searchCriteriaComboBox;
    QLineEdit *searchCriteriaLineEdit;
    QPushButton *searchButton;
    QPushButton *resetButton;
    QPushButton *getLatestDataButton;
    QPushButton *generatePdfButton;
    QPushButton *openReportDirButton;
    QPushButton *printPreviewButton;
    QTableView *reportTableView;
    void setupUi(QDialog *Dialog)
    {
        if (Dialog->objectName().isEmpty())
            Dialog->setObjectName(QString::fromUtf8("Dialog"));
        Dialog->setWindowFlag(Qt::WindowContextHelpButtonHint, false);
        Dialog->resize(1050, 600);

        QFile qssFile(":/qss/reportdialogui.qss");
        if (qssFile.open(QIODevice::ReadOnly))
            Dialog->setStyleSheet(qssFile.readAll());

        mainLayout = new QVBoxLayout(Dialog);
        mainLayout->setObjectName(QString::fromUtf8("mainLayout"));

        searchLayout = new QHBoxLayout();
        searchLayout->setObjectName(QString::fromUtf8("searchLayout"));

        mainLayout->addLayout(searchLayout);

        searchCriteriaComboBox = new QComboBox(Dialog);
        searchCriteriaComboBox->setObjectName(QString::fromUtf8("searchCriteriaComboBox"));

        searchLayout->addWidget(searchCriteriaComboBox);

        searchCriteriaLineEdit = new QLineEdit(Dialog);
        searchCriteriaLineEdit->setObjectName(QString::fromUtf8("searchCriteriaLineEdit"));

        searchLayout->addWidget(searchCriteriaLineEdit);

        searchButton = new QPushButton(Dialog);
        searchButton->setObjectName(QString::fromUtf8("searchButton"));

        searchLayout->addWidget(searchButton);

        resetButton = new QPushButton(Dialog);
        resetButton->setObjectName(QString::fromUtf8("resetButton"));

        searchLayout->addWidget(resetButton);

        searchLayout->addStretch();

        getLatestDataButton = new QPushButton(Dialog);
        getLatestDataButton->setObjectName(QString::fromUtf8("getReportDataButton"));

        searchLayout->addWidget(getLatestDataButton);

        searchLayout->addStretch();

        generatePdfButton = new QPushButton(Dialog);
        generatePdfButton->setObjectName(QString::fromUtf8("generatePdfButton"));

        searchLayout->addWidget(generatePdfButton);

        openReportDirButton = new QPushButton(Dialog);
        openReportDirButton->setObjectName(QString::fromUtf8("openReportDirButton"));

        searchLayout->addWidget(openReportDirButton);

        printPreviewButton = new QPushButton(Dialog);
        printPreviewButton->setObjectName(QString::fromUtf8("printPreviewButton"));

        searchLayout->addWidget(printPreviewButton);

        reportTableView = new QTableView(Dialog);
        reportTableView->setObjectName(QString::fromUtf8("reportTableView"));

        mainLayout->addWidget(reportTableView);

        retranslateUi(Dialog);
    }
    void retranslateUi(QDialog *Dialog)
    {
        Dialog->setWindowTitle(QCoreApplication::applicationName() + QString::fromUtf8(" - 报告"));
        searchCriteriaComboBox->addItems(QStringList{ QString::fromUtf8("姓名"),
                                                      QString::fromUtf8("编号"),
                                                      QString::fromUtf8("时间") });
        searchButton->setText(QString::fromUtf8("搜索"));
        resetButton->setText(QString::fromUtf8("重置"));
        getLatestDataButton->setText(QString::fromUtf8("获取最新数据"));
        generatePdfButton->setText(QString::fromUtf8("生成PDF"));
        openReportDirButton->setText(QString::fromUtf8("打开目录"));
        printPreviewButton->setText(QString::fromUtf8("打印预览"));

    }
};

namespace Ui {
    class ReportDialog : public ReportDialogUi {};
}

QT_END_NAMESPACE

#endif // REPORTDIALOGUI_H
