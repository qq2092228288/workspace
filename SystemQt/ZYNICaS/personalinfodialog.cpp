#include "personalinfodialog.h"
#include "infoeditdialog.h"
#include "datamanagement.h"

PersonalInfoDialog::PersonalInfoDialog(QWidget *parent)
    : QDialog{parent},
      m_databaseName{"PatientInfo.db"},
      m_tableName{"patient"}
{
    setWindowTitle(tr("患者信息导入"));
    auto &instance = DataManagement::getInstance();
    setMinimumSize(650*instance.wZoom(), 400*instance.hZoom());

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QHBoxLayout *hLayout = new QHBoxLayout;
    comboBox = new QComboBox(this);
    searchEdit = new QLineEdit(this);
    searchBtn = new QPushButton(tr("查找"),this);
    tableView = new QTableView(this);
    deleteBtn = new QPushButton(tr("删除"),this);
    confirmBtn = new QPushButton(tr("确定"),this);

    comboBox->addItems(QStringList()<<instance.idName()<<tr("姓名"));
    comboBox->setFixedWidth(120*instance.hZoom());

    mainLayout->addLayout(hLayout);
    mainLayout->addWidget(tableView);
    hLayout->addWidget(comboBox);
    hLayout->addWidget(searchEdit);
    hLayout->addWidget(searchBtn);
    hLayout->addWidget(deleteBtn);
    hLayout->addStretch();
    hLayout->addWidget(confirmBtn);

    // 选中整行，不可编辑，单选
    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableView->setSelectionMode(QAbstractItemView::SingleSelection);

    // 建立和SQlite数据库的连接
    database = QSqlDatabase::addDatabase("QSQLITE", m_tableName);
    // 设置数据库文件的名字
    database.setDatabaseName(m_databaseName);
    if (!database.open()) {
//        qDebug() << "Error: Failed to connect database." << database.lastError();
    }
    else {
        // 用于执行sql语句的对象
        QSqlQuery sqlQuery(database);
        // 构建创建数据库的sql语句字符串
        QString createSql = QString("CREATE TABLE %1 ("
                                    "id TEXT PRIMARY KEY NOT NULL,"
                                    "name TEXT NOT NULL,"
                                    "sex TEXT NOT NULL,"
                                    "age INT NOT NULL,"
                                    "height INT NOT NULL,"
                                    "weight INT NOT NULL,"
                                    "fhh INT NOT NULL DEFAULT 0,"
                                    "edh INT NOT NULL DEFAULT 0,"
                                    "ltsh INT NOT NULL DEFAULT 0,"
                                    "lthms INT NOT NULL DEFAULT 0,"
                                    "ptm INT NOT NULL DEFAULT 0,"
                                    "al INT NOT NULL DEFAULT 0)").arg(m_tableName);
        sqlQuery.prepare(createSql);
        // 执行sql语句
        if(!sqlQuery.exec()) {
//            qDebug() << "Error: Fail to create table. " << sqlQuery.lastError();
            sqlQuery.exec(QString("SELECT * FROM %1").arg(m_tableName));
            if (6 == sqlQuery.record().count()) {
                auto newColumns = QStringList()<<"fhh"<<"edh"<<"ltsh"<<"lthms"<<"ptm"<<"al";
                foreach (auto name, newColumns) {
                    sqlQuery.exec(QString("ALTER TABLE %1 ADD COLUMN %2 INT DEFAULT 0")
                                  .arg(m_tableName, name));
                }
            }
        }
        else {
//            qDebug() << "Table created!";
        }
        m_model = new QSqlTableModel(this, database);
        // 查表
        m_model->setTable("patient");
        m_model->select();
        // 列名
        m_model->setHeaderData(m_model->fieldIndex("id"), Qt::Horizontal, instance.idName());
        m_model->setHeaderData(m_model->fieldIndex("name"), Qt::Horizontal, "姓名");
        m_model->setHeaderData(m_model->fieldIndex("sex"), Qt::Horizontal, "性别");
        m_model->setHeaderData(m_model->fieldIndex("age"), Qt::Horizontal, "年龄");
        m_model->setHeaderData(m_model->fieldIndex("height"), Qt::Horizontal, "身高(cm)");
        m_model->setHeaderData(m_model->fieldIndex("weight"), Qt::Horizontal, "体重(kg)");

        m_model->setHeaderData(m_model->fieldIndex("fhh"), Qt::Horizontal, "高血压家族史");
        m_model->setHeaderData(m_model->fieldIndex("edh"), Qt::Horizontal, "过量饮酒史");
        m_model->setHeaderData(m_model->fieldIndex("ltsh"), Qt::Horizontal, "长期吸烟史");
        m_model->setHeaderData(m_model->fieldIndex("lthms"), Qt::Horizontal, "长期精神紧张史");
        m_model->setHeaderData(m_model->fieldIndex("ptm"), Qt::Horizontal, "坚持服药");
        m_model->setHeaderData(m_model->fieldIndex("al"), Qt::Horizontal, "活动受限");

        tableView->setModel(m_model);
        tableView->setColumnWidth(0, 120);
    }
    connect(searchBtn,&QPushButton::clicked,this,&PersonalInfoDialog::searchSlot);
    connect(tableView,&QTableView::doubleClicked,this,&PersonalInfoDialog::emitPatientInfo);
    connect(deleteBtn,&QPushButton::clicked,this,&PersonalInfoDialog::deletePatientInfo);
    connect(confirmBtn,&QPushButton::clicked,this,&PersonalInfoDialog::emitPatientInfo);
}

