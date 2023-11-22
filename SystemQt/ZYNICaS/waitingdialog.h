#ifndef WAITINGDIALOG_H
#define WAITINGDIALOG_H

#include <QDialog>
#include <QMovie>
#include <QLabel>
#include <QVBoxLayout>

class WaitingDialog : public QDialog
{
    Q_OBJECT
public:
    explicit WaitingDialog(QWidget *parent = nullptr);
    virtual ~WaitingDialog();
public slots:
    void setValue(const int &value, const int &total);
signals:
private:
    QVBoxLayout *mainLayout;
    QLabel *movieLabel;
    QLabel *textLabel;
    QMovie *m_movie;
};

#endif // WAITINGDIALOG_H
