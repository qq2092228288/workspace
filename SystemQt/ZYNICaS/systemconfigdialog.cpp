#include "systemconfigdialog.h"
#include "datamanagement.h"
#include "anothersetdialog.h"

SystemConfigDialog::SystemConfigDialog(QWidget *parent)
    : QDialog{parent}
{
    auto &instance = DataManagement::getInstance();
    resize(500*instance.wZoom(), 800*instance.hZoom());
    this->setWindowTitle(tr("系统配置"));
    setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
    infoFileName = instance.getPaths().baseInfo();
    //样式表
    this->setStyleSheet(instance.dialogQss(1.2));

    hospitalInfoGroupBox = new QGroupBox(tr("信息配置"),this);
    hospitalNameLabel = new QLabel(tr("医院"), this);
    roomNameLabel = new QLabel(instance.departmentName(), this);
    doctorNameLabel = new QLabel(tr("检查人员"), this);
    consultationHospitalLabel = new QLabel(tr("会诊医院"), this);
    hospitalNameLineEdit = new QLineEdit(this);
    roomNameLineEdit = new QLineEdit(this);
    doctorNameLineEdit = new QLineEdit(this);
    consultationHospitalLineEdit = new QLineEdit(this);
    titleGroupBox = new QGroupBox(tr("名称配置"), this);
    roomButtonGroup = new QButtonGroup(this);
    roomRadio = new QRadioButton(tr("科室"), this);
    execRoomRadio = new QRadioButton(tr("执行科室"), this);
    idButtonGroup = new QButtonGroup(this);
    mNumRadio = new QRadioButton(tr("病历号"), this);
    oNumRadio = new QRadioButton(tr("门诊号"), this);
    idRadio = new QRadioButton(tr("编号"), this);
    reportGroupBox = new QGroupBox(tr("报告配置"), this);
    printerButtonGroup = new QButtonGroup(this);
    printerRadio = new QRadioButton(tr("常规打印机报告"), this);
    xprinterRadio = new QRadioButton(tr("热敏打印机报告"), this);
    tipCheckBox = new QCheckBox(tr("高风险人群提示"), this);
    samePageCheckBox = new QCheckBox(tr("双体位同页"), this);
    trendChartPageTitleLabel = new QLabel(tr("趋势图页标题"), this);
    trendChartPageTitleLineEdit = new QLineEdit(this);
    selectLogoBtn = new QPushButton(tr("选择logo"), this);
    logoLabel = new QLabel(this);
    checkModeGroupBox = new QGroupBox(tr("模式配置"), this);
    modeButtonGroup = new QButtonGroup(this);
    generalModeRadio = new QRadioButton(tr("高血压模式"), this);
    professionalModeRadio = new QRadioButton(tr("内科模式"), this);
    icuModeRadio = new QRadioButton(tr("重症模式"), this);
    healthCheckModeRadio = new QRadioButton(tr("体检模式"), this);
    healthModeRadio = new QRadioButton(tr("健康模式"), this);
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

    roomButtonGroup->addButton(roomRadio, 0);
    roomButtonGroup->addButton(execRoomRadio, 1);
    idButtonGroup->addButton(mNumRadio, 0);
    idButtonGroup->addButton(oNumRadio, 1);
    idButtonGroup->addButton(idRadio, 2);
    printerButtonGroup->addButton(printerRadio, 0);
    printerButtonGroup->addButton(xprinterRadio, 1);
    modeButtonGroup->addButton(generalModeRadio, Check_Mode::Hypertension);
    modeButtonGroup->addButton(professionalModeRadio, Check_Mode::InternalMedicine);
    modeButtonGroup->addButton(icuModeRadio, Check_Mode::IntensiveCareUnit);
    modeButtonGroup->addButton(healthCheckModeRadio, Check_Mode::PhysicalExamination);
    modeButtonGroup->addButton(healthModeRadio, Check_Mode::Health);

    serialPortComboBox->setFixedWidth(120*instance.wZoom());
    roomRadio->setChecked(true);
    mNumRadio->setChecked(true);
    printerRadio->setChecked(true);
    generalModeRadio->setChecked(true);

    QGridLayout *mainLayout = new QGridLayout(this);

    mainLayout->addWidget(hospitalInfoGroupBox, 0, 0);
    mainLayout->addWidget(titleGroupBox, 1, 0);
    mainLayout->addWidget(reportGroupBox, 2, 0);
    mainLayout->addWidget(checkModeGroupBox, 3, 0);
    mainLayout->addWidget(systemInfoGroupBox, 4, 0);
    mainLayout->addWidget(appMsgGroupBox, 5, 0);
    mainLayout->addWidget(confirmBtn, 6, 0);

    QGridLayout *hLayout = new QGridLayout(hospitalInfoGroupBox);
    QGridLayout *tLayout = new QGridLayout(titleGroupBox);
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
    hLayout->addWidget(consultationHospitalLabel, 3, 0);
    hLayout->addWidget(consultationHospitalLineEdit, 3, 1);

    tLayout->addWidget(roomRadio, 0, 0);
    tLayout->addWidget(execRoomRadio, 0, 1);
    tLayout->addWidget(mNumRadio, 1, 0);
    tLayout->addWidget(oNumRadio, 1, 1);
    tLayout->addWidget(idRadio, 1, 2);

    rLayout->addWidget(printerRadio, 0, 0, 1, 2);
    rLayout->addWidget(xprinterRadio, 0, 2, 1, 2);
    rLayout->addWidget(tipCheckBox, 1, 0, 1, 2);
    rLayout->addWidget(samePageCheckBox, 1, 2, 1, 2);
    rLayout->addWidget(trendChartPageTitleLabel, 2, 0);
    rLayout->addWidget(trendChartPageTitleLineEdit, 2, 1, 1, 2);
    rLayout->addWidget(selectLogoBtn, 3, 0, Qt::AlignLeft);
    rLayout->addWidget(logoLabel, 3, 1, Qt::AlignLeft);

    cLayout->addWidget(generalModeRadio, 0, 0);
    cLayout->addWidget(professionalModeRadio, 0, 1);
    cLayout->addWidget(icuModeRadio, 1, 0);
    cLayout->addWidget(healthCheckModeRadio, 1, 1);
    cLayout->addWidget(healthModeRadio, 2, 0);

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
#ifdef Q_OS_WIN
    QStringList portNames;
    for (int var = 1; var <= 20; ++var) {
        portNames<<QString("COM%1").arg(var);
    }
    serialPortComboBox->addItems(portNames);
#endif
#ifdef Q_OS_LINUX
    auto ports = QSerialPortInfo::availablePorts();
    foreach (auto info, ports) {
        serialPortComboBox->addItem(info.portName());
    }
#endif
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
        in<<QString("consultationhospital=\"\"\n");
        in<<QString("departmentname=\"科室\"\n");
        in<<QString("idname=\"病历号\"\n");
        in<<QString("printer=\"0\"\n");
        in<<QString("tip=\"0\"\n");
        in<<QString("samepage=\"1\"\n");
        in<<QString("trendcharttitle=\"心血流图监测报告\"\n");
        in<<QString("mode=\"0\"\n");
#ifdef Q_OS_WIN
        in<<QString("serialport=\"COM3\"\n");
#endif
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
                else if (name == "doctor") {
                    doctorNameLineEdit->setText(value);
                }
                else if (name == "consultationhospital") {
                    consultationHospitalLineEdit->setText(value);
                }
                else if (name == "departmentname") {
                    if (execRoomRadio->text() == QString::fromStdString(value.toStdString())) {
                        execRoomRadio->setChecked(true);
                    }
                    else {
                        roomRadio->setChecked(true);
                    }
                }
                else if (name == "idname") {
                    if (idRadio->text() == QString::fromStdString(value.toStdString())) {
                        idRadio->setChecked(true);
                    }
                    else if (oNumRadio->text() == QString::fromStdString(value.toStdString())) {
                        oNumRadio->setChecked(true);
                    }
                    else {
                        mNumRadio->setChecked(true);
                    }
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
                else if (name == "trendcharttitle") {
                    trendChartPageTitleLineEdit->setText(value);
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
    hospitalInfo.hospitalName = hospitalNameLineEdit->text();
    hospitalInfo.roomName = roomNameLineEdit->text();
    hospitalInfo.doctorName = doctorNameLineEdit->text();
    hospitalInfo.consultationHospitalName = consultationHospitalLineEdit->text();
    hospitalInfo.pType = Printer_Type(printerButtonGroup->checkedId());
    hospitalInfo.tip = tipCheckBox->isChecked();
    hospitalInfo.samePage = samePageCheckBox->isChecked();
    hospitalInfo.trendChartTitle = trendChartPageTitleLineEdit->text();
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
    QDialog dialog(this);
    dialog.setWindowFlag(Qt::WindowContextHelpButtonHint, false);
    dialog.setWindowTitle(tr("关于"));
    QVBoxLayout *vLayout = new QVBoxLayout(&dialog);
    auto infoLabel = new QLabel(&dialog);
    vLayout->addWidget(infoLabel);
    infoLabel->setOpenExternalLinks(true);
    auto str = QString::fromUtf8("<p style='%1'>软件版本：%2</p>"
                                 "<p style='%1'>软件名称：无创血流动力学检测系统</p>"
                                 "<p style='%1'>公司名称：长沙芫泽医疗器械有限公司</p>"
                                 "<p style='%1'>"
                                 "公司网址：<a href='%3' style='%1 color:blue; text-decoration:underline;'>%3</a>"
                                 "</p>")
            .arg(QString("font-size:18px;margin:30px;"),
                 QCoreApplication::applicationVersion(),
                 QCoreApplication::organizationDomain());
    infoLabel->setText(str);
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
        in<<QString("consultationhospital=\"%1\"\n").arg(consultationHospitalLineEdit->text());
        in<<QString("departmentname=\"%1\"\n").arg(roomButtonGroup->checkedButton()->text());
        in<<QString("idname=\"%1\"\n").arg(idButtonGroup->checkedButton()->text());
        in<<QString("printer=\"%1\"\n").arg(printerButtonGroup->checkedId());
        in<<QString("tip=\"%1\"\n").arg(tipCheckBox->isChecked());
        in<<QString("samepage=\"%1\"\n").arg(samePageCheckBox->isChecked());
        in<<QString("trendcharttitle=\"%1\"\n").arg(trendChartPageTitleLineEdit->text());
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
