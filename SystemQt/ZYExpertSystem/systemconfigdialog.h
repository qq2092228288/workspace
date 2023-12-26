#ifndef SYSTEMCONFIGDIALOG_H
#define SYSTEMCONFIGDIALOG_H

#include <QDialog>
#include "printpreviewdialog.h"

namespace Ui {
class SystemConfigDialog;
}

class SystemConfigDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SystemConfigDialog(QWidget *parent = nullptr);
    virtual ~SystemConfigDialog();
    ReportConfig config();
signals:

private slots:
    void confirmButtonClicked();
private:
    Ui::SystemConfigDialog *ui;
    QString m_fileName;
};

#endif // SYSTEMCONFIGDIALOG_H
