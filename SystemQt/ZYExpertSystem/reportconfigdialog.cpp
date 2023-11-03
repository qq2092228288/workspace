#include "reportconfigdialog.h"
#include "ui/reportconfigdialogui.h"
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QTextCodec>
#include <QDebug>

ReportConfigDialog::ReportConfigDialog(QWidget *parent)
    : QDialog{parent},
      ui{new Ui::ReportConfigDialog},
      m_fileName{QCoreApplication::applicationDirPath() + "/reportset.ini"}
{
    ui->setupUi(this);

    QFile file(m_fileName);
    QFileInfo info(file);
    if (!info.isFile()) {
        file.open(QFile::WriteOnly);
        QTextStream in(&file);
        in.setCodec(QTextCodec::codecForName("utf-8"));
        in<<QString("samepage=\"1\"\n");
        in<<QString("trendcharttitle=\"心血流图监测报告\"\n");
        in<<QString("mode=\"0\"\n");
        file.close();
    }
    if(file.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream out(&file);
        out.setCodec(QTextCodec::codecForName("utf-8"));
        QRegExp nameExp("(.*)(?:=)");
        QRegExp valueExp("(?:\")(.*)(?:\")");
        while (!out.atEnd()) {
            QString strLine = out.readLine();
            if (strLine.indexOf(nameExp) >= 0 && strLine.indexOf(valueExp) >= 0) {
                QString name = nameExp.cap(1);
                QString value = valueExp.cap(1);
                if (name == "samepage") {
                    ui->samePageCheckBox->setChecked(value.toInt());
                }
                else if (name == "trendcharttitle") {
                    ui->trendChartTitleLineEdit->setText(value);
                }
                else if (name == "mode") {
                    auto btn = ui->modeButtonGroup->button(value.toInt());
                    if (nullptr != btn) {
                        btn->setChecked(true);
                    }
                    else {
                        ui->modeButtonGroup->button(0)->setChecked(true);
                    }
                }
            }
        }
    }

    connect(ui->confirmButton, &QPushButton::clicked, this, &ReportConfigDialog::confirmButtonClicked);
}

ReportConfigDialog::~ReportConfigDialog()
{
    delete ui;
}

ReportConfig ReportConfigDialog::config()
{
    return ReportConfig(ui->samePageCheckBox->isChecked(),
                        Check_Mode(ui->modeButtonGroup->checkedId()),
                        ui->trendChartTitleLineEdit->text());
}

void ReportConfigDialog::confirmButtonClicked()
{
    QFile file(m_fileName);
    if(file.open(QFile::WriteOnly)){
        QTextStream in(&file);
        in.setCodec(QTextCodec::codecForName("utf-8"));
        in<<QString("samepage=\"%1\"\n").arg(ui->samePageCheckBox->isChecked());
        in<<QString("trendcharttitle=\"%1\"\n").arg(ui->trendChartTitleLineEdit->text());
        in<<QString("mode=\"%1\"\n").arg(ui->modeButtonGroup->checkedId());
        file.close();
    }
    this->close();
}
