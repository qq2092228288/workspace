#ifndef PRINTPREVIEWDIALOG_H
#define PRINTPREVIEWDIALOG_H

#include <QPrintPreviewDialog>
#include <QPrinter>
#include <QJsonObject>
#include "reportpainter.h"
#include "reportset.h"

struct ReportConfig
{
    ReportConfig(bool _samepage = true, Check_Mode _mode = Check_Mode::Hypertension, QString _title = nullptr)
        :samepage(_samepage), mode(_mode), title(_title) {}
    bool samepage;
    Check_Mode mode;
    QString title;
};

class PrintPreviewDialog : public QPrintPreviewDialog
{
    Q_OBJECT
public:
    explicit PrintPreviewDialog(const ReportConfig &config, const QJsonObject &data,
                                QPrinter *printer, QWidget *parent = nullptr);
    static void initPrinter(QPrinter *printer);
    static ReportStruct reportSet(const ReportConfig &config, const QJsonObject &data);
private slots:
    void paintRequestedSlot(QPrinter *printer);
private:
    ReportConfig m_config;
    QJsonObject m_data;
};

#endif // PRINTPREVIEWDIALOG_H
