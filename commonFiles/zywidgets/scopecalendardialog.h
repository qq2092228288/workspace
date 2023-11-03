#ifndef SCOPECALENDARDIALOG_H
#define SCOPECALENDARDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QCalendarWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>

class ScopeCalendarDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ScopeCalendarDialog(QWidget *parent = nullptr);
private slots:
    void setLabel();
    void confirmSlot();
signals:
    void scopeTime(qint64 start, qint64 end);
private:
    QLabel *label;
    QCalendarWidget *startWidget;
    QCalendarWidget *endWidget;
    QPushButton *confirmButton;
    QPushButton *cancelButton;
};

#endif // SCOPECALENDARDIALOG_H
