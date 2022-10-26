#include "getiddialog.h"
#include "datamanagement.h"

GetIdDialog::GetIdDialog(QWidget *parent)
    : QDialog{parent}
{
    auto &instance = DataManagement::getInstance();
    setFixedSize(500*instance.wZoom(),300*instance.hZoom());
    this->setWindowTitle(tr("有效验证码"));
    QGridLayout *mainLayout = new QGridLayout(this);
    consumablesLabel = new QLabel(tr("剩余有效验证码:"),this);
    macLabel = new QLabel(tr("MAC地址:"),this);
    codeLabel = new QLabel(tr("有效验证码:"),this);
    consumablesEdit = new QLineEdit(this);
    macEdit = new QLineEdit(this);
    codeEdit = new QLineEdit(this);
    copyBtn = new QPushButton(tr("复制"),this);
    usedBtn = new QPushButton(tr("使用"),this);

    mainLayout->addWidget(consumablesLabel,0,0);
    mainLayout->addWidget(consumablesEdit,0,1);
    mainLayout->addWidget(macLabel,1,0);
    mainLayout->addWidget(macEdit,1,1);
    mainLayout->addWidget(copyBtn,1,2);
    mainLayout->addWidget(codeLabel,2,0);
    mainLayout->addWidget(codeEdit,2,1);
    mainLayout->addWidget(usedBtn,2,2);

    consumablesEdit->setFixedWidth(100);
    consumablesEdit->setReadOnly(true);
    consumablesEdit->setAlignment(Qt::AlignCenter);
    macEdit->setReadOnly(true);
    macEdit->setAlignment(Qt::AlignCenter);

    consumablesEdit->setText(QString::number(instance.getIdCheck()->getCurrentConsumables()));
    macEdit->setText(getMac());

    connect(copyBtn, &QPushButton::clicked, this, &GetIdDialog::copySlot);
    connect(usedBtn, &QPushButton::clicked, this, &GetIdDialog::usedSlot);
    connect(instance.getIdCheck(), &IdCheck::currentConsumables, this, &GetIdDialog::setConsumables);
}

QString GetIdDialog::getMac() const
{
    QList<QNetworkInterface> nets = QNetworkInterface::allInterfaces();// 获取所有网络接口列表
    for(int i = 0; i < nets.count(); i ++)
    {
        // 如果此网络接口不是回环地址，则就是我们需要找的Mac地址
        if(!nets[i].flags().testFlag(QNetworkInterface::IsLoopBack)) {
            return nets[i].hardwareAddress();
            break;
        }
    }
    return QString();
}

void GetIdDialog::setConsumables(const int &count)
{
    consumablesEdit->setText(QString::number(count));
    if (count <= 20 && count > 0) {
        QMessageBox::warning(this,tr("警告！"),tr("有效验证码剩余：%1 ，请注意！").arg(count));
    }
    else if(count == 0) {
        QMessageBox::warning(this,tr("警告！"),tr("有效验证码已使用完，请联系厂家！"));
    }
}

void GetIdDialog::copySlot()
{
    QApplication::clipboard()->setText(macEdit->text());
}

void GetIdDialog::usedSlot()
{
    if (codeEdit->text().isEmpty()) {
        QMessageBox::warning(this,tr("提示"),tr("有效验证码不能为空！"));
        return;
    }
    auto idCheck = DataManagement::getInstance().getIdCheck();
    QByteArray array = idCheck->strToByteArray(codeEdit->text());
    switch (idCheck->modifyConfig(array, getMac())) {
    case ZipError::Validity:
        QMessageBox::information(this,tr("提示"),tr("有效验证码增加%1，剩余%2")
                                 .arg(idCheck->getCodeCount(array, getMac()))
                                 .arg(idCheck->getCurrentConsumables()));
        break;
    case ZipError::CodeError:
        QMessageBox::warning(this,tr("警告！"),tr("有效验证码不正确！"));
        break;
    case ZipError::Overdue:
        QMessageBox::warning(this,tr("警告！"),tr("有效验证码过期！"));
        break;
    case ZipError::Used:
        QMessageBox::warning(this,tr("警告！"),tr("有效验证码已经被使用！"));
        break;
    }
}
