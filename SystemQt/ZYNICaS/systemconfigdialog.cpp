#include "systemconfigdialog.h"
#include "datamanagement.h"
#include "anothersetdialog.h"

SystemConfigDialog::SystemConfigDialog(QWidget *parent)
    : QDialog{parent}
{
    auto &instance = DataManagement::getInstance();
    setMinimumSize(500*instance.wZoom(), 800*instance.hZoom());
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
    printerButtonGroup = new QButtonGroup(this);
    printerRadio = new QRadioButton(tr("常规打印机报告"), this);
    xprinterRadio = new QRadioButton(tr("热敏打印机报告"), this);
    tipCheckBox = new QCheckBox(tr("高风险人群提示"), this);
    samePageCheckBox = new QCheckBox(tr("双体位同页"), this);
    selectLogoBtn = new QPushButton(tr("选择logo"), this);
    logoLabel = new QLabel(this);
    checkModeGroupBox = new QGroupBox(tr("模式配置"), this);
    modeButtonGroup = new QButtonGroup(this);
    generalModeRadio = new QRadioButton(tr("高血压模式"), this);
    professionalModeRadio = new QRadioButton(tr("内科模式"), this);
    icuModeRadio = new QRadioButton(tr("重症模式"), this);
    healthCheckModeRadio = new QRadioButton(tr("体检模式"), this);
    systemInfoGroupBox = new QGroupBox(tr("系统配置"), this);
    serialPortLabel = new QLabel(tr("串口设置"), this);
    serialPortComboBox = new QComboBox(this);
    getIDBtn = new QPushButton(tr("有效验证码"), this);
    confirmBtn = new QPushButton(tr("确定"), this);
    getIdDialog = new GetIdDialog(this);
    appMsgGroupBox = new QGroupBox(this);
//    anotherSetBtn = new QPushButton(tr("其它设置"), this);
    logoutBtn = new QPushButton(tr("退出登录"), this);
    aboutAppBtn = new QPushButton(tr("关于"), this);

    printerButtonGroup->addButton(printerRadio, 0);
    printerButtonGroup->addButton(xprinterRadio, 1);
    modeButtonGroup->addButton(generalModeRadio, Check_Mode::Hypertension);
    modeButtonGroup->addButton(professionalModeRadio, Check_Mode::InternalMedicine);
    modeButtonGroup->addButton(icuModeRadio, Check_Mode::IntensiveCareUnit);
    modeButtonGroup->addButton(healthCheckModeRadio, Check_Mode::PhysicalExamination);

    serialPortComboBox->setFixedWidth(120*instance.wZoom());
    printerRadio->setChecked(true);
    generalModeRadio->setChecked(true);

    QGridLayout *mainLayout = new QGridLayout(this);

    mainLayout->addWidget(hospitalInfoGroupBox, 0, 0);
    mainLayout->addWidget(reportGroupBox, 1, 0);
    mainLayout->addWidget(checkModeGroupBox, 2, 0);
    mainLayout->addWidget(systemInfoGroupBox, 3, 0);
    mainLayout->addWidget(appMsgGroupBox, 4, 0);
    mainLayout->addWidget(confirmBtn, 5, 0);

    mainLayout->setRowStretch(0, 4);
    mainLayout->setRowStretch(1, 1);
    mainLayout->setRowStretch(2, 1);
    mainLayout->setRowStretch(3, 1);
    mainLayout->setRowStretch(4, 1);
    mainLayout->setRowStretch(5, 1);

    QGridLayout *hLayout = new QGridLayout(hospitalInfoGroupBox);
    QGridLayout *rLayout = new QGridLayout(reportGroupBox);
    QGridLayout *cLayout = new QGridLayout(checkModeGroupBox);
    QHBoxLayout *sLayout = new QHBoxLayout(systemInfoGroupBox);
    QGridLayout *aLayout = new QGridLayout(appMsgGroupBox);

    hLayout->addWidget(hospitalNameLabel, 0, 0);
    hLayout->addWidget(hospitalNameLineEdit, 0, 1);
    hLayout->addWidget(roomNameLabel, 1, 0);
    hLayout->addWidget(roomNameLineEdit, 1, 1);
    hLayout->addWidget(doctorNameLabel, 2, 0);
    hLayout->addWidget(doctorNameLineEdit, 2, 1);

    rLayout->addWidget(printerRadio, 0, 0, 1, 2);
    rLayout->addWidget(xprinterRadio, 0, 2, 1, 2);
    rLayout->addWidget(tipCheckBox, 1, 0, 1, 2);
    rLayout->addWidget(samePageCheckBox, 1, 2, 1, 2);
    rLayout->addWidget(selectLogoBtn, 2, 0, Qt::AlignLeft);
    rLayout->addWidget(logoLabel, 2, 1, Qt::AlignLeft);

    cLayout->addWidget(generalModeRadio, 0, 0);
    cLayout->addWidget(professionalModeRadio, 0, 1);
    cLayout->addWidget(icuModeRadio, 1, 0);
    cLayout->addWidget(healthCheckModeRadio, 1, 1);

    sLayout->addWidget(serialPortLabel);
    sLayout->addWidget(serialPortComboBox);
    sLayout->addStretch();
    sLayout->addWidget(getIDBtn, 0, Qt::AlignRight);

//    aLayout->addWidget(anotherSetBtn, 0, 0);
    aLayout->addWidget(logoutBtn, 0, 0, Qt::AlignLeft);
    aLayout->addWidget(aboutAppBtn, 0, 1, Qt::AlignRight);

    logoLabel->setToolTip(tr("双击删除"));
    logoLabel->installEventFilter(this);

    // 隐藏其它设置按钮
//    anotherSetBtn->hide();

    QStringList portNames;
    for (int var = 1; var <= 20; ++var) {
        portNames<<QString("COM%1").arg(var);
    }
    serialPortComboBox->addItems(portNames);

    connect(selectLogoBtn, &QPushButton::clicked, this, &SystemConfigDialog::selectLogoSlot);
    connect(getIDBtn, &QPushButton::clicked, getIdDialog, &GetIdDialog::exec);
//    connect(anotherSetBtn, &QPushButton::clicked, this, &SystemConfigDialog::anotherSetSlot);
    connect(logoutBtn, &QPushButton::clicked, this, &SystemConfigDialog::logoutSlot);
    connect(aboutAppBtn, &QPushButton::clicked, this, &SystemConfigDialog::aboutAppSlot);
    connect(confirmBtn, &QPushButton::clicked, this, &SystemConfigDialog::close);

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
        in<<QString("samepage=\"0\"\n");
        in<<QString("mode=\"0\"\n");
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
                    auto btn = printerButtonGroup->button(value.toInt());
                    if (nullptr != btn) {
                        btn->setChecked(true);
                    }
                    else {
                        printerButtonGroup->button(0)->setChecked(true);
                    }
                }
                else if (name == "tip") {
                    tipCheckBox->setChecked(value.toInt());
                }
                else if (name == "samepage") {
                    samePageCheckBox->setChecked(value.toInt());
                }
                else if (name == "mode") {
                    auto btn = modeButtonGroup->button(value.toInt());
                    if (nullptr != btn) {
                        btn->setChecked(true);
                    }
                    else {
                        modeButtonGroup->button(0)->setChecked(true);
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
    setHospitalLogo();
}

QString SystemConfigDialog::getPortName() const
{
    return serialPortComboBox->currentText();
}

void SystemConfigDialog::updateHospitalInfo()
{
//    auto database = DataManagement::getInstance().deviceDatabase();
//    QString place1Name = database->getDeviceInfo("place1Name");
//    QString place2Name = database->getDeviceInfo("place2Name");
//    hospitalNameLineEdit->setText(place1Name);
//    roomNameLineEdit->setText(place2Name);
    hospitalInfo.hospitalName = hospitalNameLineEdit->text();
    hospitalInfo.roomName = roomNameLineEdit->text();
//    hospitalInfo.place1Name = database->getDeviceInfo("place1Name");
//    hospitalInfo.place2Name = database->getDeviceInfo("place2Name");
    hospitalInfo.doctorName = doctorNameLineEdit->text();
//    hospitalInfo.place1Id = database->getDeviceInfo("place1Id");
//    hospitalInfo.place2Id = database->getDeviceInfo("place2Id");
//    hospitalInfo.deviceId = database->getDeviceInfo("deviceId");
    hospitalInfo.pType = Printer_Type(printerButtonGroup->checkedId());
    hospitalInfo.tip = tipCheckBox->isChecked();
    hospitalInfo.samePage = samePageCheckBox->isChecked();
    hospitalInfo.cMode = Check_Mode(modeButtonGroup->checkedId());
    DataManagement::getInstance().setHospitalInfo(&hospitalInfo);
    emit modeChanged(Check_Mode(modeButtonGroup->checkedId()));
}

void SystemConfigDialog::logoutSlot()
{
    if (QMessageBox::question(this, tr("提示"), tr("退出软件并重新登录？")) == QMessageBox::Yes) {
        DataManagement::getInstance().mqttClient()->logout();
        exit(0);
    }
}

void SystemConfigDialog::aboutAppSlot()
{
    QDialog dialog;
    dialog.setWindowTitle(tr("关于"));
    QVBoxLayout *vLayout = new QVBoxLayout(&dialog);
    QTextBrowser *textBrowser = new QTextBrowser(&dialog);
    textBrowser->setStyleSheet("background:transparent;border-width:0;border-style:outset");
    vLayout->addWidget(textBrowser);
    textBrowser->append(tr("软件版本: %1\n").arg(QCoreApplication::applicationVersion()));
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
//                else if ("ProductVersion" == regExp.cap(1)) {
//                    textBrowser->append(tr("软件版本: %1\n").arg(regExp.cap(2)));
//                }
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

void SystemConfigDialog::selectLogoSlot()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("选择图片"),
                                                    "/home",
                                                    tr("Images (*.png *.xpm *.jpg *.jpeg *.bmp)"));
    if (!fileName.isEmpty()) {
        QImage logo(fileName);
        int len = logo.height();
        if (logo.height() > logo.width()) {
            len = width();
        }
        logo.scaled(len, len, Qt::IgnoreAspectRatio, Qt::SmoothTransformation)
                .save(DataManagement::getInstance().getPaths().hospitalLogo(), "PNG");
        setHospitalLogo();
    }
}

