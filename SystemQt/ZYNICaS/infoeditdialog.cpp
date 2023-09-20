#include "infoeditdialog.h"
#include "datamanagement.h"
#include "datacalculation.h"

double BodyValue::BSA() const
{
    return DatCa::cBsa(height, weight);
}

double BodyValue::VEPT() const
{
    return DatCa::cVept(height, weight, sex);
}

int BodyValue::MAP() const
{
    return DatCa::cMap(SBP, DBP);
}

InfoEditDialog::InfoEditDialog(QWidget *parent)
    : QDialog{parent}
{
    this->setWindowTitle(tr("编辑患者信息"));
    auto &instance = DataManagement::getInstance();
    setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
    setMinimumSize(800*instance.wZoom(), 650*instance.hZoom());
    //样式表
    this->setStyleSheet(instance.dialogQss(1.5));

    baseInfoGroupBox = new QGroupBox(tr("基本信息"), this);
    nameLabel = new QLabel(tr("姓名"), this);
    numLabel = new QLabel(instance.idName(), this);
    sexLabel = new QLabel(tr("性别"), this);
    ageLabel = new QLabel(tr("年龄"), this);
    heightLabel = new QLabel(tr("身高(cm)"), this);
    weightLabel = new QLabel(tr("体重(kg)"), this);
    hbLabel = new QLabel(tr("血红蛋白(g/dl)"), this);
    nameLineEdit = new QLineEdit(this);
    numLineEdit = new QLineEdit(this);
    sexComboBox = new QComboBox(this);
    ageLineEdit = new QLineEdit(this);
    heightLineEdit = new QLineEdit(this);
    weightLineEdit = new QLineEdit(this);
    hbLineEdit = new QLineEdit(this);

    inquiryGroupBox = new QGroupBox(tr("问诊"), this);
    fhhLabel = new QLabel(tr("高血压家族史:"), this);
    edhLabel = new QLabel(tr("过量饮酒史:"), this);
    ltshLabel = new QLabel(tr("长期吸烟史:"), this);
    lthmsLabel = new QLabel(tr("长期精神紧张史:"), this);
    ptmLabel = new QLabel(tr("坚持服药:"), this);
    alLabel = new QLabel(tr("活动受限:"), this);
    fhhComboxBox = new QComboBox(this);
    edhComboxBox = new QComboBox(this);
    ltshComboxBox = new QComboBox(this);
    lthmsComboxBox = new QComboBox(this);
    ptmComboxBox = new QComboBox(this);
    alComboxBox = new QComboBox(this);

    importBtn = new QPushButton(tr("导 入"),this);
    clearBtn = new QPushButton(tr("清 空"),this);
    confirmBtn = new QPushButton(tr("确 定"),this);
    pInfoDialog = new PersonalInfoDialog(this);

    auto list = QStringList()<<tr("")<<tr("是")<<tr("否")<<tr("不详");
    fhhComboxBox->addItems(list);
    edhComboxBox->addItems(list);
    ltshComboxBox->addItems(list);
    lthmsComboxBox->addItems(list);
    ptmComboxBox->addItems(list);
    alComboxBox->addItems(list);

    sexComboBox->addItem(tr("男"));
    sexComboBox->addItem(tr("女"));
    sexComboBox->addItem("");
    sexComboBox->setCurrentIndex(2);
//    numLineEdit->setValidator(new QRegExpValidator(QRegExp("^[a-zA-Z0-9]+$"),this));
    ageLineEdit->setValidator(new QRegExpValidator(QRegExp("^([1-9]|(1[0-4])){0,1}[0-9]$"),this));
    heightLineEdit->setValidator(new QRegExpValidator(QRegExp("^([2-9]|[1-2][0-9])[0-9]$"),this));
    weightLineEdit->setValidator(new QRegExpValidator(QRegExp("^([2-9]|[1-2][0-9])[0-9]$"),this));
    hbLineEdit->setValidator(new QRegExpValidator(QRegExp("^([1-9]|[1-4][0-9])([\\.][0-9]){0,1}$"),this));

    auto mainLayout = new QVBoxLayout(this);
    auto bLayout = new QGridLayout(baseInfoGroupBox);
    auto iLayout = new QGridLayout(inquiryGroupBox);
    auto hLayout = new QHBoxLayout;
    mainLayout->addWidget(baseInfoGroupBox, 4);
    mainLayout->addWidget(inquiryGroupBox, 2);
    mainLayout->addLayout(hLayout, 1);

    bLayout->addWidget(nameLabel, 0, 0);
    bLayout->addWidget(nameLineEdit, 0, 1);
    bLayout->addWidget(numLabel, 0, 2);
    bLayout->addWidget(numLineEdit, 0, 3);
    bLayout->addWidget(sexLabel, 1, 0);
    bLayout->addWidget(sexComboBox, 1, 1);
    bLayout->addWidget(ageLabel, 1, 2);
    bLayout->addWidget(ageLineEdit, 1, 3);
    bLayout->addWidget(heightLabel, 2, 0);
    bLayout->addWidget(heightLineEdit, 2, 1);
    bLayout->addWidget(weightLabel, 2, 2);
    bLayout->addWidget(weightLineEdit, 2, 3);
    bLayout->addWidget(hbLabel, 3, 0);
    bLayout->addWidget(hbLineEdit, 3, 1);

    iLayout->addWidget(fhhLabel, 0, 0);
    iLayout->addWidget(fhhComboxBox, 0, 1);
    iLayout->addWidget(edhLabel, 0, 2);
    iLayout->addWidget(edhComboxBox, 0, 3);
    iLayout->addWidget(ltshLabel, 0, 4);
    iLayout->addWidget(ltshComboxBox, 0, 5);
    iLayout->addWidget(lthmsLabel, 1, 0);
    iLayout->addWidget(lthmsComboxBox, 1, 1);
    iLayout->addWidget(ptmLabel, 1, 2);
    iLayout->addWidget(ptmComboxBox, 1, 3);
    iLayout->addWidget(alLabel, 1, 4);
    iLayout->addWidget(alComboxBox, 1, 5);

    hLayout->addStretch(4);
    hLayout->addWidget(importBtn);
    hLayout->addStretch(1);
    hLayout->addWidget(clearBtn);
    hLayout->addStretch(1);
    hLayout->addWidget(confirmBtn);

    heightLineEdit->setPlaceholderText(tr("120 ~ 230"));
    weightLineEdit->setPlaceholderText(tr("30 ~ 160"));

    connect(clearBtn, &QPushButton::clicked, this, &InfoEditDialog::clearSlot);
    connect(confirmBtn, &QPushButton::clicked, this, &InfoEditDialog::confirmSlot);
    connect(nameLineEdit, &QLineEdit::textEdited, this, &InfoEditDialog::stopCheck);
    connect(numLineEdit, &QLineEdit::textEdited, this, &InfoEditDialog::stopCheck);
    connect(sexComboBox, &QComboBox::currentTextChanged, this, &InfoEditDialog::stopCheck);
    connect(ageLineEdit, &QLineEdit::textEdited, this, &InfoEditDialog::stopCheck);
    connect(heightLineEdit, &QLineEdit::textEdited, this, &InfoEditDialog::stopCheck);
    connect(weightLineEdit, &QLineEdit::textEdited, this, &InfoEditDialog::stopCheck);
    connect(importBtn, &QPushButton::clicked, pInfoDialog, &PersonalInfoDialog::exec);
    connect(pInfoDialog, &PersonalInfoDialog::patientInfo, this, &InfoEditDialog::importPatientInfo);
}