void PersonalInfoDialog::showEvent(QShowEvent *event)
{
    searchEdit->clear();
    m_model->setFilter(nullptr);
    m_model->select();
    event->accept();
}

void PersonalInfoDialog::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return){
        searchSlot();
    }else {
        event->accept();
    }
}

void PersonalInfoDialog::insertData(BodyValue &bValue)
{
    QSqlQuery sqlQuery(database);
    sqlQuery.prepare(QString("REPLACE INTO %1(id, name, sex, age, height, weight, fhh, edh, ltsh, lthms, ptm, al) "
                     "VALUES(:id, :name, :sex, :age, :height, :weight, :fhh, :edh, :ltsh, :lthms, :ptm, :al)")
                     .arg(m_tableName));
    sqlQuery.bindValue(":id", bValue.id);
    sqlQuery.bindValue(":name", bValue.name);
    if (bValue.sex == 0) {
        sqlQuery.bindValue(":sex", tr("男"));
    }
    else {
        sqlQuery.bindValue(":sex", tr("女"));
    }
    sqlQuery.bindValue(":age", bValue.age);
    sqlQuery.bindValue(":height", bValue.height);
    sqlQuery.bindValue(":weight", bValue.weight);

    sqlQuery.bindValue(":fhh", bValue.fhh);
    sqlQuery.bindValue(":edh", bValue.edh);
    sqlQuery.bindValue(":ltsh", bValue.ltsh);
    sqlQuery.bindValue(":lthms", bValue.lthms);
    sqlQuery.bindValue(":ptm", bValue.ptm);
    sqlQuery.bindValue(":al", bValue.al);
    if(!sqlQuery.exec()) {
//        qDebug() << "Error: Fail to insert data. " << sqlQuery.lastError();
    }
    else {
        // do something
        m_model->select();
    }
}

void PersonalInfoDialog::searchSlot()
{
    if (searchEdit->text().isEmpty()) {
        m_model->setFilter(nullptr);
    }
    else {
        QString colName = "id";
        if (comboBox->currentIndex() == 1) {
            colName = "name";
        }
        m_model->setFilter(QString("%1 like '%%2%'").arg(colName,searchEdit->text()));
    }
    m_model->select();
}

void PersonalInfoDialog::deletePatientInfo()
{
    if (!tableView->currentIndex().isValid()) {
        QMessageBox::information(this,tr("提示:"),tr("请选择一条信息。"));
        return;
    }
    QString id = m_model->data(m_model->index(tableView->currentIndex().row(),0)).toString();
    QSqlQuery sqlQuery(database);
    sqlQuery.prepare(QString("DELETE FROM %1 WHERE id='%2'").arg(m_tableName, id));
    if(!sqlQuery.exec()) {
//        qDebug()<<id<<sqlQuery.lastError();
    }
    else {
        m_model->select();
    }
}

void PersonalInfoDialog::emitPatientInfo()
{
    if (!tableView->currentIndex().isValid()) {
        QMessageBox::warning(this,tr("警告！"),tr("请选择一条信息！"));
        return;
    }
    int row = tableView->currentIndex().row();
    QString id = m_model->data(m_model->index(row,0)).toString();
    QString name = m_model->data(m_model->index(row,1)).toString();
    int sex = 0;
    if (m_model->data(m_model->index(row,2)).toString() == tr("女")) {
        sex = 1;
    }
    int age = m_model->data(m_model->index(row,3)).toInt();
    int height = m_model->data(m_model->index(row,4)).toInt();
    int weight = m_model->data(m_model->index(row,5)).toInt();

    int fhh = m_model->data(m_model->index(row, 6)).toInt();
    int edh = m_model->data(m_model->index(row, 7)).toInt();
    int ltsh = m_model->data(m_model->index(row, 8)).toInt();
    int lthms = m_model->data(m_model->index(row, 9)).toInt();
    int ptm = m_model->data(m_model->index(row, 10)).toInt();
    int al = m_model->data(m_model->index(row, 11)).toInt();

    emit patientInfo(id, name, sex, age, height, weight,
                     fhh, edh, ltsh, lthms, ptm, al);
    this->close();
}
