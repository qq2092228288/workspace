#include "mainpagewidget.h"
#include "waveform.h"
#include "parameterwidget.h"
#include "plrwidget.h"
#include "reportset.h"
#include "datatype.h"

#include <QScreen>
#include <QDebug>

MainPageWidget::MainPageWidget(QWidget *parent)
    : QWidget{parent}
{
    auto group = new QGroupBox(this);
    auto infoGroup = new QGroupBox(QString::fromUtf8("个人信息"), this);
    auto inquiryGroup = new QGroupBox(QString::fromUtf8("问诊"), this);
    auto opGroup = new QGroupBox(this);
    auto bpGroup = new QGroupBox(QString::fromUtf8("血压(单位: mmHg)"), this);
    auto auxGroup = new QGroupBox(QString::fromUtf8("辅助参数(单位: mmHg)"), this);
    auto otherGroup = new QGroupBox(this);
    auto dataGroup = new QGroupBox(this);

    auto nameLabel = new QLabel(QString::fromUtf8("姓名"), this);
    auto idLabel = new QLabel(QString::fromUtf8("病历号"), this);
    auto genderLabel = new QLabel(QString::fromUtf8("性别"), this);
    auto ageLabel = new QLabel(QString::fromUtf8("年龄"), this);
    auto yearLabel = new QLabel(QString::fromUtf8("岁"), this);
    auto heightLabel = new QLabel(QString::fromUtf8("身高"), this);
    auto cmLabel = new QLabel(QString::fromUtf8("cm"), this);
    auto weightLabel = new QLabel(QString::fromUtf8("体重"), this);
    auto kgLabel = new QLabel(QString::fromUtf8("kg"), this);
    auto hbLabel = new QLabel(QString::fromUtf8("血红蛋白"), this);
    auto gdlLabel = new QLabel(QString::fromUtf8("g/dl"), this);
    auto sbpLabel = new QLabel(QString::fromUtf8("收缩压(SBP)"), this);
    auto dbpLabel = new QLabel(QString::fromUtf8("舒张压(DBP)"), this);
    auto cvpLabel = new QLabel(QString::fromUtf8("中心静脉压(CVP)"), this);
    auto lapLabel = new QLabel(QString::fromUtf8("左房压(LAP)"), this);

    auto nameLineEdit = new QLineEdit(this);
    auto idLineEdit = new QLineEdit(this);
    auto maleButton = new QRadioButton(QString::fromUtf8("男"), this);
    auto femaleButton = new QRadioButton(QString::fromUtf8("女"), this);
    auto ageLineEdit = new QLineEdit(this);
    auto heightLineEdit = new QLineEdit(this);
    auto weightLineEdit = new QLineEdit(this);
    auto hbLineEdit = new QLineEdit(this);
    auto sbpLineEdit = new QLineEdit(this);
    auto dbpLineEdit = new QLineEdit(this);
    auto cvpLineEdit = new QLineEdit(this);
    auto lapLineEdit = new QLineEdit(this);

    auto importButton = new QPushButton(QString::fromUtf8("历史\n导入"), this);
    auto clearButton = new QPushButton(QString::fromUtf8("清空\n数据"), this);
    auto confirmButton = new QPushButton(QString::fromUtf8("开始\n检测"), this);
    auto bpButton = new QPushButton(QString::fromUtf8("血压\n输入"), this);
    auto auxButton = new QPushButton(QString::fromUtf8("参数\n输入"), this);
    auto doubleButton = new QPushButton(QString::fromUtf8("双体位"), this);
    auto singleButton = new QPushButton(QString::fromUtf8("单体位"), this);
    auto otherStack = new QStackedWidget(this);

    importButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    clearButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    confirmButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    bpButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    auxButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    auto layout = new QHBoxLayout(this);
    auto groupLayout = new QVBoxLayout;
    auto infoLayout = new QGridLayout;
    auto inquiryLayout = new QGridLayout;
    auto opLayout = new QHBoxLayout;
    auto bpLayout = new QGridLayout;
    auto auxLayout = new QGridLayout;
    auto otherLayout = new QGridLayout;
    auto dataLayout = new QGridLayout;

    group->setLayout(groupLayout);
    infoGroup->setLayout(infoLayout);
    inquiryGroup->setLayout(inquiryLayout);
    opGroup->setLayout(opLayout);
    bpGroup->setLayout(bpLayout);
    auxGroup->setLayout(auxLayout);
    otherGroup->setLayout(otherLayout);
    dataGroup->setLayout(dataLayout);

    groupLayout->addWidget(infoGroup, 5);
    groupLayout->addWidget(inquiryGroup, 1);
    groupLayout->addWidget(opGroup, 1);
    groupLayout->addWidget(bpGroup, 1);
    groupLayout->addWidget(auxGroup, 1);
    groupLayout->addWidget(otherGroup, 1);

    infoLayout->addWidget(nameLabel, 0, 0);
    infoLayout->addWidget(nameLineEdit, 0, 1, 1, 4);
    infoLayout->addWidget(idLabel, 1, 0);
    infoLayout->addWidget(idLineEdit, 1, 1, 1, 4);
    infoLayout->addWidget(genderLabel, 2, 0);
    infoLayout->addWidget(maleButton, 2, 1, Qt::AlignLeft);
    infoLayout->addWidget(femaleButton, 2, 2, Qt::AlignRight);
    infoLayout->addWidget(ageLabel, 2, 4);
    infoLayout->addWidget(ageLineEdit, 2, 5, 1, 2);
    infoLayout->addWidget(yearLabel, 2, 7);
    infoLayout->addWidget(heightLabel, 3, 0);
    infoLayout->addWidget(heightLineEdit, 3, 1, 1, 2);
    infoLayout->addWidget(cmLabel, 3, 3);
    infoLayout->addWidget(weightLabel, 3, 4);
    infoLayout->addWidget(weightLineEdit, 3, 5, 1, 2);
    infoLayout->addWidget(kgLabel, 3, 7);
    infoLayout->addWidget(hbLabel, 4, 0);
    infoLayout->addWidget(hbLineEdit, 4, 1, 1, 2);
    infoLayout->addWidget(gdlLabel, 4, 3);

    QStringList inquiry{"高血压家族史", "过量饮酒史", "长期吸烟史", "长期精神紧张史", "坚持服药", "活动受限"};
    QStringList texts{"", "是", "否", "不详"};
    QVector<QComboBox *> vector;
    for (int i = 0; i < inquiry.count(); ++i) {
        auto label = new QLabel(inquiry.at(i), this);
        auto comboBox = new QComboBox(this);
        comboBox->addItems(texts);
        vector.append(comboBox);
        inquiryLayout->addWidget(label, i / 2, i%2 * 2);
        inquiryLayout->addWidget(comboBox, i / 2, i%2 * 2 + 1, Qt::AlignRight);
    }

    opLayout->addWidget(importButton, 0, Qt::AlignLeft);
    opLayout->addWidget(clearButton, 0, Qt::AlignHCenter);
    opLayout->addWidget(confirmButton, 0, Qt::AlignRight);

    bpLayout->addWidget(sbpLabel, 0, 0);
    bpLayout->addWidget(sbpLineEdit, 0, 1, Qt::AlignLeft);
    bpLayout->addWidget(dbpLabel, 1, 0);
    bpLayout->addWidget(dbpLineEdit, 1, 1, Qt::AlignLeft);
    bpLayout->addWidget(bpButton, 0, 2, 2, 1, Qt::AlignRight);

    auxLayout->addWidget(cvpLabel, 0, 0);
    auxLayout->addWidget(cvpLineEdit, 0, 1, Qt::AlignLeft);
    auxLayout->addWidget(lapLabel, 1, 0);
    auxLayout->addWidget(lapLineEdit, 1, 1, Qt::AlignLeft);
    auxLayout->addWidget(auxButton, 0, 2, 2, 1, Qt::AlignRight);

    otherLayout->addWidget(doubleButton, 0, 0, Qt::AlignLeft);
    otherLayout->addWidget(singleButton, 1, 0, Qt::AlignLeft);
    otherLayout->addWidget(otherStack, 0, 1, 2, 1, Qt::AlignRight);

    auto widget1 = new QWidget(otherStack);
    auto widget2 = new QWidget(otherStack);
    auto wlayout1 = new QHBoxLayout(widget1);
    auto wlayout2 = new QHBoxLayout(widget2);
    auto recordButton = new QPushButton(QString::fromUtf8("记录\n体位"), otherStack);
    auto testButton = new QPushButton(QString::fromUtf8("PLR"), otherStack);
    auto reportButton = new QPushButton(QString::fromUtf8("生成\n报告"), otherStack);

    wlayout1->setContentsMargins(0, 0, 0, 0);
    wlayout2->setContentsMargins(0, 0, 0, 0);
    recordButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    testButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    reportButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    wlayout1->addWidget(recordButton, 0, Qt::AlignLeft);
    wlayout1->addWidget(testButton, 0, Qt::AlignRight);
    wlayout2->addWidget(reportButton, 0, Qt::AlignRight);
    otherStack->addWidget(widget1);
    otherStack->addWidget(widget2);

    auto waveform = new Waveform(this);
    auto bpWidget = new ParameterWidget(ParamJson(ReportParameters::find(Type::MAP)), this);
    auto pBpOrCapLayout = new QHBoxLayout;
    auto dataStack = new QStackedWidget(this);

    auto bBpWidget = new SbpDbpWidget(ParamJson(ReportParameters::find(Type::SBP)),
                                      ParamJson(ReportParameters::find(Type::DBP)),
                                      this);
    auto capWidget = new ParameterWidget(ParamJson(ReportParameters::find(Type::DBP)), this);

    pBpOrCapLayout->addWidget(bBpWidget);
    pBpOrCapLayout->addWidget(capWidget);

    capWidget->hide();

    auto paramWidget = new QWidget(dataStack);
    auto paramLayout = new QGridLayout(paramWidget);
    paramLayout->setContentsMargins(0, 0, 0, 0);
    QList<Type> paramList {Type::HR, Type::TFC, Type::CO, Type::CI, Type::SV, Type::SI};
    for (int i = 0; i < paramList.count(); ++i) {
        auto widget = new ParameterWidget(ParamJson(ReportParameters::find(paramList.at(i))), paramWidget);
        paramLayout->addWidget(widget, i / 2, i%2 * 2);
    }
    auto plrWidget = new PlrWidget(dataStack);

    dataStack->addWidget(paramWidget);
    dataStack->addWidget(plrWidget);

    dataLayout->addWidget(waveform, 0, 0, 2, 2);
    dataLayout->addWidget(bpWidget, 2, 0);
    dataLayout->addLayout(pBpOrCapLayout, 2, 1);
    dataLayout->addWidget(dataStack, 0, 2, 3, 2);

    layout->addWidget(group, 2);
    layout->addWidget(dataGroup, 5);

    auto genderGroup = new QButtonGroup(this);
    genderGroup->addButton(maleButton, 0);
    genderGroup->addButton(femaleButton, 1);

    auto positionGroup = new QButtonGroup(this);
    positionGroup->addButton(doubleButton, 0);
    positionGroup->addButton(singleButton, 1);

    auto setLineEdit = [=](QLineEdit * lineEdit, int bottom, int top) {
        lineEdit->setValidator(new QIntValidator(13, 130, this));
        lineEdit->setPlaceholderText(QString("%1~%2").arg(bottom).arg(top));
    };

    nameLineEdit->setMaxLength(10);
    idLineEdit->setMaxLength(20);
    setLineEdit(ageLineEdit, 13, 130);
    setLineEdit(heightLineEdit, 120, 230);
    setLineEdit(weightLineEdit, 30, 155);
    setLineEdit(hbLineEdit, 1, 100);
    setLineEdit(sbpLineEdit, 40, 250);
    setLineEdit(dbpLineEdit, 20, 200);
    setLineEdit(cvpLineEdit, 1, 80);
    setLineEdit(lapLineEdit, 1, 80);

    doubleButton->setEnabled(false);

    auto pageInit = [=]() {
        nameLineEdit->clear();
        idLineEdit->clear();
        genderGroup->setExclusive(false);
        maleButton->setChecked(false);
        femaleButton->setChecked(false);
        genderGroup->setExclusive(true);
        ageLineEdit->clear();
        heightLineEdit->clear();
        weightLineEdit->clear();
        hbLineEdit->clear();
        foreach (auto comboBox, vector) {
            comboBox->setCurrentIndex(0);
        }
        sbpLineEdit->clear();
        dbpLineEdit->clear();
        cvpLineEdit->setText(QString::number(6));
        lapLineEdit->setText(QString::number(10));
    };

    connect(clearButton, &QPushButton::clicked, this, pageInit);

    connect(positionGroup, &QButtonGroup::idClicked, this, [=](int index){
        doubleButton->setEnabled(index == 1);
        singleButton->setEnabled(index == 0);
        otherStack->setCurrentIndex(index);
    });

    connect(testButton, &QPushButton::clicked, this, [=](){
        bBpWidget->hide();
        capWidget->show();
        dataStack->setCurrentIndex(1);
    });
    connect(plrWidget, &PlrWidget::stop, this, [=](){
        dataStack->setCurrentIndex(0);
    });
    // dataStack->setCurrentIndex(1);
}
