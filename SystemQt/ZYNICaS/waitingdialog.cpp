#include "waitingdialog.h"

WaitingDialog::WaitingDialog(const QString &text, QWidget *parent)
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
    textLabel = new QLabel(text, this);
    mainLayout->addWidget(movieLabel,0,Qt::AlignHCenter);
    mainLayout->addWidget(textLabel,0,Qt::AlignHCenter);

    m_movie = new QMovie(":/images/waiting.gif");
    movieLabel->setMovie(m_movie);
    movieLabel->setScaledContents(true);
    m_movie->start();
}

WaitingDialog::~WaitingDialog()
{
    m_movie->stop();
    delete m_movie;
//    qDebug()<<"~WaitingDialog()";
}