void SystemConfigDialog::closeEvent(QCloseEvent *event)
{
    event->accept();
    //保存配置
    QFile file(infoFileName);
    if(file.open(QFile::WriteOnly)){
        QTextStream in(&file);
        in.setCodec(QTextCodec::codecForName("utf-8"));
        in<<QString("hospital=\"%1\"\n").arg(hospitalNameLineEdit->text());
        in<<QString("department=\"%1\"\n").arg(roomNameLineEdit->text());
        in<<QString("doctor=\"%1\"\n").arg(doctorNameLineEdit->text());
        in<<QString("printer=\"%1\"\n").arg(printerButtonGroup->checkedId());
        in<<QString("tip=\"%1\"\n").arg(tipCheckBox->isChecked());
        in<<QString("samepage=\"%1\"\n").arg(samePageCheckBox->isChecked());
        in<<QString("mode=\"%1\"\n").arg(modeButtonGroup->checkedId());
        in<<QString("serialport=\"%1\"\n").arg(serialPortComboBox->currentText());
        file.close();
    }
    updateHospitalInfo();
}

bool SystemConfigDialog::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonDblClick) {
        QString logoFileName = DataManagement::getInstance().getPaths().hospitalLogo();
        if (QFile::exists(logoFileName)) {
            if (QMessageBox::question(this, tr("提示"), tr("确定删除logo吗？")) == QMessageBox::Yes) {
                QFile::remove(logoFileName);
                logoLabel->setPixmap(QPixmap());
            }
        }
        else {
            QMessageBox::information(this, tr("提示"), tr("未设置logo"));
        }
    }
    return QDialog::eventFilter(obj, event);
}

void SystemConfigDialog::setHospitalLogo()
{
    QPixmap logo(DataManagement::getInstance().getPaths().hospitalLogo());
    if (!logo.isNull()) {
        logoLabel->setPixmap(logo.scaled(logoLabel->height(), logoLabel->height(),
                                         Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    }
}
