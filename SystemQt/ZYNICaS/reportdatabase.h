#ifndef REPORTDATABASE_H
#define REPORTDATABASE_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QDebug>

class ReportDataBase : public QObject
{
    Q_OBJECT
public:
    explicit ReportDataBase(QObject *parent = nullptr);
    virtual ~ReportDataBase();
public slots:
    void insert(qint64 time, int upload, QString dataString);
    void dataUpload();
private slots:
    void dataUploaded(const qint64 &time);
signals:
private:
    QSqlDatabase m_database;
};

#endif // REPORTDATABASE_H
