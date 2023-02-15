#include "systemconfigdialog.h"
#include "datamanagement.h"


SystemConfigDialog::SystemConfigDialog(QWidget *parent)
    : QDialog{parent}
{
    auto &instance = DataManagement::getInstance();
    setMinimumWidth(500*instance.wZoom());
    this->setWindowTitle(tr("系统配置"));
    setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
    infoFileName = instance.getPaths().baseInfo();
    //样式表
    this->setStyleSheet(instance.dialogQss(1.2));

    hospitalInfoGroupBox = new QGroupBox(tr("信息配置"),this);
    hospitalNameLabel = new QLabel(tr("医院"), this);
    roomNameLabel = new QLabel(tr("科室"), this);
    doctorNameLabel = new QLabel(tr("检查人员"), this);
    hospitalNameLineEdit = new QLineEdit(this);
    roomNameLineEdit = new QLineEdit(this);
    doctorNameLineEdit = new QLineEdit(this);
    reportGroupBox = new QGroupBox(tr("报告配置"), this);
    printerRadio = new QRadioButton(tr("常规打印机报告"), this);
    xprinterRadio = new QRadioButton(tr("热敏打印机报告"), this);
    tipCheckBox = new QCheckBox(tr("心源性猝死提示"), this);
    checkModeGroupBox = new QGroupBox(tr("模式配置"), this);
    generalModeRadio = new QRadioButton(tr("常规模式"), this);
    professionalModeRadio = new QRadioButton(tr("专业模式"), this);
    systemInfoGroupBox = new QGroupBox(tr("系统配置"), this);
    serialPortLabel = new QLabel(tr("串口设置"), this);
    serialPortComboBox = new QComboBox(this);
    getIDBtn = new QPushButton(tr("有效验证码"), this);
    confirmBtn = new QPushButton(tr("确定"), this);
    getIdDialog = new GetIdDialog(this);
    appMsgGroupBox = new QGroupBox(this);
    anotherSetBtn = new QPushButton(tr("其它设置"), this);
    aboutAppBtn = new QPushButton(tr("关于"), this);

    serialPortComboBox->setFixedWidth(120*instance.wZoom());
    printerRadio->setChecked(true);
    generalModeRadio->setChecked(true);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QGridLayout *gLayout = new QGridLayout(hospitalInfoGroupBox);
    QVBoxLayout *rLayout = new QVBoxLayout(reportGroupBox);
    QHBoxLayout *cLayout = new QHBoxLayout(checkModeGroupBox);
    QHBoxLayout *hLayout = new QHBoxLayout(systemInfoGroupBox);
    QHBoxLayout *pLayout = new QHBoxLayout;
    QHBoxLayout *btnLayout = new QHBoxLayout;
    QHBoxLayout *appMsgLayout = new QHBoxLayout;

    hospitalInfoGroupBox->setFixedHeight(250*instance.hZoom());
    mainLayout->addWidget(hospitalInfoGroupBox);
    mainLayout->addWidget(reportGroupBox);
    mainLayout->addWidget(checkModeGroupBox);
    mainLayout->addWidget(systemInfoGroupBox);
    mainLayout->addWidget(appMsgGroupBox);
    mainLayout->addLayout(btnLayout);
    hospitalInfoGroupBox->setLayout(gLayout);
    reportGroupBox->setLayout(rLayout);
    systemInfoGroupBox->setLayout(hLayout);
    appMsgGroupBox->setLayout(appMsgLayout);
    gLayout->addWidget(hospitalNameLabel, 0, 0);
    gLayout->addWidget(hospitalNameLineEdit, 0, 1);
    gLayout->addWidget(roomNameLabel, 1, 0);
    gLayout->addWidget(roomNameLineEdit, 1, 1);
    gLayout->addWidget(doctorNameLabel, 2, 0);
    gLayout->addWidget(doctorNameLineEdit, 2, 1);
    rLayout->addLayout(pLayout);
    pLayout->addWidget(printerRadio);
    pLayout->addWidget(xprinterRadio);
    cLayout->addWidget(generalModeRadio);
    cLayout->addWidget(professionalModeRadio);
    rLayout->addWidget(tipCheckBox);
    hLayout->addWidget(serialPortLabel);
    hLayout->addWidget(serialPortComboBox);
    hLayout->addStretch();
    hLayout->addWidget(getIDBtn);
    appMsgLayout->addWidget(anotherSetBtn, 0, Qt::AlignLeft);
    appMsgLayout->addWidget(aboutAppBtn, 0, Qt::AlignRight);
    btnLayout->addWidget(confirmBtn);


    QStringList portNames;
    for (int var = 1; var <= 20; ++var) {
        portNames<<QString("COM%1").arg(var);
    }
    serialPortComboBox->addItems(portNames);

    connect(getIDBtn, &QPushButton::clicked, getIdDialog, &GetIdDialog::exec);
    connect(anotherSetBtn, &QPushButton::clicked, this, &SystemConfigDialog::anotherSetSlot);
    connect(aboutAppBtn, &QPushButton::clicked, this, &SystemConfigDialog::aboutAppSlot);
    connect(confirmBtn, &QPushButton::clicked, this, &SystemConfigDialog::confirmSlot);

    //读取配置
    QFile file(infoFileName);
    QFileInfo fileInfo(file);
    if(!fileInfo.isFile())
    {
        file.open(QFile::WriteOnly);
        QTextStream in(&file);
        in.setCodec(QTextCodec::codecForName("utf-8"));
        in<<QString("hospital=\"\"\n");
        in<<QString("department=\"\"\n");
        in<<QString("doctor=\"\"\n");
        in<<QString("printer=\"0\"\n");
        in<<QString("tip=\"0\"\n");
        in<<QString("professional=\"0\"\n");
        in<<QString("serialport=\"COM3\"\n");
        file.close();
    }
    if(file.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream out(&file);
        out.setCodec(QTextCodec::codecForName("utf-8"));

        QRegExp nameExp("(.*)(?:=)");
        QRegExp valueExp("(?:\")(.*)(?:\")");
        while (!out.atEnd()) {
            QString strLine = out.readLine();
            if (strLine.indexOf(nameExp) >= 0 && strLine.indexOf(valueExp) >= 0) {
                QString name = nameExp.cap(1);
                QString value = valueExp.cap(1);
                if (name == "hospital") {
                    hospitalNameLineEdit->setText(value);
                }
                else if (name == "department") {
                    roomNameLineEdit->setText(value);
                }
                if (name == "doctor") {
                    doctorNameLineEdit->setText(value);
                }
                else if (name == "printer") {
                    if (value == "1") {
                        xprinterRadio->setChecked(true);
                    }
                    else {
                        printerRadio->setChecked(true);
                    }
                }
                else if (name == "tip") {
                    if (value == "1") {
                        tipCheckBox->setChecked(true);
                    }
                    else {
                        tipCheckBox->setChecked(false);
                    }
                }
                else if (name == "professional") {
                    if (value == "1") {
                        professionalModeRadio->setChecked(true);
                    }
                    else {
                        generalModeRadio->setChecked(true);
                    }
                }
                else if (name == "serialport") {
                    if(-1 != serialPortComboBox->findText(value)) {
                        serialPortComboBox->setCurrentIndex(serialPortComboBox->findText(value));
                    }
                }
            }
        }
        file.close();
    }
    updateHospitalInfo();
}

