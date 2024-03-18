#include "reportdialog.h"
#include "ui/reportdialogui.h"
#include <QMessageBox>
#include <QDateTime>
#include <QDir>
#include <QProcess>
#include <QDesktopServices>
#include <QApplication>

#include "tcpclientsocket.h"
#include "singleton.h"
#include "databasens.h"
#include "reportdataname.h"


#include "reportgraphicsitem.h"
#include "printgraphicsview.h"
#include <QPageSize>
#include <QScreen>

using namespace DatabaseEnumNs;

ReportDialog::ReportDialog(QWidget *parent)
    : QDialog{parent},
      ui{new Ui::ReportDialog},
      m_path{QCoreApplication::applicationDirPath() + "/reports"}
{
    ui->setupUi(this);

    QDir dir(m_path);
    if (!dir.exists()) {
        dir.mkpath(m_path);
    }
    calendarDialog = new ScopeCalendarDialog(this);
    waitingDialog = new WaitingDialog(this);
    model = new ReportTableModel(this);

    ui->reportTableView->setModel(model);
    ui->reportTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->reportTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->reportTableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->reportTableView->setColumnWidth(0, 150);
    ui->reportTableView->setColumnWidth(1, 150);
    ui->reportTableView->horizontalHeader()->setStretchLastSection(true);

    connect(ui->searchCriteriaComboBox, QOverload<int>::of(&QComboBox::activated),
            this, &ReportDialog::searchCriteriaComboBoxActivated);
    connect(calendarDialog, &ScopeCalendarDialog::scopeTime, model, &ReportTableModel::filterTime);
    connect(ui->searchButton, &QPushButton::clicked, this, &ReportDialog::searchButtonClicked);
    connect(ui->resetButton, &QPushButton::clicked, this, &ReportDialog::resetButtonClicked);
    connect(ui->generatePdfButton, &QPushButton::clicked, this, &ReportDialog::generatePdfButtonClicked);
    connect(ui->openReportDirButton, &QPushButton::clicked, this, &ReportDialog::openReportDirButtonClicked);
    connect(ui->printPreviewButton, &QPushButton::clicked, this, &ReportDialog::printPreviewButtonClicked);
    connect(ui->reportTableView, &QTableView::doubleClicked, this, &ReportDialog::reportTableViewDoubleClicked);
    auto instance = TcpClientSocket::getInstance();
    connect(instance, &TcpClientSocket::reportPulled, this, &ReportDialog::reportPulled);
    connect(ui->getLatestDataButton, &QPushButton::clicked, instance, &TcpClientSocket::getNewReports);
    connect(instance, &TcpClientSocket::reportReady, waitingDialog, &WaitingDialog::start);
    connect(instance, &TcpClientSocket::reportPulled, waitingDialog, &WaitingDialog::tryStop);
}

ReportDialog::~ReportDialog()
{
    delete ui;
}

void ReportDialog::setConfig(const ReportConfig &config)
{
    m_config = config;
}

void ReportDialog::searchCriteriaComboBoxActivated(int index)
{
    if (2 == index) {
        ui->searchCriteriaLineEdit->setEnabled(false);
        calendarDialog->exec();
    }
    else {
        ui->searchCriteriaLineEdit->setEnabled(true);
    }
}

void ReportDialog::searchButtonClicked()
{
    auto index = ui->searchCriteriaComboBox->currentIndex();
    if (0 == index) {
        model->filterPatientName(ui->searchCriteriaLineEdit->text());
    }
    else if (1 == index) {
        model->filterMedicalRecordNumber(ui->searchCriteriaLineEdit->text());
    }
}

void ReportDialog::resetButtonClicked()
{
    ui->searchCriteriaLineEdit->clear();
    model->updateList(getItems());
}

void ReportDialog::reportPulled()
{
    model->updateList(getItems());
}

void ReportDialog::generatePdfButtonClicked()
{
    auto index = ui->reportTableView->currentIndex();
    if (indexIsValid(index)) {
        auto object = getReportJson(index);
        if (!object.isEmpty()) {
            auto patientInfo = object.value(ReportDataName::ekey(ReportDataName::patientInfo)).toObject();
            auto fileName = m_path + "/"
                    + selectedTime(index).toString("yyyyMMddhhmmss")
                    + "-"
                    + patientInfo.value(ReportDataName::ekey(ReportDataName::medicalRecordNumber)).toString()
                    + "-"
                    + patientInfo.value(ReportDataName::ekey(ReportDataName::patientName)).toString()
                    + ".pdf";
            QFileInfo fileInfo(fileName);
            if (fileInfo.isFile()) {
                if (QMessageBox::question(this, "提示", "此报告已生成PDF文件，是否重新生成？") == QMessageBox::No) {
                    QDesktopServices::openUrl(QUrl::fromLocalFile(fileName));
                    return;
                }
            }
            QPrinter printer(QPrinter::ScreenResolution);
            PrintPreviewDialog::initPrinter(&printer);
            printer.setOutputFormat(QPrinter::PdfFormat);
            printer.setOutputFileName(fileName);
            ReportPainter painter(PrintPreviewDialog::reportSet(m_config, object),
                                  &printer);
            QDesktopServices::openUrl(QUrl::fromLocalFile(fileName));
        }
        else {
            QMessageBox::warning(this, "警告", "报告不存在！");
        }
    }
}

