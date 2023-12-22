#include "datawidget.h"
#include "ui/datawidgetui.h"
#include "datatablemodel.h"

#include <xlsxdocument.h>
#include <QCloseEvent>
#include <QThread>
#include <QFileDialog>
#include <QStandardPaths>

DataWidget::DataWidget(QWidget *parent)
    : QWidget{parent}
    , ui{new Ui::DataWidget}
{
    ui->setupUi(this);

    filterWidgets
        <<ui->deviceIdFilter
        <<ui->timeFilter
        <<ui->positionFilter
        <<ui->primaryPlaceFilter
        <<ui->secondaryPlaceFilter
        <<ui->sexFilter
        <<ui->ageFilter
        <<ui->heightFilter
        <<ui->weightFilter
        <<ui->bsaFilter;
    foreach (auto filter, ui->nicasDataFilterWidget->filters()) {
        filterWidgets<<filter;
    }
    QStringList header;
    foreach (auto widget, filterWidgets) {
        if (nullptr != qobject_cast<NicasFilter *>(widget)) {
            auto filter = qobject_cast<NicasFilter *>(widget);
            header<<QString("%1\n%2 (%3)").arg(filter->cn(), filter->en(), filter->unit());
        }
        else if (nullptr != qobject_cast<DataFilter *>(widget)) {
            auto filter = qobject_cast<DataFilter *>(widget);
            header<<QString("%1\n(%2)").arg(filter->text(), filter->unit());
        }
        else {
            header<<widget->text();
        }
        connect(widget, &BaseFilter::checked, this, &DataWidget::setColumns);
    }

    model = new DataTableModel(header, this);
    ui->tableView->setModel(model);

    connect(ui->exportXlsxButton, &QPushButton::clicked, this, &DataWidget::exportXlsxButtonSlot);
    connect(ui->resetButton, &QPushButton::clicked, this, &DataWidget::resetButtonSlot);
    connect(ui->confirmButton, &QPushButton::clicked, this, &DataWidget::confirmButtonSlot);

    setColumns();

    for (int i = 0; i < header.size(); ++i) {
        ui->tableView->setColumnWidth(i, 120);
    }
}

DataWidget::~DataWidget()
{
    delete ui;
}

void DataWidget::closeEvent(QCloseEvent *event)
{
    event->accept();
    emit hidden();
}

void DataWidget::exportXlsxButtonSlot()
{
    QXlsx::Document xlsx;

    QXlsx::Format rowFormat;
    rowFormat.setFontBold(true);
    rowFormat.setHorizontalAlignment(QXlsx::Format::AlignHCenter);
    rowFormat.setVerticalAlignment(QXlsx::Format::AlignVCenter);
    rowFormat.setTextWarp(true);
    xlsx.setRowFormat(1, rowFormat);

    QXlsx::Format columnFormat;
    columnFormat.setHorizontalAlignment(QXlsx::Format::AlignLeft);

    auto header = model->header();
    for (int i = 0, column = 0; i < header.count(); ++i) {
        if (!ui->tableView->isColumnHidden(i)) {
            xlsx.write(1, ++column, header.at(i));
            xlsx.setColumnWidth(column, 19);
            xlsx.setColumnFormat(column, columnFormat);
        }
    }

    for (int row = 0; row < model->rowCount(); ++row) {
        for (int col = 0, column = 0; col < filterWidgets.count(); ++col) {
            auto widget = filterWidgets.at(col);
            if (widget->isChecked()) {
                auto index = model->index(row, col);
                auto data = model->data(index, Qt::DisplayRole);
                switch (data.type()) {
                case QVariant::Int:
                case QVariant::Double:
                    xlsx.write(row + 2, ++column, data.toDouble());
                    break;
                case QVariant::DateTime:
                    xlsx.write(row + 2, ++column, data.toDateTime().toString("yyyy/MM/dd hh:mm"));
                    break;
                default:
                    xlsx.write(row + 2, ++column, data.toString());
                    break;
                }

            }
        }
    }
    auto dpath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    auto fileName = QFileDialog::getSaveFileName(this,
                                                 "save excel file",
                                                 dpath + "/nicas",
                                                 "*.xlsx");
    if (!fileName.isEmpty()) {
        xlsx.saveAs(fileName);
    }
}

void DataWidget::resetButtonSlot()
{
    ui->deviceIdFilter->init();
    ui->timeFilter->init();
    ui->positionFilter->init();
    ui->primaryPlaceFilter->init();
    ui->secondaryPlaceFilter->init();

    ui->sexFilter->init();
    ui->ageFilter->init();
    ui->heightFilter->init();
    ui->weightFilter->init();
    ui->bsaFilter->init();

    ui->nicasDataFilterWidget->init();

    auto thread = QThread::create([=](){
        model->resetList();
    });
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    thread->start();
}

void DataWidget::confirmButtonSlot()
{

}

void DataWidget::setColumns()
{
    for (int i = 0; i < filterWidgets.size(); ++i) {
        ui->tableView->setColumnHidden(i, !filterWidgets.at(i)->isChecked());
    }
}
