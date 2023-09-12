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
    void printPreviewSlot();
    void tableDoubleCilicked(const QModelIndex &index);
private:
    QVector<ReportModelItem> getItems() const;
private:
    QLabel *searchLabel;
    QComboBox *comboBox;
    QLineEdit *lineEdit;
    QPushButton *searchButton;
    QPushButton *resettingButton;
    QPushButton *printPreviewButton;
    ScopeCalendarDialog *calendarDialog;
    QTableView *tableView;
    ReportTableModel *model;
};

#endif // VIEWREPORTDIALOG_H
