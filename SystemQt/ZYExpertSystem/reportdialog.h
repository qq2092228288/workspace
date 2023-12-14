#ifndef REPORTDIALOG_H
#define REPORTDIALOG_H

#include <QDialog>
#include <QPrinter>
#include "reporttablemodel.h"
#include "scopecalendardialog.h"
#include "printpreviewdialog.h"
#include "waitingdialog.h"

namespace Ui {
class ReportDialog;
}

class ReportDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ReportDialog(QWidget *parent = nullptr);
    virtual ~ReportDialog();
    void setConfig(const ReportConfig &config);
private slots:
    void currentIndexChanged(int index);
    void searchButtonClicked();
    void resetButtonClicked();
    void reportPulled();
    void generatePdfButtonClicked();
    void openReportDirButtonClicked();
    void printPreviewButtonClicked();
    void reportTableViewDoubleClicked(const QModelIndex &index);
private:
    QDateTime selectedTime(const QModelIndex &index) const;
    QJsonObject getReportJson(const QModelIndex &index);
    bool indexIsValid(const QModelIndex &index);
    QVector<ReportModelItem> getItems();
    QString timeFormat() const;
private:
    Ui::ReportDialog *ui;
    const QString m_path;
    ScopeCalendarDialog *calendarDialog;
    ReportTableModel *model;
    ReportConfig m_config;
    WaitingDialog *waitingDialog;
};

#endif // REPORTDIALOG_H
