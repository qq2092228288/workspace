#include "baseargwidget.h"


BaseArgWidget::BaseArgWidget(const qreal &fontZoom, QWidget *parent)
    : QWidget{parent}
{
    QVBoxLayout *vLayout = new QVBoxLayout(this);
    m_pMainLabel = new QLabel(this);
    m_pSecondaryLabel = new QLabel(this);
    m_pArgValueLineEdit = new QLineEdit(this);
    m_pScopeLabel = new QLabel(this);
    m_pUnitLabel = new QLabel(this);

    vLayout->addWidget(m_pMainLabel, 0, Qt::AlignLeft);
    vLayout->addWidget(m_pSecondaryLabel, 0, Qt::AlignLeft);
    vLayout->addWidget(m_pArgValueLineEdit);
    vLayout->addWidget(m_pScopeLabel, 0, Qt::AlignCenter);
    vLayout->addWidget(m_pUnitLabel, 0, Qt::AlignCenter);

    setStyleSheet(QString("BaseArgWidget{"
                          "background-color:#363636;"
                          "border-radius: %1px;"
                          "}").arg(10*fontZoom));
    m_pMainLabel->setStyleSheet(QString("QLabel{"
                                        "font-size: %1px;"
                                        "color: #ffffff;"
                                        "font-weight:bold;"
                                        "}").arg(35*fontZoom));
    m_pSecondaryLabel->setStyleSheet(QString("QLabel{"
                                             "font-size: %1px;"
                                             "color: #ffffff;"
                                             "}").arg(25*fontZoom));
    m_pArgValueLineEdit->setStyleSheet(QString("QLineEdit{"
                                               "font-size: %1px;"
                                               "color: #00ff7f;"
                                               "font-weight:bold;"
                                               "background:transparent;"
                                               "border-style:outset;"
                                               "}").arg(50*fontZoom));
    m_pScopeLabel->setStyleSheet(QString("QLabel{"
                                         "font-size: %1px;"
                                         "color: #ffffff;"
                                         "font-weight:normal;"
                                         "}").arg(18*fontZoom));
    m_pUnitLabel->setStyleSheet(QString("QLabel{"
                                        "font-size: %1px;"
                                        "color: #ffffff;"
                                        "font-weight:normal;"
                                        "}").arg(16*fontZoom));

    m_pArgValueLineEdit->setEnabled(false);
    m_pArgValueLineEdit->setAlignment(Qt::AlignCenter);
    m_pArgValueLineEdit->setPlaceholderText("--");
}

BaseArgWidget::BaseArgWidget(const QString &main, const QString &secondary, const QString &scope,
                             const QString &unit, const int &digit, const qreal &fontZoom, QWidget *parent)
{
    BaseArgWidget(fontZoom, parent);

    m_pMainLabel->setText(main);
    m_pSecondaryLabel->setText(secondary);
    m_pScopeLabel->setText(scope);
    m_pUnitLabel->setText(unit);
    m_digit = digit;
}

BaseArgWidget::~BaseArgWidget()
{

}

QString BaseArgWidget::getMainString() const
{
    return m_pMainLabel->text();
}

QString BaseArgWidget::getSecondaryString() const
{
    return m_pSecondaryLabel->text();
}

QString BaseArgWidget::getArgValueString() const
{
    return m_pArgValueLineEdit->text();
}

QString BaseArgWidget::getScopeString() const
{
    return m_pScopeLabel->text();
}

QString BaseArgWidget::getUnitString() const
{
    return m_pUnitLabel->text();
}

int BaseArgWidget::getDigit() const
{
    return m_digit;
}

void BaseArgWidget::clearArgValue()
{
    m_pArgValueLineEdit->clear();
}

void BaseArgWidget::setMainLabel(const QString &str)
{
    m_pMainLabel->setText(str);
}

void BaseArgWidget::setSecondaryLabel(const QString &str)
{
    m_pSecondaryLabel->setText(str);
}

void BaseArgWidget::setArgValueLindeEditSlot(const qreal &value)
{
    const QString old = m_pArgValueLineEdit->text();
    m_pArgValueLineEdit->setText(QString::number(value, 'f', m_digit));
    emit argValueChanged(QDateTime::currentMSecsSinceEpoch(), old, m_pArgValueLineEdit->text());
}

void BaseArgWidget::setScopeLabel(const QString &str)
{
    m_pScopeLabel->setText(str);
}

void BaseArgWidget::setUnitLabel(const QString &str)
{
    m_pUnitLabel->setText(str);
}

void BaseArgWidget::setDigit(const int &digit)
{
    m_digit = digit;
}
void BaseArgWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    event->accept();
    emit mouseDoubleClicked();
}

