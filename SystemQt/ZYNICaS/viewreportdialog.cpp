#include "viewreportdialog.h"
#include "datamanagement.h"
#include "reportpreviewdialog.h"
#include "reportpainter.h"
#include "waitingdialog.h"
#include "reportgraphicsitem.h"
#include "printgraphicsview.h"
#include <QThread>

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
//    tableView->setSelectionMode(QAbstractItemView::SingleSelection);
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
    auto selections = tableView->selectionModel();
    QVector<QModelIndex> indexs;
    foreach (auto index, selections->selectedIndexes()) {
        if (0 == index.column()) {
            indexs<<index;
        }
    }
    if (indexs.size() == 0) {
        QMessageBox::information(this, "提示", "至少选择一个报告");
    }
    else if (indexs.size() == 1) {
        auto fileName = createdPdf(indexs.at(0));
        QDesktopServices::openUrl(QUrl::fromLocalFile(fileName));
    }
    else {
        WaitingDialog dialog(this);
        connect(this, &ViewReportDialog::pdfCreationProgress, &dialog, &WaitingDialog::setValue);
        auto thread = QThread::create(&ViewReportDialog::createdPdfs, this, indexs);
        thread->start();
        dialog.exec();
        QDesktopServices::openUrl(QUrl::fromLocalFile(DataManagement::getInstance().getPaths().reports()));
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

        // auto samepage = DataManagement::getInstance().getHospitalInfo()->samePage;
        // QDialog dialog(this);
        // dialog.resize(screen()->availableSize());
        // dialog.setWindowFlags(Qt::Dialog | Qt::WindowMinimizeButtonHint |
        //                       Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint);
        // QGraphicsScene scene;
        // auto view = new PrintGraphicsView(samepage, object, &scene, &dialog);
        // auto psize = QPageSize::sizePixels(QPageSize::A4, screen()->logicalDotsPerInch());
        // scene.setItemIndexMethod(QGraphicsScene::NoIndex);
        // if (!samepage && object.value(ReportDataName::ekey(ReportDataName::position)).toArray().size() > 1) {
        //     // 分页
        //     auto item0 = new ReportGraphicsItem(psize, object, samepage, PageType::MainPage_0);
        //     auto item1 = new ReportGraphicsItem(psize, object, samepage, PageType::MainPage_1);
        //     auto imagePage = new ReportGraphicsItem(psize, object, samepage, PageType::ImagePage);
        //     item1->setPos(0, item0->boundingRect().height());
        //     imagePage->setPos(0, item0->boundingRect().height() * 2);
        //     scene.addItem(item0);
        //     scene.addItem(item1);
        //     scene.addItem(imagePage);
        // }
        // else {
        //     // 不分页
        //     auto item = new ReportGraphicsItem(psize, object, samepage, PageType::MainPage_0);
        //     auto imagePage = new ReportGraphicsItem(psize, object, samepage, PageType::ImagePage);
        //     imagePage->setPos(0, item->boundingRect().height());
        //     scene.addItem(item);
        //     scene.addItem(imagePage);
        // }
        // foreach (auto item, scene.items()) {
        //     auto ritem = dynamic_cast<ReportGraphicsItem *>(item);
        //     connect(view, &PrintGraphicsView::startRecache, ritem, &ReportGraphicsItem::startRecache);
        //     connect(view, &PrintGraphicsView::endRecache, ritem, &ReportGraphicsItem::endRecache);
        // }
        // view->resize(screen()->availableSize());
        // dialog.showMaximized();
        // dialog.exec();
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

QString ViewReportDialog::createdPdf(const QModelIndex &index)
{
    if (index.isValid()) {
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
            QPrinter printer(QPrinter::ScreenResolution);
            printer.setOutputFormat(QPrinter::PdfFormat);
            printer.setOutputFileName(fileName);
            auto info = DataManagement::getInstance().getHospitalInfo();
            if (info->pType == Printer_Type::Thermal) {
                printer.setPageSize(QPageSize(QSizeF(72, 297), QPageSize::Millimeter));
            }
            else {
                printer.setPageSize(QPageSize(QSizeF(210, 297), QPageSize::Millimeter));
            }
            printer.setFullPage(true);
            printer.setPageMargins(QMarginsF(0, 0, 0, 0));
            auto &instance = DataManagement::getInstance();
            ReportStruct temp(info->pType, info->cMode, !info->samePage, instance.getPaths().hospitalLogo(),
                              instance.getHospitalInfo()->trendChartTitle, object);
            temp.departmentName = instance.departmentName();
            temp.idName = instance.idName();
            ReportPainter painter(temp, &printer);

            // QPainter painter(&printer);
            // painter.setRenderHint(QPainter::Antialiasing);
            // QGraphicsScene scene;
            // auto psize = QPageSize::sizePixels(QPageSize::A4, screen()->logicalDotsPerInch());
            // scene.setItemIndexMethod(QGraphicsScene::NoIndex);
            // if (!info->samePage && object.value(ReportDataName::ekey(ReportDataName::position)).toArray().size() > 1) {
            //     // 分页
            //     auto item0 = new ReportGraphicsItem(psize, object, info->samePage, PageType::MainPage_0);
            //     auto item1 = new ReportGraphicsItem(psize, object, info->samePage, PageType::MainPage_1);
            //     auto imagePage = new ReportGraphicsItem(psize, object, info->samePage, PageType::ImagePage);
            //     item1->setPos(0, item0->boundingRect().height());
            //     imagePage->setPos(0, item0->boundingRect().height() * 2);
            //     scene.addItem(item0);
            //     scene.addItem(item1);
            //     scene.addItem(imagePage);
            // }
            // else {
            //     // 不分页
            //     auto item = new ReportGraphicsItem(psize, object, info->samePage, PageType::MainPage_0);
            //     auto imagePage = new ReportGraphicsItem(psize, object, info->samePage, PageType::ImagePage);
            //     imagePage->setPos(0, item->boundingRect().height());
            //     scene.addItem(item);
            //     scene.addItem(imagePage);
            // }
            // auto items = scene.items();
            // for (int i = 0; i < items.size(); ++i) {
            //     auto item = dynamic_cast<ReportGraphicsItem *>(items.at(i));
            //     if (item != nullptr) {
            //         PainterConfig config(&painter, psize, object, info->samePage);
            //         config.paintPage(item->pageType());
            //         if (i + 1 != items.size()) {
            //             printer.newPage();
            //         }
            //     }
            // }
            // painter.end();
            return fileName;
        }
    }
    return QString();
}

void ViewReportDialog::createdPdfs(const QVector<QModelIndex> &vector)
{
    for (int i = 0, size = vector.size(); i < size; ++i) {
        auto index = vector.at(i);
        createdPdf(index);
        emit pdfCreationProgress(i + 1, size);
    }
    emit created();
}

bool ViewReportDialog::indexIsValid(const QModelIndex &index)
{
    if (!index.isValid()) {
        QMessageBox::information(this, "提示", "请选择一个报告");
        return false;
    }
    return true;
}
