#ifndef REPORTCONFIGDIALOG_H
#define REPORTCONFIGDIALOG_H

#include <QDialog>
#include "printpreviewdialog.h"

namespace Ui {
class ReportConfigDialog;
}

class ReportConfigDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ReportConfigDialog(QWidget *parent = nullptr);
    virtual ~ReportConfigDialog();
    ReportConfig config();
signals:

private slots:
    void confirmButtonClicked();
private:
    Ui::ReportConfigDialog *ui;
    const QString m_fileName;
};

#endif // REPORTCONFIGDIALOG_H
