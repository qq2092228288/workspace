#ifndef SHOWREPORTDIALOG_H
#define SHOWREPORTDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QListView>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDir>
#include <QThread>
#include <QPrintDialog>
#include <QPrinter>
#include <QPrinterInfo>
#include <QTextDocument>
#include <QCalendarWidget>
#include <QMessageBox>
#include <QPrintPreviewDialog>
#include <QAxWidget>
#include <QGroupBox>

class MyModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit MyModel(QStringList &list, QObject *parent = nullptr);

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    int rowCount(const QModelIndex &parent = QModelIndex()) const;

    //    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;

    //    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    //    QModelIndex parent(const QModelIndex &index) const;

    void update();
private:
    QStringList &m_list;
};

class MyLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    explicit MyLineEdit(QWidget *parent = nullptr);
protected:
    void mousePressEvent(QMouseEvent *event);
signals:
    void mousePress();
private:

};

class CalendarDialog : public QDialog
{
    Q_OBJECT
public:
    explicit CalendarDialog(QWidget *parent = nullptr);
    virtual ~CalendarDialog();
signals:
    void currentDate(const QDate &);
private:
    QHBoxLayout *hLayout;
    QCalendarWidget *calendarWidget;
};

class MyListView : public QListView
{
    Q_OBJECT
public:
    explicit MyListView(QWidget *parent = nullptr);
    QString currentPath() const;
protected:
    void mouseDoubleClickEvent(QMouseEvent *event);
signals:
    // Absolute path
    void doubleClick(QString);
};

enum PrintError
{
    NoError = 0,
    InvalidPath = 1,
    NoPrinter = 2,
    WinApiError = 3
};

class ShowReportDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ShowReportDialog(QWidget *parent = nullptr);
    virtual ~ShowReportDialog();
    void showEvent(QShowEvent *event);
    QStringList allReportNames();
public slots:
    void showCalendarWidget();
    void filter();
    void printer();
    static int printWord(const QString &fileName);
signals:

private:
    QGroupBox *funcGroupBox;
    QLabel *searchLabel;
    QComboBox *comboBox;
    MyLineEdit *lineEdit;
    CalendarDialog *calendarDialog;
    QPushButton *searchBtn;
    QPushButton *printBtn;
    MyListView *listView;
    MyModel *model;
    QStringList fileNames;
};

#endif // SHOWREPORTDIALOG_H