void InfoEditDialog::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return){
        confirmSlot();
    }else {
        event->accept();
    }
}

void InfoEditDialog::clear()
{
    nameLineEdit->clear();
    numLineEdit->clear();
    sexComboBox->setCurrentIndex(2);
    ageLineEdit->clear();
    heightLineEdit->clear();
    weightLineEdit->clear();
    hbLineEdit->clear();

    fhhComboxBox->setCurrentIndex(0);
    edhComboxBox->setCurrentIndex(0);
    ltshComboxBox->setCurrentIndex(0);
    lthmsComboxBox->setCurrentIndex(0);
    ptmComboxBox->setCurrentIndex(0);
    alComboxBox->setCurrentIndex(0);
}

void InfoEditDialog::clearSlot()
{
    emit DataManagement::getInstance().startCheck(false);
    emit DataManagement::getInstance().clear();
    clear();
}

void InfoEditDialog::confirmSlot()
{
    QString age = ageLineEdit->text();
    QString height = heightLineEdit->text();
    QString weight = weightLineEdit->text();
    if(nameLineEdit->text().isEmpty() || numLineEdit->text().isEmpty() ||
            age.isEmpty() || height.isEmpty() || weight.isEmpty()){
        QMessageBox::warning(this, tr("警告！"), tr("信息不完整！"));
        return;
    }
    else if (age.toInt() < 6 || age.toInt() > 150) {
        QMessageBox::warning(this, tr("警告！"), tr("年龄超过范围！"));
        return;
    }
    else if (height.toInt() < 120 || height.toInt() > 230) {
        QMessageBox::warning(this, tr("警告！"), tr("身高超过范围！"));
        return;
    }
    else if (weight.toInt() < 30 || weight.toInt() > 160) {
        QMessageBox::warning(this,tr("警告！"),tr("体重超过范围！"));
        return;
    }
    else if (sexComboBox->currentIndex() == 2) {
        QMessageBox::warning(this, tr("警告！"), tr("请选择性别！"));
        return;
    }
    else if (!hbLineEdit->text().isEmpty() && hbLineEdit->text().indexOf(".") == hbLineEdit->text().size() -1) {
        QMessageBox::warning(this, tr("警告！"), tr("血红蛋白输入格式错误！"));
        return;
    }
    else {
        if (inquiryGroupBox->isVisible() && (fhhComboxBox->currentIndex() == 0 || edhComboxBox->currentIndex() == 0 ||
            ltshComboxBox->currentIndex() == 0 || lthmsComboxBox->currentIndex() == 0 ||
            ptmComboxBox->currentIndex() == 0 || alComboxBox->currentIndex() == 0)) {
            QMessageBox::warning(this, tr("警告！"), tr("请将问诊信息选择完全！"));
            return;
        }
        BodyValue bValue;
        bValue.name = nameLineEdit->text();
        bValue.id = numLineEdit->text();
        bValue.sex = sexComboBox->currentIndex();
        bValue.age = ageLineEdit->text().toInt();
        bValue.height = heightLineEdit->text().toInt();
        bValue.weight = weightLineEdit->text().toInt();
        bValue.hb = hbLineEdit->text().toFloat();

        bValue.fhh = fhhComboxBox->currentIndex();
        bValue.edh = edhComboxBox->currentIndex();
        bValue.ltsh = ltshComboxBox->currentIndex();
        bValue.lthms = lthmsComboxBox->currentIndex();
        bValue.ptm = ptmComboxBox->currentIndex();
        bValue.al = alComboxBox->currentIndex();

        emit bodyValue(bValue);
        emit DataManagement::getInstance().startCheck(true);
        pInfoDialog->insertData(bValue);
        this->close();
        emit updateUi();
    }
}

void InfoEditDialog::showHb(bool show)
{
    hbLabel->setVisible(show);
    hbLineEdit->setVisible(show);
}

void InfoEditDialog::showInquiry(bool show)
{
    inquiryGroupBox->setVisible(show);
}

void InfoEditDialog::stopCheck()
{
    if (DataManagement::getInstance().getTebco()->isWorking()) {
        emit DataManagement::getInstance().startCheck(false);
    }
}

void InfoEditDialog::importPatientInfo(QString id, QString name, int sex, int age, int height, int weight,
                                       int fhh, int edh, int ltsh, int lthms, int ptm, int al)
{
    nameLineEdit->setText(name);
    numLineEdit->setText(id);
    sexComboBox->setCurrentIndex(sex);
    ageLineEdit->setText(QString::number(age));
    heightLineEdit->setText(QString::number(height));
    weightLineEdit->setText(QString::number(weight));

    fhhComboxBox->setCurrentIndex(fhh);
    edhComboxBox->setCurrentIndex(edh);
    ltshComboxBox->setCurrentIndex(ltsh);
    lthmsComboxBox->setCurrentIndex(lthms);
    ptmComboxBox->setCurrentIndex(ptm);
    alComboxBox->setCurrentIndex(al);
}