void ReportDialog::openReportDirButtonClicked()
{
    const QString explorer = "explorer";
    QStringList param;
    if(!QFileInfo(m_path).isDir()){
        param<<QLatin1String("/select,");
    }
    param<<QDir::toNativeSeparators(m_path);
    QProcess::startDetached(explorer, param);
}

void ReportDialog::printPreviewButtonClicked()
{
    auto index = ui->reportTableView->currentIndex();
    if (indexIsValid(index)) {
        reportTableViewDoubleClicked(index);
    }
}

void ReportDialog::reportTableViewDoubleClicked(const QModelIndex &index)
{
    auto object = getReportJson(index);
    if (!object.isEmpty()) {
        // 报告预览
#if 1
        QPrinter printer(QPrinter::ScreenResolution);
        PrintPreviewDialog dialog(m_config, object, &printer, this);
        dialog.exec();
#else
        QDialog dialog(this);
        dialog.resize(screen()->availableSize());
        dialog.setWindowFlags(Qt::Dialog | Qt::WindowMinimizeButtonHint |
                              Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint);
        QGraphicsScene scene;
        auto view = new PrintGraphicsView(m_config.samepage, object, &scene, &dialog);
        auto psize = QPageSize::sizePixels(QPageSize::A4, screen()->logicalDotsPerInch());
        scene.setItemIndexMethod(QGraphicsScene::NoIndex);
        if (!m_config.samepage && object.value(ReportDataName::ekey(ReportDataName::position)).toArray().size() > 1) {
            // 分页
            auto item0 = new ReportGraphicsItem(psize, object, m_config.samepage, PageType::MainPage_0);
            auto item1 = new ReportGraphicsItem(psize, object, m_config.samepage, PageType::MainPage_1);
            auto imagePage = new ReportGraphicsItem(psize, object, m_config.samepage, PageType::ImagePage);
            item1->setPos(0, item0->boundingRect().height());
            imagePage->setPos(0, item0->boundingRect().height() * 2);
            scene.addItem(item0);
            scene.addItem(item1);
            scene.addItem(imagePage);
        }
        else {
            // 不分页
            auto item = new ReportGraphicsItem(psize, object, m_config.samepage, PageType::MainPage_0);
            auto imagePage = new ReportGraphicsItem(psize, object, m_config.samepage, PageType::ImagePage);
            imagePage->setPos(0, item->boundingRect().height());
            scene.addItem(item);
            scene.addItem(imagePage);
        }
        foreach (auto item, scene.items()) {
            auto ritem = dynamic_cast<ReportGraphicsItem *>(item);
            connect(view, &PrintGraphicsView::startRecache, ritem, &ReportGraphicsItem::startRecache);
            connect(view, &PrintGraphicsView::endRecache, ritem, &ReportGraphicsItem::endRecache);
        }
        view->resize(screen()->availableSize());
        dialog.showMaximized();
        dialog.exec();
#endif
    }
    else {
        QMessageBox::warning(this, "警告", "报告不存在！");
    }
}

QDateTime ReportDialog::selectedTime(const QModelIndex &index) const
{
    return model->data(model->index(index.row(), 0)).toDateTime();
}

QJsonObject ReportDialog::getReportJson(const QModelIndex &index)
{
    QSqlQuery query(TcpClientSocket::getInstance()->db());
    query.prepare(QString("SELECT %1 FROM %2 WHERE %3 = ?")
                  .arg(Singleton::enumValueToKey(ReportInfo::reportData),
                       Singleton::enumName<ReportInfo>(),
                       Singleton::enumValueToKey(ReportInfo::reportTime)));
    query.addBindValue(selectedTime(index).toString(timeFormat()));
    query.exec();
    if (query.next()) {
        return QJsonDocument::fromJson(query.value(0).toString().toUtf8()).object();
    }
    return QJsonObject();
}

bool ReportDialog::indexIsValid(const QModelIndex &index)
{
    if (!index.isValid()) {
        QMessageBox::information(this, "提示", "请选择一个报告");
        return false;
    }
    return true;
}

QVector<ReportModelItem> ReportDialog::getItems()
{
    QVector<ReportModelItem> items;
    QSqlQuery query(TcpClientSocket::getInstance()->db());
    query.exec(QString("SELECT %1, %2, %3 FROM %4 ORDER BY %1 DESC")
               .arg(Singleton::enumValueToKey(ReportInfo::reportTime),
                    Singleton::enumValueToKey(ReportInfo::modify),
                    Singleton::enumValueToKey(ReportInfo::reportData),
                    Singleton::enumName<ReportInfo>()));
    while (query.next()) {
        items.append(ReportModelItem(QDateTime::fromString(query.value(0).toString(), timeFormat()).toMSecsSinceEpoch(),
                                     2 == query.value(1).toInt(),
                                     QJsonDocument::fromJson(query.value(2).toString().toUtf8()).object()));
    }
    return items;
}

QString ReportDialog::timeFormat() const
{
    return QString("yyyy-MM-dd hh:mm:ss.zzz");
}
