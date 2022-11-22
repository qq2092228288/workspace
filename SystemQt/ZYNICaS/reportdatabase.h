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
    virtual ~ReportDataBase();
public slots:
    void insert(qint64 time, int upload, QString dataString);
    void dataUpload();
    void dataUploaded(const qint64 &time);
    void deleteUploaded();
signals:
private:
    QSqlDatabase m_database;
    HttpPost *httpPost;
};

#endif // REPORTDATABASE_H
