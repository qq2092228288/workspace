#include "idcheck.h"
#include "quazipfile.h"
#include <QCoreApplication>
#include <windows.h>

const quint64 C_COUNT = 0x005A6559616F0000;     // (check 1B)ZeYao(count 2B)
const quint64  C_TIME = 0x0000000337E11906;     // (check 4B)13824400774
const char * PASSWORD = "^ZeYao@13824400774$";  // zip password
static bool ok;

IdCheck::IdCheck(QObject *parent)
    : QObject{parent}
{

}

int IdCheck::getCurrentConsumables()
{
    return savedConsumables(getConfigFilesTxt().config);
}

QString IdCheck::removeSymbol(QString str)
{
    return str.remove(QRegExp("[^0-9a-zA-Z]"));
}

QByteArray IdCheck::strToByteArray(QString str)
{
    QByteArray array;
    for (int index = 0; index < str.size(); index+=2) {
        array.append(char(str.midRef(index, 2).toInt(&ok, 16)));
    }
    return array;
}

quint16 IdCheck::getCodeCount(const QByteArray &code, const quint64 &mac)
{
    return C_COUNT^getCodeTime(code, mac)^code.mid(0, 8).toHex().toULongLong(&ok, 16);
}

quint16 IdCheck::getCodeCount(const QByteArray &code, QString mac)
{
    return getCodeCount(code, removeSymbol(mac).toULongLong(&ok, 16));
}

QByteArray IdCheck::getCode(const quint16 &count, const quint64 &mac)
{
    // id = C_COUNT^count C_TIME^mac^time
    qint64 ctime = QDateTime::currentMSecsSinceEpoch();
    QByteArray farray, sarray;
    quint64 firstCode = C_COUNT^count^ctime;
    quint64 secondeCode = C_TIME^mac^ctime;
    for (int index = 0; index < 8; ++index) {
        farray.append(char(firstCode>>8*(7-index)));
        sarray.append(char(secondeCode>>8*(7-index)));
    }
    return (farray + sarray);
}

QByteArray IdCheck::getCode(const quint16 &count, QString mac)
{
    return getCode(count, removeSymbol(mac).toULongLong(&ok, 16));
}

ZipError IdCheck::modifyConfig(const QByteArray &code, const quint64 &mac)
{
    qint64 ctime = QDateTime::currentMSecsSinceEpoch();
    // 无效
    if (code.size() != 16)
        return ZipError::CodeError;
    quint16 count = getCodeCount(code, mac);
    qint64 time = getCodeTime(code, mac);
    if (count == 0 || ctime <= time)
        return ZipError::CodeError;
    // 过期
    if (ctime - time > 5*24*60*60*1000)
        return ZipError::Overdue;
    ConfigFilesTxt cfTxt = getConfigFilesTxt();
    // 已使用
    if (!cfTxt.secretkey.filter(QString::number(time)).isEmpty())
        return ZipError::Used;
    cfTxt.secretkey<<QString("%1\n").arg(time);
    // 增加耗材数量
    updateConsumablesCount(savedConsumables(cfTxt.config) + count, cfTxt.secretkey);
    return ZipError::Validity;
}

ZipError IdCheck::modifyConfig(const QByteArray &code, QString mac)
{
    return modifyConfig(code, removeSymbol(mac).toULongLong(&ok, 16));
}

void IdCheck::consumablesUsed()
{
    ConfigFilesTxt cfTxt = getConfigFilesTxt();
    int count = 0;
    if (savedConsumables(cfTxt.config) > 0) {
        count = savedConsumables(cfTxt.config) - 1;
    }
    updateConsumablesCount(count, cfTxt.secretkey);
}

qint64 IdCheck::getCodeTime(const QByteArray &code, const quint64 &mac)
{
    return C_TIME^mac^code.mid(8, 8).toHex().toULongLong(&ok, 16);
}

int IdCheck::savedConsumables(QString str)
{
    str.remove("consumables=");
    if (!str.isEmpty() && str.indexOf("[^0-9]") == -1) {
        return str.toInt();
    }
    return 0;
}

ConfigFilesTxt IdCheck::getConfigFilesTxt()
{
    QuaZip zip(zipName());
    // 文件丢失
    if (!zip.open(QuaZip::mdUnzip))
        return ConfigFilesTxt();
    QString config;
    QStringList secretkey;
    for (bool suc = zip.goToFirstFile(); suc; suc = zip.goToNextFile())
    {
        //实际的文件处理
        QuaZipFile zipFile;
        QString qsTempZipName = zip.getZipName();
        zipFile.setZipName(qsTempZipName);
        QString filename = zip.getCurrentFileName();
        zipFile.setFileName(filename);
        if (zipFile.open(QIODevice::ReadOnly, PASSWORD)) {
            if ("config.ini" == filename) {
                config = zipFile.readAll();
            }
            else if ("usedId.txt" == filename) {
                while (!zipFile.atEnd()) {
                    secretkey<<zipFile.readLine();
                }
            }
            zipFile.close();
        }
    }
    zip.close();
    return ConfigFilesTxt(config, secretkey);
}

QString IdCheck::zipName()
{
    return QCoreApplication::applicationDirPath() + "/config.zip";
}

void IdCheck::updateConsumablesCount(const int &count, const QStringList &usedIds)
{
    QMutexLocker locker(&mutex);
    // 压缩文件
    QuaZip zip(zipName());
    if (!zip.open(QuaZip::mdCreate))
    {
        qDebug("Could not create zip: %s", qPrintable(zipName()));
        return;
    }
    QuaZipFile configFile(&zip);
    if (configFile.open(QIODevice::WriteOnly,QuaZipNewInfo("config.ini"), PASSWORD, 0, 8)) {
        configFile.write(QString("consumables=%1").arg(count).toUtf8().constData());
        configFile.close();
    }
    QuaZipFile usedId(&zip);
    if (usedId.open(QIODevice::WriteOnly,QuaZipNewInfo("usedId.txt"), PASSWORD, 0, 8)) {
        foreach (QString line, usedIds) {
            usedId.write(line.toUtf8().constData());
        }
        usedId.close();
    }
    zip.close();
    SetFileAttributes(zipName().toStdWString().c_str(),FILE_ATTRIBUTE_HIDDEN);
    emit currentConsumables(count);
}
