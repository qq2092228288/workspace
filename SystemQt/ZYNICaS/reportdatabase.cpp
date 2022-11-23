#include "reportdatabase.h"
#include "httppost.h"

ReportDataBase::ReportDataBase(QObject *parent)
    : QObject{parent}
{
    // 连接数据库
    if (QSqlDatabase::contains("qt_sql_default_connection")) {
        m_database = QSqlDatabase::database("qt_sql_default_connection");
    }
    else {
        // 建立和SQlite数据库的连接
        m_database = QSqlDatabase::addDatabase("QSQLITE");
        // 设置数据库文件的名字
        m_database.setDatabaseName("PersonalInfoDataBase.db");
    }
    if (m_database.open()) {
        // 用于执行sql语句的对象
        QSqlQuery sqlQuery(m_database);
        // 构建创建数据库的sql语句字符串
        QString createSql = QString("CREATE TABLE reports(time REAL PRIMARY KEY NOT NULL, upload INT, data TEXT)");
        sqlQuery.prepare(createSql);
        // 执行sql语句
        sqlQuery.exec();
    }
    else {
        qWarning("数据库打开失败！");
    }
    m_pHttpPost = new HttpPost(this);
    connect(this, &ReportDataBase::unupload, m_pHttpPost, &HttpPost::reportUpload);
    connect(m_pHttpPost, &HttpPost::finished, this, &ReportDataBase::dataUploaded);
}

void ReportDataBase::insert(qint64 time, int upload, QString dataString)
{
    QSqlQuery sqlQuery(m_database);
    sqlQuery.prepare("INSERT INTO reports(time, upload, data) VALUES(:time, :upload, :data)");
    sqlQuery.bindValue(":time", time);
    sqlQuery.bindValue(":upload", upload);
    sqlQuery.bindValue(":data", dataString);
    if (!sqlQuery.exec()) {
        qWarning("数据插入失败！");
    }
}

void ReportDataBase::dataUpload()
{
    QSqlQuery sqlQuery(m_database);
    sqlQuery.exec("SELECT * FROM reports");
    while (sqlQuery.next()) {
        qDebug()<<sqlQuery.value(0).toLongLong()<<sqlQuery.value(1).toInt()<<sqlQuery.value(2).toString().size();
        if (sqlQuery.value(1).toInt() == 0) {
            emit unupload(sqlQuery.value(0).toLongLong(), sqlQuery.value(2).toString());
        }
    }
}

void ReportDataBase::deleteUploaded()
{
    QSqlQuery sqlQuery(m_database);
    sqlQuery.prepare(QString("DELETE FROM reports WHERE time<%1 and upload=1")
                     .arg(QDateTime::currentSecsSinceEpoch() - 360*24*60*60));
    sqlQuery.exec();
}

void ReportDataBase::dataUploaded(const qint64 &time)
{
    QSqlQuery sqlQuery(m_database);
    if (!sqlQuery.exec(QString("UPDATE reports SET upload=1 WHERE time=%1").arg(time))) {
        qWarning("数据更新失败！");
    }
}
