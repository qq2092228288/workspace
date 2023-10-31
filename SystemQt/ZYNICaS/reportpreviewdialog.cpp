#include "reportpreviewdialog.h"
#include <QScreen>
#include "systemconfigdialog.h"
#include "datamanagement.h"
#include "reportpainter.h"

ReportPreviewDialog::ReportPreviewDialog(const QJsonObject &data, HospitalInfo *info, QPrinter *printer, QWidget *parent)
    : QPrintPreviewDialog{printer, parent},
      m_data{data},
      m_info{info}
{
    printer->setPageSize(QPageSize(info->pType == Printer_Type::Thermal ? QSizeF(72, 297) : QSizeF(210, 297),
                                   QPageSize::Millimeter));
    printer->setFullPage(true);
    printer->setPageMargins(QMarginsF(0, 0, 0, 0), QPageLayout::Millimeter);
    resize(screen()->availableSize());
    setWindowTitle("报告预览");
    setWindowFlags(Qt::Dialog | Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);
    connect(this, &QPrintPreviewDialog::paintRequested, this, &ReportPreviewDialog::paintRequestedSlot);
}

void ReportPreviewDialog::paintRequestedSlot(QPrinter *printer)
{
    auto &instance = DataManagement::getInstance();
    ReportStruct temp(m_info->pType, m_info->cMode, !m_info->samePage, instance.getPaths().hospitalLogo(),
                      instance.getHospitalInfo()->trendChartTitle, m_data);
    temp.departmentName = instance.departmentName();
    temp.idName = instance.idName();
    ReportPainter painter(temp, printer);
}
