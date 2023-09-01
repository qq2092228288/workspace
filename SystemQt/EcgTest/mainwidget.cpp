#include "mainwidget.h"
#include "threadservice.h"
#include <QDebug>
#include <QMessageBox>

MainWidget::MainWidget(QWidget *parent)
    : QWidget{parent}
{
    this->setWindowTitle("心电图");
    this->setMinimumSize(1280, 960);
    gpropList<<GProp(MsgDT::LeadWireI, "I")
             <<GProp(MsgDT::LeadWireII, "II")
             <<GProp(MsgDT::LeadWireV1, "V1")
             <<GProp(MsgDT::LeadWireV2, "V2")
             <<GProp(MsgDT::LeadWireV3, "V3")
             <<GProp(MsgDT::LeadWireV4, "V4")
             <<GProp(MsgDT::LeadWireV5, "V5")
             <<GProp(MsgDT::LeadWireV6, "V6")
             <<GProp(MsgDT::LeadWireIII, "III")
             <<GProp(MsgDT::LeadWireAVR, "aVR")
             <<GProp(MsgDT::LeadWireAVL, "aVL")
             <<GProp(MsgDT::LeadWireAVF, "aVF");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    comboBox = new QComboBox(this);
    label = new QLabel("速度:", this);
    lineEdit = new QLineEdit(this);
    operationButton = new QPushButton(openStr, this);

    lineEdit->setFixedWidth(50);
    lineEdit->setValidator(new QRegExpValidator(QRegExp("[1-9]{1}[0-9]{0,3}$")));
    lineEdit->setText(QString::number(100));

    QHBoxLayout *hLayout = new QHBoxLayout;
    mainLayout->addLayout(hLayout);
    hLayout->addWidget(comboBox);
    hLayout->addWidget(label);
    hLayout->addWidget(lineEdit);
    hLayout->addWidget(operationButton);
    hLayout->addStretch();

    QGridLayout *gLayout = new QGridLayout;
    mainLayout->addLayout(gLayout);
    for (int i = 0; i < gpropList.length(); ++i) {
        auto temp = gpropList.at(i);
        auto waveform = new Waveform(temp.name, this);
        gLayout->addWidget(waveform, i/3, i%3);
        waveformMap.insert(temp.type, waveform);
    }

    comboBox->setFixedWidth(100);

    ads = new AdsModule;
    comboBox->addItems(ads->portNames());

    ThreadService::getInstance()->objectMoveToThread(ads);

    connect(operationButton, &QPushButton::clicked, this, &MainWidget::operationButtonClicked);
    connect(ads, &AdsModule::newdata, this, &MainWidget::newdata);
    connect(ads, &AdsModule::connectFailed, this, [=](){
        operationButton->setText(openStr);
        lineEdit->setEnabled(true);
    });
}

void MainWidget::operationButtonClicked()
{
    if (operationButton->text() == openStr) {
        foreach (auto waveform, waveformMap) {
            waveform->setSpeed(lineEdit->text().toInt());
        }
        if (ads->open(comboBox->currentText())) {
            operationButton->setText(closeStr);
            lineEdit->setEnabled(false);
        }
        else {
            QMessageBox::warning(this, "警告", "串口打开失败");
        }
    }
    else {
        ads->close();
        operationButton->setText(openStr);
        lineEdit->setEnabled(true);
    }
}

void MainWidget::newdata(AdsModule::MessageDataType type, double voltage)
{
    auto waveform = waveformMap.value(type, nullptr);
    if (waveform != nullptr) {
        waveform->append(voltage);
    }
}
