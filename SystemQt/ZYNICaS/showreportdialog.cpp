#include "showreportdialog.h"
#include "datamanagement.h"
#include <QDesktopServices>
#include <shellapi.h>


MyModel::MyModel(QStringList &list, QObject *parent)
    : QAbstractListModel{parent},m_list{list}
{

}

QVariant MyModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }
    int row = index.row();
    if (row >= rowCount() || row < 0) {
        return QVariant();
    }
    switch (role) {
    case Qt::DisplayRole:
        return m_list.at(row);
        break;
    }
    return QVariant();
}

int MyModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_list.count();
}

void MyModel::update()
{
    this->beginResetModel();
    this->endResetModel();
}

MyLineEdit::MyLineEdit(QWidget *parent)
    :QLineEdit{parent}
{

}

void MyLineEdit::mousePressEvent(QMouseEvent *event)
{
    event->accept();
    emit mousePress();
}

CalendarDialog::CalendarDialog(QWidget *parent)
    :QDialog{parent}
{
    auto &instance = DataManagement::getInstance();
    resize(300*instance.wZoom(),250*instance.hZoom());
    // 任务栏图标隐藏，无边框，去除阴影
    setWindowFlags(Qt::Popup | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    hLayout = new QHBoxLayout(this);
    calendarWidget = new QCalendarWidget(this);
    hLayout->addWidget(calendarWidget);

    connect(calendarWidget,&QCalendarWidget::clicked,this,[=](const QDate &date){
        emit currentDate(date);
        this->close();
    });
}

CalendarDialog::~CalendarDialog()
{
    delete calendarWidget;
}

MyListView::MyListView(QWidget *parent)
    :QListView{parent}
{
    QFont font = this->font();
    font.setPointSize(12);
    setFont(font);
}

QString MyListView::currentPath() const
{
    return (DataManagement::getInstance().getPaths().reports()
            + "/" + currentIndex().data().toString());
}

void MyListView::mouseDoubleClickEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    if (!currentIndex().isValid())
        return;
    emit doubleClick(currentPath());
}

ShowReportDialog::ShowReportDialog(QWidget *parent)
    : QDialog{parent}
{
    this->setWindowTitle(tr("查看报告"));
    auto &instance = DataManagement::getInstance();
    setMinimumSize(500*instance.wZoom(), 600*instance.hZoom());
    setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
    //样式表
    this->setStyleSheet(instance.dialogQss(1));

    funcGroupBox = new QGroupBox(this);
    searchLabel = new QLabel(tr("查找条件:"), this);
    comboBox = new QComboBox(this);
    lineEdit = new MyLineEdit(this);
    calendarDialog = new CalendarDialog;
    searchBtn = new QPushButton(tr("搜索"), this);
    printBtn = new QPushButton(tr("打印"), this);
    listView = new MyListView(this);
    countLabel = new QLabel(this);
    model = new MyModel(fileNames,this);

    comboBox->setFixedWidth(100*instance.hZoom());
    listView->setModel(model);
    funcGroupBox->setStyleSheet("QGroupBox\
                                {\
                                    border:2px solid #ffffff;\
                                    color:#000000;\
                                    border-radius:3px;\
                                }");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QHBoxLayout *ctrlLayout = new QHBoxLayout(funcGroupBox);

    mainLayout->addWidget(funcGroupBox);
    ctrlLayout->addWidget(searchLabel);
    ctrlLayout->addWidget(comboBox);
    ctrlLayout->addWidget(lineEdit);
    ctrlLayout->addWidget(searchBtn);
    ctrlLayout->addWidget(printBtn);
    mainLayout->addWidget(listView);
    mainLayout->addWidget(countLabel, 0, Qt::AlignRight);

    comboBox->addItems(QStringList()<<tr("关键字")<<tr("时间"));

    connect(lineEdit, &MyLineEdit::mousePress, this, &ShowReportDialog::showCalendarWidget);
    connect(calendarDialog, &CalendarDialog::currentDate, this, [=](const QDate &date){
        lineEdit->setText(date.toString("yyyyMMdd"));
    });
    connect(searchBtn, &QPushButton::clicked, this, &ShowReportDialog::filter);
    connect(printBtn, &QPushButton::clicked, this, &ShowReportDialog::printer);
    // 双击打开
    connect(listView, &MyListView::doubleClick, this, [=](QString path){
        if (!QDesktopServices::openUrl(QUrl::fromLocalFile(path))) {
            QMessageBox::warning(this, tr("错误！"), tr("文件不存在！"));
        }
    });
}

ShowReportDialog::~ShowReportDialog()
{
    delete calendarDialog;
//    qDebug()<<"~ShowReportDialog()";
}

void ShowReportDialog::showEvent(QShowEvent *event)
{
    event->accept();
    lineEdit->clear();
    fileNames = allReportNames();
    model->update();
    countLabel->setText(tr("报告总计：%1 份").arg(fileNames.size()));
}

QStringList ShowReportDialog::allReportNames()
{
    QDir dir(DataManagement::getInstance().getPaths().reports());
    return dir.entryList(QStringList()<<"*.docx", QDir::Files | QDir::NoSymLinks, QDir::Name);
}

void ShowReportDialog::showCalendarWidget()
{
    if (comboBox->currentIndex() == 1) {
        int x = pos().x()+lineEdit->pos().x()+lineEdit->width()/2-calendarDialog->width()/2;
        int y = pos().y()+80*DataManagement::getInstance().hZoom();
        calendarDialog->move(x,y);
        calendarDialog->exec();
    }
}

void ShowReportDialog::filter()
{
    if (allReportNames().isEmpty())
        return;
    QString strRegExp = lineEdit->text();
    if (comboBox->currentIndex() == 1) {
        strRegExp = QString("^%1[\\d]{4}-[\\w]{1,}-.+\\.docx$").arg(lineEdit->text());
    }
    if (strRegExp.isEmpty()) {
        fileNames = allReportNames();
    }
    else {
        fileNames = allReportNames().filter(QRegExp(strRegExp));
    }
    model->update();
}

void ShowReportDialog::printer()
{
    if (!listView->currentIndex().isValid()) {
        QMessageBox::information(this,tr("提示"),tr("选择一个文件。"));
        return;
    }
    auto &instance = DataManagement::getInstance();
    instance.reportPrintOut(listView->currentPath());
}

int ShowReportDialog::printWord(const QString &fileName)
{
    QPrinter printer;
    QString printerName = printer.printerName();
    // no default printer or no file return false
    if( printerName.size() == 0 || fileName.size() == 0) {
        return PrintError::NoPrinter;
    }
    QFileInfo file(fileName);
    QString filePath = file.absoluteFilePath();
    if(!file.exists()) {
        return PrintError::InvalidPath;
    }
    intptr_t ret = reinterpret_cast<intptr_t>
            (ShellExecuteW(NULL,                                    /*父窗口*/
                           QString("print").toStdWString().c_str(), /*字符串指示操作行为*/
                           filePath.toStdWString().c_str(),         /*文件或文件夹路径*/
                           NULL,                                    /*如果是可执行文件，这里作为其参数*/
                           NULL,                                    /*被执行文件的工作路径*/
                           SW_HIDE));                               /*打开的文件窗口显示模式*/
    // if return number bigger than 31 indicate succ
    if (ret <= SE_ERR_NOASSOC) {
        return PrintError::WinApiError;
    }
    return PrintError::NoError;
}
