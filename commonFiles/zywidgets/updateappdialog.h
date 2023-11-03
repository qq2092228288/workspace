#ifndef UPDATEAPPDIALOG_H
#define UPDATEAPPDIALOG_H

#include <QDialog>
#include <QJsonObject>
#include <QLabel>
#include <QTextBrowser>
#include <QPushButton>
#include <QVBoxLayout>
#include <QProgressBar>
#include <QFile>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QFileDialog>
#include <QProcess>

class UpdateAppDialog : public QDialog
{
    Q_OBJECT
public:
    explicit UpdateAppDialog(const QJsonObject &object, QWidget *parent = nullptr);
signals:
    void installNewApp();
private slots:
    void installSlot();
    void downloadSlot();
    void replyContent();
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void downloadFinished(QNetworkReply *reply);
private:
    static QString toMB(const qint64 &btyes);
private:
    QTextBrowser *contentBrowser;
    QPushButton *installBtn;
    QPushButton *downloadBtn;
    QProgressBar *progressBar;
    QJsonObject object;

    QNetworkAccessManager *manager;
    QNetworkReply *m_reply;
    QString fileName;
    QFile *m_file;
    QProcess *process;
};

#endif // UPDATEAPPDIALOG_H
