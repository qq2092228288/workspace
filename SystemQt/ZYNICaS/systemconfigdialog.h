#ifndef SYSTEMCONFIGDIALOG_H
#define SYSTEMCONFIGDIALOG_H

#pragma comment(lib, "version.lib")
#include <windows.h>
#include <tchar.h>

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
#include <QButtonGroup>


#include "getiddialog.h"



struct HospitalInfo
{
    QString hospitalName;
    QString roomName;
    QString doctorName;
    QString place1Name;
    QString place2Name;
    QString place1Id;
    QString place2Id;
    QString deviceId;
    int printer;
    bool tip;
    int mode;
};

class SystemConfigDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SystemConfigDialog(QWidget *parent = nullptr);
    QString getPortName() const;
protected slots:
    void confirmSlot();
    void aboutAppSlot();
    void anotherSetSlot();
protected:
    void updateHospitalInfo();
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
    QButtonGroup *printerButtonGroup;
    QRadioButton *printerRadio;
    QRadioButton *xprinterRadio;
    QCheckBox *tipCheckBox;
    QGroupBox *checkModeGroupBox;
    QButtonGroup *modeButtonGroup;
    QRadioButton *generalModeRadio;
    QRadioButton *professionalModeRadio;
    QRadioButton *hypertensionModeRadio;
    QGroupBox *systemInfoGroupBox;
    QLabel *serialPortLabel;
    QComboBox *serialPortComboBox;
    QPushButton *getIDBtn;
    QPushButton *confirmBtn;
    QString infoFileName;
    GetIdDialog *getIdDialog;
    QGroupBox *appMsgGroupBox;
    QPushButton *anotherSetBtn;
    QPushButton *aboutAppBtn;
};

#endif // SYSTEMCONFIGDIALOG_H
