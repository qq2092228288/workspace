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
    setMinimumSize(600*instance.wZoom(), 400*instance.hZoom());
    //样式表
    this->setStyleSheet(instance.dialogQss(1.5));

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
    importBtn = new QPushButton(tr("导 入"),this);
    clearBtn = new QPushButton(tr("清 空"),this);
    confirmBtn = new QPushButton(tr("确 定"),this);
    pInfoDialog = new PersonalInfoDialog(this);

    sexComboBox->addItem(tr("男"));
    sexComboBox->addItem(tr("女"));
    sexComboBox->addItem("");
    sexComboBox->setCurrentIndex(2);
//    numLineEdit->setValidator(new QRegExpValidator(QRegExp("^[a-zA-Z0-9]+$"),this));
    ageLineEdit->setValidator(new QRegExpValidator(QRegExp("^([1-9]|(1[0-4])){0,1}[0-9]$"),this));
    heightLineEdit->setValidator(new QRegExpValidator(QRegExp("^([2-9]|[1-2][0-9])[0-9]$"),this));
    weightLineEdit->setValidator(new QRegExpValidator(QRegExp("^([2-9]|[1-2][0-9])[0-9]$"),this));
    hbLineEdit->setValidator(new QRegExpValidator(QRegExp("^([1-9]|[1-4][0-9])([\\.][0-9]){0,1}$"),this));

    QGridLayout *mainLayout = new QGridLayout(this);
    mainLayout->addWidget(nameLabel, 0, 0);
    mainLayout->addWidget(nameLineEdit, 0, 1);
    mainLayout->addWidget(numLabel, 0, 2);
    mainLayout->addWidget(numLineEdit, 0, 3);
    mainLayout->addWidget(sexLabel, 1, 0);
    mainLayout->addWidget(sexComboBox, 1, 1);
    mainLayout->addWidget(ageLabel, 1, 2);
    mainLayout->addWidget(ageLineEdit, 1, 3);
    mainLayout->addWidget(heightLabel, 2, 0);
    mainLayout->addWidget(heightLineEdit, 2, 1);
    mainLayout->addWidget(weightLabel, 2, 2);
    mainLayout->addWidget(weightLineEdit, 2, 3);
    mainLayout->addWidget(hbLabel, 3, 0);
    mainLayout->addWidget(hbLineEdit, 3, 1);
    mainLayout->addWidget(importBtn, 4, 1, Qt::AlignRight);
    mainLayout->addWidget(clearBtn, 4, 2, Qt::AlignRight);
    mainLayout->addWidget(confirmBtn, 4, 3, Qt::AlignRight);

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
        QMessageBox::warning(this,tr("警告！"),tr("信息不完整！"));
        return;
    }
    else if (age.toInt() < 6 || age.toInt() > 150) {
        QMessageBox::warning(this,tr("警告！"),tr("年龄超过范围！"));
        return;
    }
    else if (height.toInt() < 120 || height.toInt() > 230) {
        QMessageBox::warning(this,tr("警告！"),tr("身高超过范围！"));
        return;
    }
    else if (weight.toInt() < 30 || weight.toInt() > 160) {
        QMessageBox::warning(this,tr("警告！"),tr("体重超过范围！"));
        return;
    }
    else if (sexComboBox->currentIndex() == 2) {
        QMessageBox::warning(this,tr("警告！"),tr("请选择性别！"));
        return;
    }
    else if (!hbLineEdit->text().isEmpty() && hbLineEdit->text().indexOf(".") == hbLineEdit->text().size() -1) {
        QMessageBox::warning(this,tr("警告！"),tr("血红蛋白输入格式错误！"));
        return;
    }
    else{
        BodyValue bValue;
        bValue.name = nameLineEdit->text();
        bValue.id = numLineEdit->text();
        bValue.sex = sexComboBox->currentIndex();
        bValue.age = ageLineEdit->text().toInt();
        bValue.height = heightLineEdit->text().toInt();
        bValue.weight = weightLineEdit->text().toInt();
        bValue.hb = hbLineEdit->text().toFloat();
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

void InfoEditDialog::stopCheck()
{
    if (DataManagement::getInstance().getTebco()->isWorking()) {
        emit DataManagement::getInstance().startCheck(false);
    }
}

void InfoEditDialog::importPatientInfo(QString id, QString name, int sex, int age, int height, int weight)
{
    nameLineEdit->setText(name);
    numLineEdit->setText(id);
    sexComboBox->setCurrentIndex(sex);
    ageLineEdit->setText(QString::number(age));
    heightLineEdit->setText(QString::number(height));
    weightLineEdit->setText(QString::number(weight));
}
