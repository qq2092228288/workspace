#ifndef VIEWREPORTDIALOG_H
#define VIEWREPORTDIALOG_H

#include <QDialog>
#include <QTableView>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QCalendarWidget>
#include <QSqlDatabase>
#include "reporttablemodel.h"
#include "scopecalendardialog.h"

class ViewReportDialog : public QDialog
{
    Q_OBJECT
public:
    ViewReportDialog(QWidget *parent = nullptr);
protected:
    virtual void showEvent(QShowEvent *event);
private slots:
    void comboBoxActivatedSlot(int index);
    void scopeTimeSlot(qint64 start, qint64 end);
    void searchSlot();
    void resettingSlot();
    void pullingSlot();
    void pulledSlot(int state);
    void createdPdfSlot();
    void printPreviewSlot();
    void tableDoubleCilicked(const QModelIndex &index);
signals:
    void pulling(qint64 time);
private:
    QVector<ReportModelItem> getItems();
    QJsonObject getReportJson(const QModelIndex &index);
    QDateTime getReportTime(const QModelIndex &index);
    bool indexIsValid(const QModelIndex &index);
private:
    QSqlDatabase m_db;
    QLabel *searchLabel;
    QComboBox *comboBox;
    QLineEdit *lineEdit;
    QPushButton *searchButton;
    QPushButton *resettingButton;
    QPushButton *pdfButton;
    QPushButton *pullButton;
    QPushButton *printPreviewButton;
    ScopeCalendarDialog *calendarDialog;
    QTableView *tableView;
    ReportTableModel *model;
};

#endif // VIEWREPORTDIALOG_H
