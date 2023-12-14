#ifndef WAITINGDIALOGUI_H
#define WAITINGDIALOGUI_H

#include <QDialog>
#include <QVBoxLayout>
#include <QLabel>
#include <QMovie>
#include <QFile>

QT_BEGIN_NAMESPACE

class WaitingDialogUi : public QDialog
{
public:
    QVBoxLayout *mainLayout;
    QLabel *movieLabel;
    QLabel *textLabel;
    QMovie *movie;
    void setupUi(QDialog *Dialog)
    {
        if (Dialog->objectName().isEmpty())
            Dialog->setObjectName(QString::fromUtf8("Dialog"));
        Dialog->setAttribute(Qt::WA_TranslucentBackground, true);
        Dialog->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
        Dialog->setWindowModality(Qt::WindowModal);

        QFile qssFile(":/qss/waitingdialogui.qss");
        if (qssFile.open(QIODevice::ReadOnly))
            Dialog->setStyleSheet(qssFile.readAll());

        mainLayout = new QVBoxLayout(Dialog);
        mainLayout->setObjectName(QString::fromUtf8("mainLayout"));

        movieLabel = new QLabel(Dialog);
        movieLabel->setObjectName(QString::fromUtf8("movieLabel"));

        mainLayout->addWidget(movieLabel, 0, Qt::AlignHCenter);

        movie = new QMovie(":/images/waiting.gif", QByteArray(), Dialog);
        movie->setObjectName(QString::fromUtf8("movie"));

        textLabel = new QLabel(Dialog);
        textLabel->setObjectName(QString::fromUtf8("textLabel"));

        mainLayout->addWidget(textLabel, 0, Qt::AlignCenter);

        retranslateUi(Dialog);
    }
    void retranslateUi(QDialog *Dialog)
    {
        Q_UNUSED(Dialog);
        movieLabel->setMovie(movie);
        movieLabel->setScaledContents(true);
    }
};

namespace Ui {
    class WaitingDialog : public WaitingDialogUi {};
}

QT_END_NAMESPACE

#endif // WAITINGDIALOGUI_H
