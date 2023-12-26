#include "datawidget.h"
#include "ui/datawidgetui.h"
#include "datatablemodel.h"

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
        <<ui->primaryPlaceFilter
        <<ui->secondaryPlaceFilter
        <<ui->medicalRecordNumberFilter
        <<ui->patientNameFilter
        <<ui->timeFilter
        <<ui->positionFilter
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
        widget->hideFilterCriteria();
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

    connect(ui->backButton, &QPushButton::clicked, this, &DataWidget::close);
    connect(ui->exportXlsxButton, &QPushButton::clicked, this, &DataWidget::exportXlsxButtonSlot);
    connect(ui->resetButton, &QPushButton::clicked, this, &DataWidget::resetButtonSlot);
    connect(ui->confirmButton, &QPushButton::clicked, this, &DataWidget::confirmButtonSlot);

    setColumns();

    for (int i = 0; i < header.size(); ++i) {
        ui->tableView->setColumnWidth(i, 120);
    }
    // hide
    ui->deviceIdFilter->hide();
    ui->primaryPlaceFilter->hide();
    ui->secondaryPlaceFilter->hide();
    ui->medicalRecordNumberFilter->hide();
    ui->resetButton->hide();
    ui->confirmButton->hide();
}

DataWidget::~DataWidget()
{
    delete ui;
}

void DataWidget::showEvent(QShowEvent *event)
{
    event->accept();
    if (model->list().empty()) {
        getList();
    }
}

void DataWidget::closeEvent(QCloseEvent *event)
{
    event->accept();
    emit hidden();
}

void DataWidget::xlsxSetSpan(QXlsx::Document &xlsx, const int &columnIndex)
{
    if (columnIndex == 0) return;
    const auto list = model->list();
    for (int row = 0; row < list.count(); ++row) {
        if (!list.at(row).first) {
            xlsx.mergeCells(QXlsx::CellRange(row + 2, columnIndex, row + 3, columnIndex));
            ++row;
        }
    }
}

void DataWidget::getList()
{
    auto thread = QThread::create([=](){
        model->resetList();
    });
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    connect(thread, &QThread::finished, this, &DataWidget::setSpan);
    thread->start();
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
    columnFormat.setVerticalAlignment(QXlsx::Format::AlignVCenter);

    auto header = model->header();
    for (int i = 0, column = 0; i < header.count(); ++i) {
        if (!ui->tableView->isColumnHidden(i)) {
            xlsx.write(1, ++column, header.at(i));
            xlsx.setColumnWidth(column, 19);
            xlsx.setColumnFormat(column, columnFormat);
        }
    }

    QVector<DatanFilterWidgetNs::BaseFilter *> showFilters;
    for (int row = 0; row < model->rowCount(); ++row) {
        for (int col = 0, column = 0; col < filterWidgets.count(); ++col) {
            auto widget = filterWidgets.at(col);
            if (widget->isChecked()) {
                showFilters.append(widget);
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

    const auto list = model->list();

    if (showFilters.indexOf(ui->patientNameFilter) != -1) {
        auto columnIndex = showFilters.indexOf(ui->patientNameFilter) + 1;
        QString medicalRecordNumber;
        int rowSpan = 1;
        for (int row = 0; row < list.count(); ++row) {
            auto temp = list.at(row).medicalRecordNumber;
            if (temp == medicalRecordNumber) {
                ++rowSpan;
            }
            else {
                medicalRecordNumber = temp;
                if (rowSpan > 1) {
                    xlsx.mergeCells(QXlsx::CellRange(row - rowSpan + 2, columnIndex, row + 1, columnIndex));
                    rowSpan = 1;
                }
            }

        }
    }

    xlsxSetSpan(xlsx, showFilters.indexOf(ui->sexFilter) + 1);
    xlsxSetSpan(xlsx, showFilters.indexOf(ui->ageFilter) + 1);
    xlsxSetSpan(xlsx, showFilters.indexOf(ui->heightFilter) + 1);
    xlsxSetSpan(xlsx, showFilters.indexOf(ui->weightFilter) + 1);
    xlsxSetSpan(xlsx, showFilters.indexOf(ui->bsaFilter) + 1);

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

    ui->patientNameFilter->init();
    ui->medicalRecordNumberFilter->init();

    ui->sexFilter->init();
    ui->ageFilter->init();
    ui->heightFilter->init();
    ui->weightFilter->init();
    ui->bsaFilter->init();

    ui->nicasDataFilterWidget->init();

    getList();
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

void DataWidget::setSpan()
{
    const auto list = model->list();
    QString medicalRecordNumber;
    int rowSpan = 1;
    for (int i = 0; i < list.count(); ++i) {
        auto temp = list.at(i).medicalRecordNumber;
        if (temp == medicalRecordNumber) {
            ++rowSpan;
        }
        else {
            medicalRecordNumber = temp;
            if (rowSpan > 1) {
                ui->tableView->setSpan(i - rowSpan, 4, rowSpan, 1);
                rowSpan = 1;
            }
        }
    }
    for (int i = 0; i < list.count(); ++i) {
        if (!list.at(i).first) {
            ui->tableView->setSpan(i, 7, 2, 1);
            ui->tableView->setSpan(i, 8, 2, 1);
            ui->tableView->setSpan(i, 9, 2, 1);
            ui->tableView->setSpan(i, 10, 2, 1);
            ui->tableView->setSpan(i, 11, 2, 1);
            ++i;
        }
    }
}
