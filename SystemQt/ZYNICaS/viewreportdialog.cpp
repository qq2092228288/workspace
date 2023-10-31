#include "viewreportdialog.h"
#include "datamanagement.h"
#include "reportpreviewdialog.h"
#include "reportpainter.h"

ViewReportDialog::ViewReportDialog(QWidget *parent)
    : QDialog{parent}
{
    setWindowTitle(tr("查看报告"));
    auto &instance = DataManagement::getInstance();
    setMinimumSize(1050*instance.wZoom(), 600*instance.hZoom());
    setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
    //样式表
    setStyleSheet(instance.dialogQss(1));

    if(QSqlDatabase::contains("qt_sql_default_connection"))
        m_db = QSqlDatabase::database("qt_sql_default_connection");
    else
        m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName("Reports.db");

    searchLabel = new QLabel("查找条件:", this);
    comboBox = new QComboBox(this);
    lineEdit = new QLineEdit(this);
    searchButton = new QPushButton("搜索", this);
    resettingButton = new QPushButton("重置", this);
    pullButton = new QPushButton("拉取", this);
    pdfButton = new QPushButton("生成PDF", this);
    printPreviewButton = new QPushButton("打印预览", this);
    calendarDialog = new ScopeCalendarDialog(this);
    tableView = new QTableView(this);
    model = new ReportTableModel(this);

    comboBox->addItems(QStringList()<<"姓名"<<"病历号"<<"时间");
    comboBox->setMinimumWidth(100);
    pullButton->setToolTip(QString("从服务器拉取已会诊的报告结论。"));

    auto mainLayout = new QVBoxLayout(this);
    auto hLayout = new QHBoxLayout;
    mainLayout->addLayout(hLayout);

    hLayout->addWidget(searchLabel);
    hLayout->addWidget(comboBox);
    hLayout->addWidget(lineEdit);
    hLayout->addWidget(searchButton);
    hLayout->addWidget(resettingButton);
    hLayout->addStretch();
    hLayout->addWidget(pullButton);
    hLayout->addStretch();
    hLayout->addWidget(pdfButton);
    hLayout->addWidget(printPreviewButton);
    mainLayout->addWidget(tableView);

    // 选中整行，不可编辑，单选
    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    tableView->setModel(model);
    tableView->setColumnWidth(0, 150);
    tableView->setColumnWidth(1, 150);
    tableView->horizontalHeader()->setStretchLastSection(true);

    auto client = DataManagement::getInstance().mqttClient();
    connect(comboBox, QOverload<int>::of(&QComboBox::activated), this, &ViewReportDialog::comboBoxActivatedSlot);
    connect(calendarDialog, &ScopeCalendarDialog::scopeTime, this, &ViewReportDialog::scopeTimeSlot);
    connect(searchButton, &QPushButton::clicked, this, &ViewReportDialog::searchSlot);
    connect(resettingButton, &QPushButton::clicked, this, &ViewReportDialog::resettingSlot);
    connect(pullButton, &QPushButton::clicked, this, &ViewReportDialog::pullingSlot);
    connect(this, &ViewReportDialog::pulling, client, &MqttClient::pullingConclusion);
    connect(pdfButton, &QPushButton::clicked, this, &ViewReportDialog::createdPdfSlot);
    connect(client, &MqttClient::pulled, this, &ViewReportDialog::pulledSlot);
    connect(printPreviewButton, &QPushButton::clicked, this, &ViewReportDialog::printPreviewSlot);
    connect(tableView, &QTableView::doubleClicked, this, &ViewReportDialog::tableDoubleCilicked);
}

void ViewReportDialog::showEvent(QShowEvent *event)
{
    resettingSlot();
    event->accept();
}

void ViewReportDialog::comboBoxActivatedSlot(int index)
{
    if (2 == index) {
        calendarDialog->exec();
    }
}

void ViewReportDialog::scopeTimeSlot(qint64 start, qint64 end)
{
    model->filterTime(start, end);
}

void ViewReportDialog::searchSlot()
{
    auto index = comboBox->currentIndex();
    if (0 == index) {
        model->filterPatientName(lineEdit->text());
    }
    else if (1 == index) {
        model->filterMedicalRecordNumber(lineEdit->text());
    }
}

