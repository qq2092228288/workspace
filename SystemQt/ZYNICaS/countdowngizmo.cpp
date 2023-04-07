#include "countdowngizmo.h"

CountDownGizmo::CountDownGizmo(const QString &timeHeader, const QString &stopTxt, QWidget *parent)
    : QWidget{parent},
      m_timeHeader{timeHeader},
      m_stopTxt{stopTxt},
      m_sec{0}
{
    // 无边框，无任务栏标志，顶层
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint);
    // 关闭销毁
    setAttribute(Qt::WA_DeleteOnClose);
    // 透明背景
    setAttribute(Qt::WA_TranslucentBackground);

    QVBoxLayout *vLayout = new QVBoxLayout(this);

    m_pTimeLabel = new QLabel(this);

    vLayout->addWidget(m_pTimeLabel);

    m_pTimeLabel->setAlignment(Qt::AlignCenter);

    m_pTimer = new QTimer(this);

    m_pTimer->setInterval(1000);

    connect(m_pTimer, &QTimer::timeout, this, &CountDownGizmo::timeoutSlot);

    m_pTimeLabel->setStyleSheet("color:blue;border-width: 1px;border-style: solid;border-color:blue;");
}

void CountDownGizmo::startCountDown(const int &sec)
{
    if (sec < 2 || sec > 3600) {
        QMessageBox::warning(this, tr("警告！"), tr("设置的时间要在1~3600秒之间！"));
        return;
    }
    m_sec = sec;
    m_pTimeLabel->setText(getTimeTxt());
    m_pTimer->start();
    emit isStopStatus(false);
}

void CountDownGizmo::mouseMoveEvent(QMouseEvent *event)
{
    if (m_mousePress) {
        move(event->globalPos() - m_mousePoint);
    }
    event->accept();
}

void CountDownGizmo::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_mousePress = true;
        m_mousePoint = event->pos();
    }
    event->accept();
}

void CountDownGizmo::mouseReleaseEvent(QMouseEvent *event)
{
    m_mousePress = false;
    event->accept();
}

QString CountDownGizmo::getTimeTxt()
{
    return QString("%1\n%2:%3")
            .arg(m_timeHeader)
            .arg(m_sec/60, 2, 10, QLatin1Char('0'))
            .arg(m_sec%60, 2, 10, QLatin1Char('0'));
}

void CountDownGizmo::timeoutSlot()
{
    if (1 < m_sec) {
        --m_sec;
        m_pTimeLabel->setText(getTimeTxt());
    }
    else if (1 == m_sec) {
        --m_sec;
        m_pTimeLabel->setText(m_stopTxt);
    }
    else {
        m_pTimer->stop();
        this->close();
        emit isStopStatus(true);
    }
}
