#ifndef PERSONALINFODIALOG_H
#define PERSONALINFODIALOG_H

#include <QDialog>
#include <QTableView>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QLayout>
#include <QPushButton>
#include <QDebug>
#include <QMessageBox>
#include <QLineEdit>
#include <QComboBox>

struct BodyValue;
class PersonalInfoDialog : public QDialog
{
    Q_OBJECT
public:
    explicit PersonalInfoDialog(QWidget *parent = nullptr);
    void showEvent(QShowEvent *event);
    void keyPressEvent(QKeyEvent *event);
    // 插入数据
    void insertData(BodyValue &bValue);
signals:
    // 病历号，姓名，性别 0男 1女，年龄，身高，体重
    void patientInfo(QString, QString, int, int, int, int, int, int, int, int, int, int);
private slots:
    void searchSlot();
    void deletePatientInfo();
    void emitPatientInfo();
private:
    const QString m_databaseName;
    const QString m_tableName;
    QSqlDatabase database;
    QComboBox *comboBox;
    QLineEdit *searchEdit;
    QPushButton *searchBtn;
    QPushButton *deleteBtn;
    QPushButton *confirmBtn;
    QTableView *tableView;
    QSqlTableModel *m_model;
};

#endif // PERSONALINFODIALOG_H
