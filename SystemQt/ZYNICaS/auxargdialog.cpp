#include "auxargdialog.h"
#include "datamanagement.h"

AuxArgDialog::AuxArgDialog(QWidget *parent)
    : QDialog{parent}
{
    this->setWindowTitle(tr("辅助参数"));
    auto &instance = DataManagement::getInstance();
    this->setMinimumSize(400*instance.wZoom(),200*instance.hZoom());
    setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
    //样式表
    this->setStyleSheet(instance.dialogQss(1.5));

    cvpLabel = new QLabel(tr("CVP_中心静脉压(2~6mmHg):"),this);
    lapLabel = new QLabel(tr("LAP_左房压(6~12mmHg):"),this);
    cvpLineEdit = new QLineEdit(this);
    lapLineEdit = new QLineEdit(this);
    confirmBtn = new QPushButton(tr("确定"),this);

    QGridLayout *mainLayout = new QGridLayout(this);

    mainLayout->addWidget(cvpLabel,0,0);
    mainLayout->addWidget(cvpLineEdit,0,1);
    mainLayout->addWidget(lapLabel,1,0);
    mainLayout->addWidget(lapLineEdit,1,1);
    mainLayout->addWidget(confirmBtn,2,1,Qt::AlignRight);

    QIntValidator *validator1 = new QIntValidator(2,6,this);
    QIntValidator *validator2 = new QIntValidator(6,12,this);
    cvpLineEdit->setValidator(validator1);
    lapLineEdit->setValidator(validator2);
    //默认值
    cvpLineEdit->setText(QString::number(4));
    lapLineEdit->setText(QString::number(9));
    connect(confirmBtn,&QPushButton::clicked,this,&AuxArgDialog::close);
}

void AuxArgDialog::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return){
        this->close();
    }else {
        QDialog::keyPressEvent(event);
    }
}

void AuxArgDialog::closeEvent(QCloseEvent *event)
{
    int cvp = cvpLineEdit->text().toInt();
    int lap = lapLineEdit->text().toInt();
    if(cvpLineEdit->text().isEmpty() || lapLineEdit->text().isEmpty()){
        QMessageBox::warning(this,tr("警告！"),tr("信息不完整！"));
        event->ignore();
    }
    else if (cvp < 2 || cvp > 6 || lap < 6 || lap > 12 ) {
        QMessageBox::warning(this,tr("警告！"),tr("超过范围值！"));
        event->ignore();
    }
    else{
        emit value(cvp,lap);
        event->accept();
    }
}

int AuxArgDialog::getCvp()
{
    return cvpLineEdit->text().toInt();
}

int AuxArgDialog::getLap()
{
    return lapLineEdit->text().toInt();
}
