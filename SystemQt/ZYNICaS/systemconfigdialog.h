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
#include <QButtonGroup>

#include "reportset.h"
#include "getiddialog.h"

struct HospitalInfo
{
    QString hospitalName;
    QString roomName;
    QString doctorName;
    QString consultationHospitalName;
    Printer_Type pType;
    bool tip;
    bool samePage;
    QString trendChartTitle;
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
    void logoutSlot();
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
    QLabel *consultationHospitalLabel;
    QLineEdit *hospitalNameLineEdit;
    QLineEdit *roomNameLineEdit;
    QLineEdit *doctorNameLineEdit;
    QLineEdit *consultationHospitalLineEdit;
    QGroupBox *titleGroupBox;
    QButtonGroup *roomButtonGroup;
    QRadioButton *roomRadio;
    QRadioButton *execRoomRadio;
    QButtonGroup *idButtonGroup;
    QRadioButton *mNumRadio;
    QRadioButton *oNumRadio;
    QRadioButton *idRadio;
    QGroupBox *reportGroupBox;
    QButtonGroup *printerButtonGroup;
    QRadioButton *printerRadio;
    QRadioButton *xprinterRadio;
    QCheckBox *tipCheckBox;
    QCheckBox *samePageCheckBox;
    QLabel *trendChartPageTitleLabel;
    QLineEdit *trendChartPageTitleLineEdit;
    QPushButton *selectLogoBtn;
    QLabel *logoLabel;
    QGroupBox *checkModeGroupBox;
    QButtonGroup *modeButtonGroup;
    QRadioButton *generalModeRadio;
    QRadioButton *professionalModeRadio;
    QRadioButton *icuModeRadio;
    QRadioButton *healthCheckModeRadio;
    QGroupBox *systemInfoGroupBox;
    QLabel *serialPortLabel;
    QComboBox *serialPortComboBox;
    QPushButton *getIDBtn;
    QPushButton *confirmBtn;
    QString infoFileName;
    GetIdDialog *getIdDialog;
    QGroupBox *appMsgGroupBox;
//    QPushButton *anotherSetBtn;
    QPushButton *logoutBtn;
    QPushButton *aboutAppBtn;
};

#endif // SYSTEMCONFIGDIALOG_H
