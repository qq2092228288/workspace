#include "bpeditdialog.h"
#include "datamanagement.h"

BPEditDialog::BPEditDialog(QWidget *parent)
    : QDialog{parent}
{
    this->setWindowTitle(tr("输入血压"));
    auto &instance = DataManagement::getInstance();
    setFixedSize(300*instance.wZoom(),200*instance.hZoom());
    setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
    //样式表
    this->setStyleSheet(instance.dialogQss());

    sbpLabel = new QLabel(tr("收缩压(mmHg):"),this);
    dbpLabel = new QLabel(tr("舒张压(mmHg):"),this);
    sbpLineEdit = new QLineEdit(this);
    dbpLineEdit = new QLineEdit(this);
    confirmBtn = new QPushButton(tr("确定"),this);

    QVBoxLayout *vLayout = new QVBoxLayout(this);
    QHBoxLayout *sbpLayout = new QHBoxLayout;
    QHBoxLayout *dbpLayout = new QHBoxLayout;
    QHBoxLayout *btnLayout = new QHBoxLayout;
    vLayout->addLayout(sbpLayout);
    vLayout->addLayout(dbpLayout);
    vLayout->addLayout(btnLayout);
    sbpLayout->addWidget(sbpLabel);
    sbpLayout->addWidget(sbpLineEdit);
    dbpLayout->addWidget(dbpLabel);
    dbpLayout->addWidget(dbpLineEdit);
    btnLayout->addWidget(confirmBtn,0,Qt::AlignRight);

    QIntValidator *validator = new QIntValidator(0,999,this);
    sbpLineEdit->setValidator(validator);
    dbpLineEdit->setValidator(validator);
    connect(confirmBtn,&QPushButton::clicked,this,&BPEditDialog::confirmSlot);
}

void BPEditDialog::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return){
        confirmSlot();
    }else {
        QDialog::keyPressEvent(event);
    }
}

void BPEditDialog::clear()
{
    sbpLineEdit->clear();
    dbpLineEdit->clear();
}

void BPEditDialog::confirmSlot()
{
    if(!sbpLineEdit->text().isEmpty() && !dbpLineEdit->text().isEmpty()){
        emit value(sbpLineEdit->text(),dbpLineEdit->text());
        this->close();
    }else{
        QMessageBox::warning(this,tr("警告！"),tr("信息不完整！"));
    }
}
