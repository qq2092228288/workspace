#ifndef ANOTHERSETDIALOG_H
#define ANOTHERSETDIALOG_H

#include <QDialog>
#include <QVBoxLayout>
#include <QSettings>
#include <QCheckBox>
#include <QApplication>
#include <QDebug>

//开机启动的注册表
#define AUTO_RUN "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run"

class AnotherSetDialog : public QDialog
{
    Q_OBJECT
public:
    explicit AnotherSetDialog(QWidget *parent = nullptr);

signals:
private slots:
    void autoStartSlot(int state);
private:
    QCheckBox *autoStartCheckBox;
    QSettings *m_pSettings;
};

#endif // ANOTHERSETDIALOG_H
