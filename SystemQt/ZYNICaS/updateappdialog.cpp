#include "updateappdialog.h"
#include <QMessageBox>
#include <databasens.h>
#include <singleton.h>
using namespace DatabaseEnumNs;

UpdateAppDialog::UpdateAppDialog(const QJsonObject &object, QWidget *parent)
    : QDialog{parent},
      object{object},
      m_file{nullptr}
{
    setWindowTitle(tr("有新版本可用"));
    setMinimumSize(400, 300);

    contentBrowser = new QTextBrowser(this);
    installBtn = new QPushButton(tr("安装"), this);
    downloadBtn = new QPushButton(tr("下载"), this);
    progressBar = new QProgressBar(this);

    QGridLayout *gLayout = new QGridLayout(this);
    gLayout->addWidget(contentBrowser, 0, 0, 1, 2, Qt::AlignCenter);
    gLayout->addWidget(installBtn, 1, 0, Qt::AlignLeft);
    gLayout->addWidget(downloadBtn, 1, 1, Qt::AlignRight);
    gLayout->addWidget(progressBar, 2, 0, 1, 2);

    auto content = object.value(Singleton::enumValueToKey(SoftwareManagement::content)).toString();
    contentBrowser->setText(content.replace("\\n", "\n"));
    installBtn->setVisible(false);
    progressBar->setAlignment(Qt::AlignCenter);

    connect(installBtn, &QPushButton::clicked, this, &UpdateAppDialog::installSlot);
    connect(downloadBtn, &QPushButton::clicked, this, &UpdateAppDialog::downloadSlot);

    manager = new QNetworkAccessManager(this);
    connect(manager, &QNetworkAccessManager::finished, this, &UpdateAppDialog::downloadFinished);

    process = new QProcess(this);
//    connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
//        [=](int exitCode, QProcess::ExitStatus exitStatus){
//        qDebug()<<exitCode<<exitStatus;
    //    });
}

void UpdateAppDialog::installSlot()
{
    if (QMessageBox::question(this, tr("提示"), tr("退出程序，并启动安装？")) == QMessageBox::Yes) {
        if (process->startDetached(fileName, QStringList())) {
            emit installNewApp();
        }
        else {
            QMessageBox::warning(this, tr("提示"), tr("文件不存在！"));
        }
    }
}

void UpdateAppDialog::downloadSlot()
{
    QString downloadPath = object.value(Singleton::enumValueToKey(SoftwareManagement::downloadPath)).toString();
    QString fileName = QFileDialog::getSaveFileName(this, tr("保存文件"),
                               downloadPath.mid(downloadPath.lastIndexOf("/") + 1),
                               tr("*.exe"));
    m_file = new QFile(fileName);
    if (m_file->open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        if (!fileName.isEmpty()) {
            QUrl url(downloadPath);
            QNetworkRequest request(url);
            m_reply = manager->get(request);
            connect(m_reply, &QNetworkReply::readyRead, this, &UpdateAppDialog::replyContent);
            connect(m_reply, &QNetworkReply::downloadProgress, this, &UpdateAppDialog::downloadProgress);
        }
    }
}

void UpdateAppDialog::replyContent()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if (reply && reply == m_reply)
    {
        m_file->write(reply->readAll());
    }
}

void UpdateAppDialog::downloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    progressBar->setMaximum(bytesTotal);
    progressBar->setValue(bytesReceived);
    progressBar->setFormat(QString("%1/%2 MB").arg(toMB(bytesReceived), toMB(bytesTotal)));
}

void UpdateAppDialog::downloadFinished(QNetworkReply *reply)
{
    if (!reply) return;
    if (reply->error() != QNetworkReply::NoError ||
            reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() != 200) {
        reply->deleteLater();
        return;
    }
    if (m_reply && m_reply == reply) {
        if (m_file) {
            fileName = m_file->fileName();
            m_file->close();
            delete m_file;
            m_file = nullptr;
            installBtn->setVisible(true);
            downloadBtn->setText(tr("重新下载"));
        }
    }
    reply->deleteLater();
}

QString UpdateAppDialog::toMB(const qint64 &btyes)
{
    return QString::number(btyes/1048576.0, 'f', 1);
}
