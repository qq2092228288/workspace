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
    QString mac;
    QString place1Id = "383dd0778add4b848e03af84868930a1";
    QString place2Id = "b1989c5c5548473f86253da574450362";
    QString deviceId = "a18a028bca8b464b9006947d631b180a";
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
