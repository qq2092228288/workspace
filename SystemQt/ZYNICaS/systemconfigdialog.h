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

enum Printer_Type
{
    General,    // A4纸
    Thermal     // xprinter热敏纸
};

enum Check_Mode
{
    Hypertension,           // 高血压模式
    InternalMedicine,       // 内科模式
    Critical,               // 重症模式
    PhysicalExamination     // 体检模式
};

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
    Printer_Type pType;
    bool tip;
    Check_Mode cMode;
};

class SystemConfigDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SystemConfigDialog(QWidget *parent = nullptr);
    QString getPortName() const;
    void updateHospitalInfo();
protected slots:
    void aboutAppSlot();
    void anotherSetSlot();
    void selectLogoSlot();
protected:
    void closeEvent(QCloseEvent *event);
    bool eventFilter(QObject *obj, QEvent *event);
    void setHospitalLogo();
signals:
    void modeChanged(Check_Mode);
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
    QPushButton *selectLogoBtn;
    QLabel *logoLabel;
    QGroupBox *checkModeGroupBox;
    QButtonGroup *modeButtonGroup;
    QRadioButton *generalModeRadio;
    QRadioButton *professionalModeRadio;
    QRadioButton *criticalModeRadio;
    QRadioButton *healthCheckModeRadio;
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
