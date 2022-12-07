#include "reportdatabase.h"
#include "datamanagement.h"

ReportDataBase::ReportDataBase(QObject *parent)
    : QObject{parent},
      m_databaseName{"Reports.db"},
      m_tableName{"reports"}
{
    // 建立和SQlite数据库的连接
    m_database = QSqlDatabase::addDatabase("QSQLITE", m_tableName);
    // 设置数据库文件的名字
    m_database.setDatabaseName(m_databaseName);
    if (m_database.open()) {
        // 用于执行sql语句的对象
        QSqlQuery sqlQuery(m_database);
        // 构建创建数据库的sql语句字符串
        QString createSql = QString("CREATE TABLE %1(time REAL PRIMARY KEY NOT NULL, upload INT, data TEXT)")
                            .arg(m_tableName);
        sqlQuery.prepare(createSql);
        // 执行sql语句
        sqlQuery.exec();
    }
    else {
        qWarning("数据库打开失败！");
    }

    connect(DataManagement::getInstance().httpPost(), &HttpPost::finished, this, &ReportDataBase::dataUploaded);
}

ReportDataBase::~ReportDataBase()
{

}

void ReportDataBase::insert(qint64 time, int upload, QString dataString)
{
    QSqlQuery sqlQuery(m_database);
    sqlQuery.prepare(QString("INSERT INTO %1(time, upload, data) VALUES(:time, :upload, :data)").arg(m_tableName));
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
    sqlQuery.exec(QString("SELECT * FROM %1 WHERE upload=0").arg(m_tableName));
    while (sqlQuery.next()) {
        emit upload(sqlQuery.value(0).toLongLong(), sqlQuery.value(2).toString());
    }
}

void ReportDataBase::deleteUploaded()
{
//    QSqlQuery sqlQuery(m_database);
//    sqlQuery.prepare(QString("DELETE FROM reports WHERE time<%1 and upload=1")
//                     .arg(QDateTime::currentSecsSinceEpoch() - 360*24*60*60));
//    sqlQuery.exec();
}

void ReportDataBase::dataUploaded(const qint64 &time)
{
    QSqlQuery sqlQuery(m_database);
    if (!sqlQuery.exec(QString("UPDATE %1 SET upload=1 WHERE time=%2").arg(m_tableName).arg(time))) {
        qWarning("数据更新失败！");
    }
}
