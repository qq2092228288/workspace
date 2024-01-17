#include "printpreviewdialog.h"
#include <QScreen>
#include <QEvent>
#include <QGestureEvent>
#include <QPinchGesture>

PrintPreviewDialog::PrintPreviewDialog(const ReportConfig &config, const QJsonObject &data,
                                       QPrinter *printer, QWidget *parent)
    : QPrintPreviewDialog{printer, parent},
      m_config{config},
      m_data{data}
{
    initPrinter(printer);
    resize(screen()->availableSize());
    setWindowTitle("报告预览");

    grabGesture(Qt::PinchGesture);
    setAttribute(Qt::WA_AcceptTouchEvents);
    setWindowFlags(Qt::Dialog | Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);

    connect(this, &QPrintPreviewDialog::paintRequested, this, &PrintPreviewDialog::paintRequestedSlot);
}

void PrintPreviewDialog::initPrinter(QPrinter *printer)
{
    printer->setPageSize(QPageSize(QSizeF(210, 297), QPageSize::Millimeter));
    printer->setFullPage(true);
    printer->setPageMargins(QMarginsF(0, 0, 0, 0), QPageLayout::Millimeter);
}

ReportStruct PrintPreviewDialog::reportSet(const ReportConfig &config, const QJsonObject &data)
{
    ReportStruct temp(Printer_Type::General, config.mode, !config.samepage, "", config.title, data);
    temp.departmentName = "科室";
    temp.idName = "编号";
    return temp;
}

void PrintPreviewDialog::paintRequestedSlot(QPrinter *printer)
{
    ReportPainter painter(reportSet(m_config, m_data), printer);
}
