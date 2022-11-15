#include "reportdatabase.h"
#include "httppost.h"
#include "MyStruct.h"

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
        QSqlQuery sqlQuery;
        // 构建创建数据库的sql语句字符串
        QString createSql = QString("CREATE TABLE reports(time REAL PRIMARY KEY NOT NULL, upload INT, data TEXT)");
        sqlQuery.prepare(createSql);
        // 执行sql语句
        sqlQuery.exec();
    }
    else {
        qWarning("数据库打开失败！");
    }
}

ReportDataBase::~ReportDataBase()
{
    if (m_database.isOpen()) {
        m_database.close();
    }
}

void ReportDataBase::insert(qint64 time, int upload, QString dataString)
{
    QSqlQuery sqlQuery;
    sqlQuery.prepare("INSERT INTO reports(time, upload, data) VALUES(:time, :upload, :data)");
    sqlQuery.bindValue(":time", time);
    sqlQuery.bindValue(":upload", upload);
    sqlQuery.bindValue(":data", dataString);
    sqlQuery.exec();
}

void ReportDataBase::ergodic()
{
    QSqlQuery sqlQuery;
    sqlQuery.exec("SELECT * FROM reports");
    while (sqlQuery.next()) {
        qDebug()<<sqlQuery.value(0).toLongLong()
               <<sqlQuery.value(1).toInt()
              <<sqlQuery.value(2).toString().size();
        qDebug()<<BaseData::strToJson(sqlQuery.value(2).toString());
//        HttpPost *httpPost = new HttpPost(this);
//        httpPost->reportUpload(sqlQuery.value(2).toString());
//        connect(httpPost, &HttpPost::finished, httpPost, &HttpPost::deleteLater);
    }
}
