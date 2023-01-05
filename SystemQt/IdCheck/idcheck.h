#ifndef IDCHECK_H
#define IDCHECK_H

#include <QtCore/qglobal.h>
#include <QObject>
#include <QDateTime>
#include <QDir>
#include <QDebug>
#include <QMutexLocker>
#include <QCryptographicHash>

#if defined(IDCHECK_LIBRARY)
#  define IDCHECK_EXPORT Q_DECL_EXPORT
#else
#  define IDCHECK_EXPORT Q_DECL_IMPORT
#endif

enum ZipError
{
    Validity,           // 有效的
    CodeError,          // 码错误
    Overdue,            // 过期
    Used,               // 已使用
};

struct ConfigFilesTxt
{
    ConfigFilesTxt(){}
    ConfigFilesTxt(QString _c, QStringList _s):config(_c),secretkey(_s){}
    QString config;
    QStringList secretkey;
};

class IDCHECK_EXPORT IdCheck : public QObject
{
    Q_OBJECT
public:
    explicit IdCheck(QObject *parent = nullptr);
public slots:
    // 获取当前耗材数量
    static int getCurrentConsumables();
    static QString removeSymbol(QString str);
    static QByteArray strToByteArray(QString str);
    // code的count
    static quint16 getCodeCount(const QByteArray &code, const quint64 &mac);
    static quint16 getCodeCount(const QByteArray &code, QString mac);
    // 生成码
    static QByteArray getCode(const quint16 &count, const quint64 &mac);
    static QByteArray getCode(const quint16 &count, QString mac);
    // 更新耗材配置
    ZipError modifyConfig(const QByteArray &code, const quint64 &mac);
    ZipError modifyConfig(const QByteArray &code, QString mac);
    // 生成报告
    void consumablesUsed();
signals:
    void currentConsumables(int);
private:
    QMutex mutex;
    static qint64 getCodeTime(const QByteArray &code, const quint64 &mac);
    static int savedConsumables(QString str);
    static ConfigFilesTxt getConfigFilesTxt();
    static QString zipName();
    void updateConsumablesCount(const int &count, const QStringList &usedIds);
};

#endif // IDCHECK_H
