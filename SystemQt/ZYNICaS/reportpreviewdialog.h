#ifndef REPORTPREVIEWDIALOG_H
#define REPORTPREVIEWDIALOG_H

#include <QPrintPreviewDialog>
#include <QPrinter>
#include <QJsonObject>

struct HospitalInfo;
class ReportPreviewDialog : public QPrintPreviewDialog
{
    Q_OBJECT
public:
    ReportPreviewDialog(const QJsonObject &data, HospitalInfo *info, QPrinter *printer, QWidget *parent = nullptr);
private slots:
    void paintRequestedSlot(QPrinter *printer);
private:
    QJsonObject m_data;
    HospitalInfo *m_info;
};

#endif // REPORTPREVIEWDIALOG_H
