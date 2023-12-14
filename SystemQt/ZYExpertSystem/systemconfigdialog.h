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
    bool getLatest() const;
signals:

private slots:
    void confirmButtonClicked();
private:
    Ui::SystemConfigDialog *ui;
    const QString m_fileName;
};

#endif // SYSTEMCONFIGDIALOG_H
