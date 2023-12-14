#ifndef WAITINGDIALOG_H
#define WAITINGDIALOG_H

#include <QDialog>

namespace Ui {
class WaitingDialog;
}

class WaitingDialog : public QDialog
{
    Q_OBJECT
public:
    explicit WaitingDialog(QWidget *parent = nullptr);
    virtual ~WaitingDialog();
public slots:
    void start(const int &total);
    void tryStop();
signals:
private:
    Ui::WaitingDialog *ui;
    int m_total;
    int m_current;
};

#endif // WAITINGDIALOG_H
