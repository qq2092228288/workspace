#ifndef SYSTEMCONFIGDIALOG_H
#define SYSTEMCONFIGDIALOG_H

#include <QDialog>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QGridLayout>
#include <QMessageBox>
#include <QGroupBox>
#include <QComboBox>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QSet>
#include <QSharedPointer>
#include <QFile>
#include <QTextStream>
#include <QTextCodec>
#include <QCheckBox>
#include <QRadioButton>

#include "getiddialog.h"

struct HospitalInfo
{
    QString hospitalName;
    QString roomName;
    QString doctorName;
    bool xprinter;
    bool tip;
    bool professional;
};

class SystemConfigDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SystemConfigDialog(QWidget *parent = nullptr);
    QString getPortName() const;
public slots:
    void confirmSlot();
protected:
    void updateHospitalInfo();
    void testFunc();
private:
    HospitalInfo hospitalInfo;
    QGroupBox *hospitalInfoGroupBox;
    QLabel *hospitalNameLabel;
    QLabel *roomNameLabel;
    QLabel *doctorNameLabel;
    QLineEdit *hospitalNameLineEdit;
    QLineEdit *roomNameLineEdit;
    QLineEdit *doctorNameLineEdit;
    QGroupBox *reportGroupBox;
    QRadioButton *printerRadio;
    QRadioButton *xprinterRadio;
    QCheckBox *tipCheckBox;
    QGroupBox *checkModeGroupBox;
    QRadioButton *generalModeRadio;
    QRadioButton *professionalModeRadio;
    QGroupBox *systemInfoGroupBox;
    QLabel *serialPortLabel;
    QComboBox *serialPortComboBox;
    QPushButton *getIDBtn;
    QPushButton *confirmBtn;
    QString infoFileName;
    GetIdDialog *getIdDialog;
};

#endif // SYSTEMCONFIGDIALOG_H
