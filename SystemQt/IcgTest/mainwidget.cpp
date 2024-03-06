#include "mainwidget.h"
#include "threadservice.h"
#include "waveform.h"

#include <QDebug>

MainWidget::MainWidget(QWidget *parent)
    : QWidget{parent}
{
    auto mainLayout = new QGridLayout(this);

    auto comBox = new QGroupBox(this);
    auto comLayout = new QHBoxLayout(comBox);
    auto comLabel = new QLabel("端口号:", this);
    auto openButton = new QPushButton("打开", this);
    auto closeButton = new QPushButton("关闭", this);
    auto comboBox = new QComboBox(this);

    comLayout->addWidget(comLabel);
    comLayout->addWidget(comboBox);
    comLayout->addWidget(openButton);
    comLayout->addWidget(closeButton);

    auto patientDataBox = new QGroupBox(this);
    auto patientDataLayout = new QGridLayout(patientDataBox);
    auto heightLabel = new QLabel("身高(cm):", this);
    auto weightLabel = new QLabel("体重(kg):", this);
    auto genderLabel = new QLabel("性别:", this);
    auto ageLabel = new QLabel("年龄(岁):", this);
    auto heightEdit = new QLineEdit(this);
    auto weightEdit = new QLineEdit(this);
    auto maleButton = new QRadioButton("男", this);
    auto femaleButton = new QRadioButton("女", this);
    auto ageEdit = new QLineEdit(this);
    auto startButton = new QPushButton("开始检测", this);
    auto stopButton = new QPushButton("停止检测", this);
    patientDataLayout->addWidget(heightLabel, 0, 0, 1, 1);
    patientDataLayout->addWidget(weightLabel, 1, 0, 1, 1);
    patientDataLayout->addWidget(genderLabel, 2, 0, 1, 1);
    patientDataLayout->addWidget(ageLabel, 3, 0, 1, 1);
    patientDataLayout->addWidget(heightEdit, 0, 1, 1, 2);
    patientDataLayout->addWidget(weightEdit, 1, 1, 1, 2);
    patientDataLayout->addWidget(maleButton, 2, 1, 1, 1);
    patientDataLayout->addWidget(femaleButton, 2, 2, 1, 1);
    patientDataLayout->addWidget(ageEdit, 3, 1, 1, 2);
    patientDataLayout->addWidget(startButton, 4, 1, 1, 1);
    patientDataLayout->addWidget(stopButton, 4, 2, 1, 1);

    auto bpBox = new QGroupBox(this);
    auto bpLayout = new QHBoxLayout(bpBox);
    auto sbpLabel = new QLabel("收缩压(mmHg):", this);
    auto dbpLabel = new QLabel("舒张压(mmHg):", this);
    auto sbpEdit = new QLineEdit(this);
    auto dbpEdit = new QLineEdit(this);
    auto setBpButton = new QPushButton("设置血压", this);
    bpLayout->addWidget(sbpLabel);
    bpLayout->addWidget(sbpEdit);
    bpLayout->addWidget(dbpLabel);
    bpLayout->addWidget(dbpEdit);
    bpLayout->addWidget(setBpButton);

    auto extBox = new QGroupBox(this);
    auto extLayout = new QGridLayout(extBox);
    auto cvpLabel = new QLabel("CVP:", this);
    auto pawpLabel = new QLabel("PAWP:", this);
    auto hbLabel = new QLabel("Hb:", this);
    auto spO2Label = new QLabel("SpO2:", this);
    auto cvpEdit = new QLineEdit(this);
    auto pawpEdit = new QLineEdit(this);
    auto hbEdit = new QLineEdit(this);
    auto spO2Edit = new QLineEdit(this);
    auto setExtButton = new QPushButton("设置扩展参数", this);
    extLayout->addWidget(cvpLabel, 0, 0, 1, 1);
    extLayout->addWidget(pawpLabel, 1, 0, 1, 1);
    extLayout->addWidget(hbLabel, 2, 0, 1, 1);
    extLayout->addWidget(spO2Label, 3, 0, 1, 1);
    extLayout->addWidget(cvpEdit, 0, 1, 1, 2);
    extLayout->addWidget(pawpEdit, 1, 1, 1, 2);
    extLayout->addWidget(hbEdit, 2, 1, 1, 2);
    extLayout->addWidget(spO2Edit, 3, 1, 1, 2);
    extLayout->addWidget(setExtButton, 3, 3, 1, 1);

    auto bsaAndVeptBox = new QGroupBox(this);
    auto bsaAndVeptLayout = new QGridLayout(bsaAndVeptBox);
    auto bsaButton = new QPushButton("获取BSA", this);
    auto veptButton = new QPushButton("获取VEPT", this);
    auto bsaEdit = new QLineEdit(this);
    auto veptEdit = new QLineEdit(this);
    bsaAndVeptLayout->addWidget(bsaButton, 0, 0, 1, 1);
    bsaAndVeptLayout->addWidget(bsaEdit, 0, 1, 1, 1);
    bsaAndVeptLayout->addWidget(veptButton, 0, 2, 1, 1);
    bsaAndVeptLayout->addWidget(veptEdit, 0, 3, 1, 1);

    auto commandBox = new QGroupBox(this);
    auto commandLayout = new QHBoxLayout(commandBox);
    auto commandLabel = new QLabel("自定义命令:", this);
    auto commandEdit = new QLineEdit(this);
    auto sendCommButton = new QPushButton("发送", this);
    commandLayout->addWidget(commandLabel);
    commandLayout->addWidget(commandEdit);
    commandLayout->addWidget(sendCommButton);

    auto modeBox = new QGroupBox(this);
    auto modeLayout = new QHBoxLayout(modeBox);
    auto normalButton = new QPushButton("正常模式", this);
    auto powerDownButton = new QPushButton("断电模式", this);
    modeLayout->addStretch();
    modeLayout->addWidget(normalButton);
    modeLayout->addWidget(powerDownButton);

    auto valueBox = new QGroupBox(this);
    auto valueLayout = new QGridLayout(valueBox);
    valueBox->setLayout(valueLayout);
    QStringList standard {"HR", "SV", "SVRI", "SI", "CO", "TFC", "SVR", "QI", "DO2I"};
    QVector<QLineEdit *> vect1, vect2;
    for (int i = 0; i < standard.count(); ++i) {
        auto label = new QLabel(standard.at(i) + ":", this);
        auto edit = new QLineEdit(this);
        valueLayout->addWidget(label, i, 0);
        valueLayout->addWidget(edit, i, 1);
        vect1.append(edit);
    }
    QStringList extended {"ACI", "VI", "PEP", "LVET", "TFCI", "STR", "CI", "LCW", "LCWI", "Z0"};
    for (int i = 0; i < extended.count(); ++i) {
        auto label = new QLabel(extended.at(i) + ":", this);
        auto edit = new QLineEdit(this);
        valueLayout->addWidget(label, i, 2);
        valueLayout->addWidget(edit, i, 3);
        vect2.append(edit);
    }
    auto browserBox = new QGroupBox(this);
    auto browserLayout = new QHBoxLayout(browserBox);
    auto browser = new QTextBrowser(this);
    browserLayout->addWidget(browser);

    auto waveformBox = new QGroupBox(this);
    auto waveformLayout = new QVBoxLayout(waveformBox);
    auto ecgWaveform = new Waveform("ECG", this);
    auto icgWaveform = new Waveform("ICG", this);
    waveformLayout->addWidget(ecgWaveform);
    waveformLayout->addWidget(icgWaveform);

    mainLayout->addWidget(comBox, 0, 0);
    mainLayout->addWidget(patientDataBox, 1, 0);
    mainLayout->addWidget(bpBox, 2, 0);
    mainLayout->addWidget(extBox, 3, 0);
    mainLayout->addWidget(bsaAndVeptBox, 4, 0);
    mainLayout->addWidget(commandBox, 5, 0);
    mainLayout->addWidget(modeBox, 6, 0);
    mainLayout->addWidget(valueBox, 0, 1, 4, 1);
    mainLayout->addWidget(waveformBox, 0, 2, 4, 1);
    mainLayout->addWidget(browserBox, 0, 3, 4, 1);


    mainLayout->setColumnStretch(0, 1);
    mainLayout->setColumnStretch(1, 1);
    mainLayout->setColumnStretch(2, 3);
    mainLayout->setColumnStretch(3, 2);

    module = new IcgControlModule;
    ThreadService::getInstance()->objectMoveToThread(module);
    qRegisterMetaType<IcgControlModule::ParameterType>("ParameterType");
    qRegisterMetaType<QVector<float>>("QVector<float>");

    comboBox->addItems(module->availablePorts());
    connect(openButton, &QPushButton::clicked, module, [=](){
        module->open(comboBox->currentText());
    });
    connect(closeButton, &QPushButton::clicked, module, &IcgControlModule::close);
    connect(startButton, &QPushButton::clicked, module, [=](){
        module->startMeasuring(heightEdit->text().toUInt(),
                               weightEdit->text().toUInt(),
                               IcgControlModule::Gender(femaleButton->isChecked()),
                               ageEdit->text().toUInt());
    });
    connect(startButton, &QPushButton::clicked, ecgWaveform, &Waveform::clear);
    connect(startButton, &QPushButton::clicked, icgWaveform, &Waveform::clear);
    connect(stopButton, &QPushButton::clicked, module, &IcgControlModule::stopMeasuring);
    connect(setBpButton, &QPushButton::clicked, module, [=](){
        module->setBp(sbpEdit->text().toUInt(), dbpEdit->text().toUInt());
    });
    connect(setExtButton, &QPushButton::clicked, module, [=](){
        module->setExtVal(cvpEdit->text().toUInt(),
                          pawpEdit->text().toUInt(),
                          hbEdit->text().toUInt(),
                          spO2Edit->text().toUInt());
    });
    connect(bsaButton, &QPushButton::clicked, module, &IcgControlModule::retrieveBsa);
    connect(veptButton, &QPushButton::clicked, module, &IcgControlModule::retrieveVept);
    connect(sendCommButton, &QPushButton::clicked, module, [=](){
        auto list = commandEdit->text().split(' ');
        QByteArray data;
        bool ok;
        foreach (auto str, list) {
            data.append(char(str.toInt(&ok, 16)));
        }
        module->userCommand(data);
    });
    connect(normalButton, &QPushButton::clicked, module, &IcgControlModule::normalMode);
    connect(powerDownButton, &QPushButton::clicked, module, &IcgControlModule::powerDownMode);
    connect(module, &IcgControlModule::ecg, ecgWaveform, &Waveform::newdata);
    connect(module, &IcgControlModule::icg, icgWaveform, &Waveform::newdata);
    connect(module, &IcgControlModule::data,
            this, [=](const IcgControlModule::ParameterType &type, const QVector<float> &vect){
        if (IcgControlModule::Standard == type) {
            for (int i = 0; i < vect.count(); ++i) {
                vect1.at(i)->setText(ntos(vect.at(i)));
            }
        }
        else {
            for (int i = 0; i < vect.count(); ++i) {
                vect2.at(i)->setText(ntos(vect.at(i)));
            }
        }
    });
    connect(module, &IcgControlModule::bsa, this, [=](const double &value){
        bsaEdit->setText(QString::number(value, 'f', 1));
    });
    connect(module, &IcgControlModule::vept, this, [=](const double &value){
        veptEdit->setText(QString::number(value, 'f', 1));
    });
    connect(module, &IcgControlModule::error, browser, [=](const char &ctrl, const uchar &err){
        auto text = QString("CONTROL: 0x%1; ERROR: 0x%2; MESSAGE: ")
                        .arg(ctrl, 2, 16, QLatin1Char('0')).arg(err, 2, 16, QLatin1Char('0'));
        switch (ctrl) {
        case 0x02:
            switch (err) {
            case 0x00:
                text += QString("Patient data set and measuring started.");
                break;
            case 0x01:
                text += QString("Invalid patient data.");
                break;
            case 0x02:
                text += QString("ICG hardware not ready or not available.");
                break;
            case 0x03:
                text += QString("Unsupported waveform sample rate.");
                break;
            case 0x04:
                text += QString("Unsupported protocol version.");
                break;
            case 0x05:
                text += QString("Invalid software serial number.");
                break;
            default:
                text += QString("Unknown error.");
                break;
            }
            break;
        case 0x05:
            if (err == 0x00) {
                text += QString("New BP set.");
            }
            else if (err == 0x01) {
                text += QString("Invalid pressure data.");
            }
            else {
                text += QString("Unknown error.");
            }
            break;
        case 0x0c:
            if (err == 0x00) {
                text += QString("New values accepted .");
            }
            else if (err == 0x01) {
                text += QString("Invalid data, old data remain unchanged.");
            }
            else {
                text += QString("Unknown error.");
            }
            break;
        case 0x06:
            switch (err) {
            case 87:
                text += QString("ID_ICG_CONN; The ICG cable has changed. The current measurement stops.");
                break;
            case 88:
                text += QString("ID_ICG_REMOVED; The ICG cable has been removed.");
                break;
            case 92:
                text += QString("ID_EL_ERR_ICG; Bad/dry Electrodes or sensor disconnected.");
                break;
            case 131:
                text += QString("ID_EL_ERR_ICG_L1; ICG electrode (#1, left) has bad or no contact to the patient!");
                break;
            case 132:
                text += QString("ID_EL_ERR_ICG_R1; ICG electrode (#1, right) has bad or no contact to the patient!");
                break;
            case 133:
                text += QString("ID_EL_ERR_ICG_L23; ICG electrodes (#2 or #3, left) have bad or no contact to the patient!");
                break;
            case 134:
                text += QString("ID_EL_ERR_ICG_R23; ICG electrodes (#2 or #3, right) have bad or no contact to the patient!");
                break;
            case 137:
                text += QString("ID_EL_ERR_ICG_NONE; ICG sensors OK.");
                break;
            case 138:
                text += QString("ID_EL_ERR_ICG_L4; ICG electrode (#4, left) has bad or no contact to the patient!");
                break;
            case 139:
                text += QString("ID_EL_ERR_ICG_R4; ICG electrode (#4, right) has bad or no contact to the patient!");
                break;
            case 118:
                text += QString("ID_ICG_PATIENT_CABLE_SHORT_CIRCUIT; ICG patient cable exceeds current limit.");
                break;
            case 119:
                text += QString("ID_ICG_PATIENT_CABLE_LOW_VOLTAGE; The power supply of the ICG patient cable is out of range (too low voltage).");
                break;
            case 121:
                text += QString("ID_ICG_PATIENT_CABLE_NOT_CALIBRATED; ICG patient cable is not calibrated.");
                break;
            default:
                text += QString("Unknown error.");
                break;
            }
            break;
        default:
            break;
        }
        browser->append(text);
    });
    connect(this, &MainWidget::readyClose, module, &IcgControlModule::powerDownMode);

    heightEdit->setText(QString::number(175));
    weightEdit->setText(QString::number(84));
    ageEdit->setText(QString::number(70));
    maleButton->setChecked(true);
    sbpEdit->setText(QString::number(140));
    dbpEdit->setText(QString::number(90));
    cvpEdit->setText(QString::number(6));
    pawpEdit->setText(QString::number(10));
    hbEdit->setText(QString::number(16));
    spO2Edit->setText(QString::number(100));
}

MainWidget::~MainWidget()
{
    module->deleteLater();
}

void MainWidget::closeEvent(QCloseEvent *event)
{
    emit readyClose();
    event->accept();
}

QString MainWidget::ntos(const float &value) const
{
    if (value == char(0xfd)) {
        return "--";
    }
    return QString::number(value);
}
