#ifndef REPORTDATABASE_H
#define REPORTDATABASE_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QDebug>

class HttpPost;
class ReportDataBase : public QObject
{
    Q_OBJECT
public:
    explicit ReportDataBase(QObject *parent = nullptr);
public slots:
    void insert(qint64 time, int upload, QString dataString);
    void dataUpload();
    void deleteUploaded();
private slots:
    void dataUploaded(const qint64 &time);
signals:
    void unupload(const qint64 &time, const QString &data);
private:
    QSqlDatabase m_database;
    HttpPost *m_pHttpPost;
};

#endif // REPORTDATABASE_H