void ViewReportDialog::resettingSlot()
{
    lineEdit->clear();
    model->updateList(getItems());
}

void ViewReportDialog::pullingSlot()
{
    auto index = tableView->currentIndex();
    if (indexIsValid(index)) {
        emit pulling(getReportTime(index).toMSecsSinceEpoch());
    }
}

void ViewReportDialog::pulledSlot(int state)
{
    resettingSlot();
    switch (PullState(state)) {
    case PullState::Pulled:
        QMessageBox::information(this, "提示", "已拉取最新会诊报告！");
        break;
    case PullState::NotPulling:
        QMessageBox::information(this, "提示", "没有需要会诊的报告！");
        break;
    case PullState::NoData:
        QMessageBox::information(this, "提示", "该报告没有进行会诊！");
        break;
    }
}

void ViewReportDialog::createdPdfSlot()
{
    auto index = tableView->currentIndex();
    if (indexIsValid(index)) {
        auto object = getReportJson(index);
        if (!object.isEmpty()) {
            auto patientInfo = object.value(ReportDataName::ekey(ReportDataName::patientInfo)).toObject();
            auto fileName = DataManagement::getInstance().getPaths().reports()
                    + getReportTime(index).toString("yyyyMMddhhmmss")
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
            printer.setOutputFormat(QPrinter::PdfFormat);
            printer.setOutputFileName(fileName);
            auto info = DataManagement::getInstance().getHospitalInfo();
            printer.setPageSize(QPageSize(info->pType == Printer_Type::Thermal ? QSizeF(72, 297) : QSizeF(210, 297),
                                          QPageSize::Millimeter));
            printer.setFullPage(true);
            printer.setPageMargins(QMarginsF(0, 0, 0, 0));
            auto &instance = DataManagement::getInstance();
            ReportStruct temp(info->pType, info->cMode, !info->samePage, instance.getPaths().hospitalLogo(),
                              instance.getHospitalInfo()->trendChartTitle, object);
            temp.departmentName = instance.departmentName();
            temp.idName = instance.idName();
            ReportPainter painter(temp, &printer);
            QDesktopServices::openUrl(QUrl::fromLocalFile(fileName));
        }
        else {
            QMessageBox::warning(this, "警告", "报告不存在！");
        }
    }
}

void ViewReportDialog::printPreviewSlot()
{
    auto index = tableView->currentIndex();
    if (indexIsValid(index)) {
        tableDoubleCilicked(index);
    }
}

void ViewReportDialog::tableDoubleCilicked(const QModelIndex &index)
{
    auto object = getReportJson(index);
    if (!object.isEmpty()) {
        // 报告预览
        QPrinter printer(QPrinter::ScreenResolution);
        ReportPreviewDialog dialog(object, DataManagement::getInstance().getHospitalInfo(), &printer);
        dialog.exec();
    }
    else {
        QMessageBox::warning(this, "警告", "报告不存在！");
    }
}

QVector<ReportModelItem> ViewReportDialog::getItems()
{
    QVector<ReportModelItem> items;
    if (m_db.open()) {
        QSqlQuery query(m_db);
        query.exec(QString("SELECT time, upload, data from reports order by time desc"));
        while (query.next()) {
            items.append(ReportModelItem(query.value(0).toLongLong(),
                                         (query.value(1).toInt() == ReportState::Modified),
                                         QJsonDocument::fromJson(query.value(2).toString().toUtf8()).object()));
        }
    }
    return items;
}

QJsonObject ViewReportDialog::getReportJson(const QModelIndex &index)
{
    auto time = getReportTime(index).toMSecsSinceEpoch();
    if (m_db.open()) {
        QSqlQuery query(m_db);
        query.exec(QString("SELECT data from reports where time = %1").arg(time));
        if (query.next()) {
            return QJsonDocument::fromJson(query.value(0).toString().toUtf8()).object();
        }
    }
    return QJsonObject();
}

QDateTime ViewReportDialog::getReportTime(const QModelIndex &index)
{
    return model->data(model->index(index.row(), 0)).toDateTime();
}

bool ViewReportDialog::indexIsValid(const QModelIndex &index)
{
    if (!index.isValid()) {
        QMessageBox::information(this, "提示", "请选择一个报告");
        return false;
    }
    return true;
}
