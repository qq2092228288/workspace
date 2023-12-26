#ifndef MAINWIDGETUI_H
#define MAINWIDGETUI_H

#include <QWidget>
#include <QCoreApplication>
#include <QPushButton>
#include <QGridLayout>
#include <QLabel>
#include <QFile>

QT_BEGIN_NAMESPACE

class MainWidgetUi
{
public:
    QVBoxLayout *mainLayout;
    QHBoxLayout *headerLayout;
    QLabel *logoLabel;
    QLabel *titleLabel;
    QLabel *infoLabel;
    QPushButton *showReportButton;
    QPushButton *dataFilteringButton;
    QPushButton *systemConfigButton;
    QPushButton *exitButton;
    void setupUi(QWidget *Widget)
    {
        if (Widget->objectName().isEmpty())
            Widget->setObjectName(QString::fromUtf8("Widget"));

        QFile qssFile(":/qss/mainwidgetui.qss");
        if (qssFile.open(QIODevice::ReadOnly))
            Widget->setStyleSheet(qssFile.readAll());

        mainLayout = new QVBoxLayout(Widget);
        mainLayout->setObjectName(QString::fromUtf8("mainLayout"));

        headerLayout = new QHBoxLayout();
        headerLayout->setObjectName(QString::fromUtf8("headerLayout"));

        mainLayout->addLayout(headerLayout);

        logoLabel = new QLabel(Widget);
        logoLabel->setObjectName(QString::fromUtf8("logoLabel"));


        headerLayout->addWidget(logoLabel, 1);

        titleLabel = new QLabel(Widget);
        titleLabel->setObjectName(QString::fromUtf8("titleLabel"));

        headerLayout->addWidget(titleLabel, 10, Qt::AlignHCenter | Qt::AlignBottom);

        infoLabel = new QLabel(Widget);
        infoLabel->setObjectName(QString::fromUtf8("rightLabel"));

        headerLayout->addWidget(infoLabel, 1);

        mainLayout->addStretch();

        showReportButton = new QPushButton(Widget);
        showReportButton->setObjectName(QString::fromUtf8("showReportButton"));

        mainLayout->addWidget(showReportButton, 0, Qt::AlignCenter);

        dataFilteringButton = new QPushButton(Widget);
        dataFilteringButton->setObjectName(QString::fromUtf8("dataFilteringButton"));

        mainLayout->addWidget(dataFilteringButton, 0, Qt::AlignCenter);

        systemConfigButton = new QPushButton(Widget);
        systemConfigButton->setObjectName(QString::fromUtf8("systemConfigButton"));

        mainLayout->addWidget(systemConfigButton, 0, Qt::AlignCenter);

        exitButton = new QPushButton(Widget);
        exitButton->setObjectName(QString::fromUtf8("exitButton"));

        mainLayout->addWidget(exitButton, 0, Qt::AlignCenter);

        mainLayout->addStretch();

        retranslateUi(Widget);
    }

    void retranslateUi(QWidget *Widget)
    {
        Widget->setWindowTitle(QCoreApplication::applicationName());
        logoLabel->setPixmap(QPixmap(":/images/logo.png").scaled(200, 200));
        infoLabel->setOpenExternalLinks(true);
        titleLabel->setText(QCoreApplication::applicationName());
        auto str = QString::fromUtf8("<p style='%1'>软件版本: %2</p>"
                                     "<p style='%1'>公司名称：</p>"
                                     "<p style='%1'>%3</p>"
                                     "<p style='%1'>公司网址：</p>"
                                     "<a href='%4' style='%1 color:blue; text-decoration:underline;'>%4</a>")
                .arg(QString("font-size:14px;text-align:center;margin:5px;"),
                     QCoreApplication::applicationVersion(),
                     QCoreApplication::organizationName(),
                     QCoreApplication::organizationDomain());
        infoLabel->setText(str);
        showReportButton->setText(QString::fromUtf8("查看报告"));
        dataFilteringButton->setText(QString::fromUtf8("数据统计"));
        systemConfigButton->setText(QString::fromUtf8("系统配置"));
        exitButton->setText(QString::fromUtf8("退出系统"));
    }
};

namespace Ui {
    class MainWidget : public MainWidgetUi {};
}

QT_END_NAMESPACE

#endif // MAINWIDGETUI_H
