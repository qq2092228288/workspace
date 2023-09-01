#include "reportdialog.h"

ReportDialog::ReportDialog(const ReportInfo &info, QWidget *parent)
    : QDialog{parent},
      m_info{info}
{
    setWindowTitle(tr("报告预览"));
    setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
}