QString SystemConfigDialog::getPortName() const
{
    return serialPortComboBox->currentText();
}

void SystemConfigDialog::confirmSlot()
{
    this->close();
    //保存配置
    QFile file(infoFileName);
    if(file.open(QFile::WriteOnly)){
        QTextStream in(&file);
        in.setCodec(QTextCodec::codecForName("utf-8"));
        in<<QString("hospital=\"%1\"\n").arg(hospitalNameLineEdit->text());
        in<<QString("department=\"%1\"\n").arg(roomNameLineEdit->text());
        in<<QString("doctor=\"%1\"\n").arg(doctorNameLineEdit->text());
        in<<QString("printer=\"%1\"\n").arg(xprinterRadio->isChecked());
        in<<QString("tip=\"%1\"\n").arg(tipCheckBox->isChecked());
        in<<QString("professional=\"%1\"\n").arg(professionalModeRadio->isChecked());
        in<<QString("serialport=\"%1\"\n").arg(serialPortComboBox->currentText());
        file.close();
    }
    updateHospitalInfo();
}

void SystemConfigDialog::aboutAppSlot()
{
    QDialog dialog;
    dialog.setWindowTitle(tr("关于"));
    QVBoxLayout *vLayout = new QVBoxLayout(&dialog);
    QTextBrowser *textBrowser = new QTextBrowser(&dialog);
    textBrowser->setStyleSheet("background:transparent;border-width:0;border-style:outset");
    vLayout->addWidget(textBrowser);
    // get app version info
    QFile file(":/resource.rc");
    if (file.open(QIODevice::ReadOnly)) {
        QTextStream out(&file);
        out.setCodec("GB2312");
        QRegExp regExp("(?:VALUE\")(.+)(?:\",\")(.+)(?:\")");
        while (!out.atEnd()) {
            QString strLine = out.readLine().remove(" ").remove("\t").remove("\\0");
            if (strLine.indexOf(regExp) >= 0) {
                if ("CompanyName" == regExp.cap(1)) {
                    textBrowser->append(tr("公司名称: %1\n").arg(regExp.cap(2)));
                }
                else if ("ProductName" == regExp.cap(1)) {
                    textBrowser->append(tr("软件名称: %1\n").arg(regExp.cap(2)));
                }
                else if ("ProductVersion" == regExp.cap(1)) {
                    textBrowser->append(tr("软件版本: %1\n").arg(regExp.cap(2)));
                }
                else if ("OrganizationDomain" == regExp.cap(1)) {
                    textBrowser->append(tr("公司网站: %1\n").arg(regExp.cap(2)));
                }
            }
        }
    }
    dialog.exec();
}

void SystemConfigDialog::anotherSetSlot()
{
    AnotherSetDialog dialog(this);
    dialog.exec();
}

void SystemConfigDialog::updateHospitalInfo()
{
    auto database = DataManagement::getInstance().deviceDatabase();
//    QString place1Name = database->getDeviceInfo("place1Name");
//    QString place2Name = database->getDeviceInfo("place2Name");
//    hospitalNameLineEdit->setText(place1Name);
//    roomNameLineEdit->setText(place2Name);
    hospitalInfo.hospitalName = hospitalNameLineEdit->text();
    hospitalInfo.roomName = roomNameLineEdit->text();
    hospitalInfo.place1Name = database->getDeviceInfo("place1Name");
    hospitalInfo.place2Name = database->getDeviceInfo("place2Name");
    hospitalInfo.doctorName = doctorNameLineEdit->text();
    hospitalInfo.place1Id = database->getDeviceInfo("place1Id");
    hospitalInfo.place2Id = database->getDeviceInfo("place2Id");
    hospitalInfo.deviceId = database->getDeviceInfo("deviceId");
    hospitalInfo.xprinter = xprinterRadio->isChecked();
    hospitalInfo.tip = tipCheckBox->isChecked();
    hospitalInfo.professional = professionalModeRadio->isChecked();
    DataManagement::getInstance().setHospitalInfo(&hospitalInfo);
}
