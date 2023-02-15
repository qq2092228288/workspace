#include "anothersetdialog.h"

AnotherSetDialog::AnotherSetDialog(QWidget *parent)
    : QDialog{parent}
{
    setWindowTitle(tr("其它设置"));
    setMinimumSize(200, 200);

    QVBoxLayout *dLayout = new QVBoxLayout(this);

    autoStartCheckBox = new QCheckBox(tr("开机自启"), this);

    dLayout->addWidget(autoStartCheckBox, 0, Qt::AlignCenter | Qt::AlignTop);

    m_pSettings = new QSettings(AUTO_RUN, QSettings::NativeFormat, this);

    autoStartCheckBox->setChecked(m_pSettings->value(QApplication::applicationName()).isValid());

    connect(autoStartCheckBox, &QCheckBox::stateChanged, this, &AnotherSetDialog::autoStartSlot);
}

void AnotherSetDialog::autoStartSlot(int state)
{
    if (state == Qt::Checked) {
        m_pSettings->setValue(QApplication::applicationName(), QApplication::applicationFilePath().replace("/", "\\"));
    }
    else if (state == Qt::Unchecked) {
        m_pSettings->remove(QApplication::applicationName());
    }
}
