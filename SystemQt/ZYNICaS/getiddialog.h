#ifndef GETIDDIALOG_H
#define GETIDDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QGridLayout>
#include <QNetworkInterface>
#include <QClipboard>
#include <QMessageBox>

class GetIdDialog : public QDialog
{
    Q_OBJECT
public:
    explicit GetIdDialog(QWidget *parent = nullptr);
    QString getMac() const;
signals:
private slots:
    void setConsumables(const int &count);
    void copySlot();
    void usedSlot();
private:
    QLabel *consumablesLabel;
    QLabel *macLabel;
    QLabel *codeLabel;
    QLineEdit *consumablesEdit;
    QLineEdit *macEdit;
    QLineEdit *codeEdit;
    QPushButton *copyBtn;
    QPushButton *usedBtn;
};

#endif // GETIDDIALOG_H
