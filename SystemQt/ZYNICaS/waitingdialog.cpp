#include "waitingdialog.h"

WaitingDialog::WaitingDialog(QWidget *parent)
    : QDialog{parent}
{
    // 背景透明
    setAttribute(Qt::WA_TranslucentBackground, true);
    // 去边框
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    // 模式对话窗
    setWindowModality(Qt::WindowModal);

    mainLayout = new QVBoxLayout(this);
    movieLabel = new QLabel(this);
    textLabel = new QLabel(this);
    mainLayout->addWidget(movieLabel, 0, Qt::AlignHCenter);
    mainLayout->addWidget(textLabel, 0, Qt::AlignHCenter);

    auto font = textLabel->font();
    font.setPixelSize(18);
    textLabel->setFont(font);

    m_movie = new QMovie(":/images/waiting.gif");
    movieLabel->setMovie(m_movie);
    movieLabel->setScaledContents(true);
    m_movie->start();
}

WaitingDialog::~WaitingDialog()
{
    m_movie->stop();
    delete m_movie;
}

void WaitingDialog::setValue(const int &value, const int &total)
{
    textLabel->setText(QString("完成度：%1%").arg(100 * value / total, 2, 10, QLatin1Char('0')));
    if (value == total) {
        close();
    }
}

