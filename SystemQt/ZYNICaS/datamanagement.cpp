#include "datamanagement.h"
#include "qrencode.h"
#include "idcheck.h"
#include "datacalculation.h"

MyFilePath::MyFilePath(const QString &path)
    : appPath{path}
{

}

QString MyFilePath::initDir() const
{
    return appPath + "init/";
}

QString MyFilePath::tempDir() const
{
    return appPath + "temp/";
}

QString MyFilePath::baseInfo() const
{
    return initDir() + "baseInfo.ini";
}

QString MyFilePath::showItems() const
{
    return initDir() + "showItems.ini";
}

QString MyFilePath::trendCharts() const
{
    return initDir() + "trendCharts.ini";
}

QString MyFilePath::single_dot() const
{
    return appPath + "dot/singlePositionTemplate.dot";
}

QString MyFilePath::many_dot() const
{
    return appPath + "dot/manyPositionTemplate.dot";
}

QString MyFilePath::xsingle_dot() const
{
    return appPath + "dot/singlePositionXprinterTemplate.dot";
}

QString MyFilePath::xmany_dot() const
{
    return appPath + "dot/manyPositionXprinterTemplate.dot";
}

QString MyFilePath::psingle_dot() const
{
    return appPath + "dot/singlePositionProTemplate.dot";
}

QString MyFilePath::pmany_dot() const
{
    return appPath + "dot/manyPositionProTemplate.dot";
}

QString MyFilePath::_dot() const
{
    return appPath + "dot/testTemplate.dot";
}

QString MyFilePath::record_dz() const
{
    return tempDir() + "record.png";
}

QString MyFilePath::current_dz() const
{
    return tempDir() + "current.png";
}

QString MyFilePath::sudoku() const
{
    return tempDir() + "sudoku.png";
}

QString MyFilePath::isiCurve() const
{
    return tempDir() + "isicurve.png";
}

QStringList MyFilePath::trendchartspic() const
{
    QStringList list;
    for (int i = 0; i < 12; ++i) {
        list<<(tempDir() + QString("trendchart%1.png").arg(i, 2, 10, QLatin1Char('0')));
    }
    return list;
}

QString MyFilePath::reports() const
{
    return appPath + "reports";
}

QString MyFilePath::config_zip() const
{
    return initDir() + "config.zip";
}

QString MyFilePath::qrCode() const
{
    return tempDir() + "qrcode.png";
}

Argument::Argument(){}

Argument::Argument(QString _cn, QString _en, QString _unit, qreal _min, qreal _max, int _digit)
    : cn(_cn),en(_en),unit(_unit),min(_min),max(_max),digit(_digit)
{

}

Argument::Argument(QString _cn, QString _en, QString _unit, qreal _sbpmin, qreal _sbpmax, int _digit,
                   QString _dbpcn, QString _dbpen, qreal _dbpmin, qreal _dbpmax)
    : cn(_cn),en(_en),unit(_unit),min(_sbpmin),max(_sbpmax),
      digit(_digit),dbpcn(_dbpcn),dbpen(_dbpen),dbpmin(_dbpmin),dbpmax(_dbpmax)
{

}

Arguments::Arguments()
{
    arguments = QList<Argument>()
        <<Argument("心输出量", "CO", "L/min", 3.5, 9.0, 1)
        <<Argument("心脏指数", "CI", "L/min·m²", 2.0, 5.0, 1)
        <<Argument("氧输送", "DO2", "mL/min", 1000, 1000, 0)
        <<Argument("心搏量", "SV", "mL/beat", 50, 120, 1)
        <<Argument("心搏指数", "SI", "mL/beat·m²", 35, 65, 0)
        <<Argument("心率变异性", "HRV", "%", -50, 50, 0)
        <<Argument("胸液传导性", "TFC", "1/Ω", 0.025, 0.045, 3)
        <<Argument("舒张末期指数", "EDI", "mL/beat·m²", 54, 130, 0)
        <<Argument("每搏输出变异性", "SVV", "%", 0, 10, 0)
        <<Argument("血管容积", "Vol", "%", -50, 50, 0)
        <<Argument("系统阻力", "SVR", "dyn·s·m²/cm^5", 489, 2262, 0)
        <<Argument("每搏外周阻力", "SSVR", "dyn·s/cm^5", 119.6, 429.2, 1)
        <<Argument("每搏外周阻力指数", "SSVRI", "dyn·s/cm^5/beat/m²", 99.7, 185.1, 1)
        <<Argument("系统阻力指数", "SVRI", "dyn·s·m²/cm^5", 1056, 4000, 0)
        <<Argument("血管顺应性", "Vas", "%", -50, 50, 0)
        <<Argument("射血前期", "PEP", "ms", 50, 120, 0)
        <<Argument("左室射血时间", "LVET", "ms", 250, 350, 0)
        <<Argument("左心室每搏做功", "LSW", "g·m", 20, 90, 0)
        <<Argument("每搏做功指数", "LSWI", "g·m/beat/m²", 39.3, 73.0, 1)
        <<Argument("左心室做功", "LCW", "kg·m", 5.4, 10.0, 1)
        <<Argument("左心室做功指数", "LCWI", "kg·m/m²", 1.81, 7.06, 2)
        <<Argument("收缩时间比", "STR", "-", 0, 0.4, 1)
        <<Argument("射血期收缩指数", "EPCI", "1/s", 0.038, 0.076, 3)
        <<Argument("变力状态指数", "ISI", "1/s²", 0.90, 1.70, 2)
        <<Argument("收缩变力性", "Ino", "%", -50, 50, 0)
        <<Argument("心率", "HR", "bpm", 60, 100, 0)
        <<Argument("收缩压", "SBP", "mmHg", 90, 140, 0, "舒张压", "DBP", 60, 90)
        <<Argument("平均动脉压", "MAP", "mmHg", 70, 105, 0);
}

Argument Arguments::findArgument(const QString &en)
{
    if (en.isEmpty())
        return Argument();
    foreach (auto argument, arguments) {
        if (en == argument.en || en == argument.dbpen) {
            return argument;
        }
    }
    return Argument();
}

DataManagement::DataManagement()
{
    isRecord = false;
    m_pTebco = new ZyTebco;
    reportThread = new CreateReportThread;
//    m_pIdCheck = new IdCheck;
    m_pHttpPost = new HttpPost;
}

DataManagement::~DataManagement()
{
    reportThread->quit();
    reportThread->wait();
    delete m_pTebco;
    delete m_pHttpPost;
    delete reportThread;
    // delete temp dir
    QDir dir(m_filePath.tempDir());
    dir.removeRecursively();
//    qDebug()<<"~DataManagement()";
}

ZyTebco *DataManagement::getTebco() const
{
    return m_pTebco;
}

//IdCheck *DataManagement::getIdCheck() const
//{
//    return m_pIdCheck;
//}

void DataManagement::setSize(const QSize &size)
{
    m_winSize = size;
}

qreal DataManagement::wZoom() const
{
    return (qreal)m_winSize.width()/1920;
}

qreal DataManagement::hZoom() const
{
    return (qreal)m_winSize.height()/1080;
}

qreal DataManagement::zoom() const
{
    return (qreal)(wZoom()+hZoom())/2;
}

QFont DataManagement::font(const int &size, const QString &family) const
{
    QFont qfont(family);
    qfont.setPixelSize(size*zoom());
    return qfont;
}

QSize DataManagement::rectSize(const int &w, const int &h)
{
    return QSize(w*wZoom(),h*hZoom());
}

QString DataManagement::dialogQss(const double scale) const
{
    int fsize = (18*zoom() + 1)*scale;
    QString qss = QString("QLabel\
                {\
                    color:#000000;\
                    font:bold %1px;\
                    font-family:Microsoft YaHei;\
                }\
                QPushButton\
                {\
                    color:#ffffff;\
                    background-color:#778899;\
                    border-style:outset;\
                    border-width:1px;\
                    border-color:#000000;\
                    border-radius:3px;\
                    padding:2px;\
                    font:bold %2px;\
                }\
                QPushButton:hover\
                {\
                    background-color:qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop:0 #778899, stop: 1 #bebebe);\
                }\
                QPushButton:pressed\
                {\
                    background-color:qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop:0 #d3d3d3, stop: 1 #708090);\
                }\
                QLineEdit\
                {\
                    color:#000000;\
                    font:bold %3px;\
                    background-color:#bebebe;\
                    border-radius: 3px;\
                }\
                QComboBox\
                {\
                    color:#000000;\
                    font:bold %4px;\
                    background-color:#bebebe;\
                    border-radius: 3px;\
                    font-family:Microsoft YaHei;\
                }\
                QGroupBox\
                {\
                    border:2px solid #ffffff;\
                    margin-top: 2ex;\
                    color:#000000;\
                    border-radius:3px;\
                    font:bold %5px;\
                }\
                QGroupBox::title\
                {\
                    subcontrol-origin: margin;\
                    subcontrol-position: top center;\
                    padding: 0 3px;\
                }\
                QCheckBox\
                {\
                    color:#000000;\
                    font:bold %6px;\
                }\
                QRadioButton\
                {\
                    color:#000000;\
                    font:bold %7px;\
                }").arg(fsize).arg(fsize).arg(fsize).arg(fsize).arg(fsize).arg(fsize).arg(fsize);
    return qss;
}

void DataManagement::initCurrentPath()
{
    m_filePath = MyFilePath(QApplication::applicationDirPath()+"/");
}

MyFilePath DataManagement::getPaths() const
{
    return m_filePath;
}

bool DataManagement::isRecordPos() const
{
    return isRecord;
}

QString DataManagement::getNewReportName() const
{
    return m_newReportName;
}

Args &DataManagement::getArgs()
{
    return args;
}

CustomCtrlRegulator *DataManagement::getRegulator() const
{
    return m_pRegulator;
}

HospitalInfo *DataManagement::getHospitalInfo() const
{
    return m_pHospitalInfo;
}

HttpPost *DataManagement::httpPost() const
{
    return m_pHttpPost;
}

QString DataManagement::getMac() const
{
    QList<QNetworkInterface> nets = QNetworkInterface::allInterfaces();// 获取所有网络接口列表
    for(int i = 0; i < nets.count(); i ++)
    {
        // 如果此网络接口不是回环地址，则就是我们需要找的Mac地址
        if(!nets[i].flags().testFlag(QNetworkInterface::IsLoopBack)) {
            return nets[i].hardwareAddress();
            break;
        }
    }
    return QString();
}

ReportDataBase *DataManagement::reportDataBase() const
{
    return m_pReportDataBase;
}

DeviceDatabase *DataManagement::deviceDatabase() const
{
    return m_pDeviceDatabase;
}

int DataManagement::surplus() const
{
    return (deviceDatabase()->getConsumableSurplus() + IdCheck::getCurrentConsumables());
}

void DataManagement::setHospitalInfo(HospitalInfo *hospitalInfo)
{
    this->m_pHospitalInfo = hospitalInfo;
}

void DataManagement::setBodyValue(BodyValue *bodyValue)
{
    this->m_pBodyValue = bodyValue;
}

void DataManagement::setRegulator(CustomCtrlRegulator *regulator)
{
    this->m_pRegulator = regulator;
}

void DataManagement::setdZ(QChartView *dZ)
{
    this->m_pdZ = dZ;
}

void DataManagement::setSudoku(QWidget *sudoku)
{
    this->m_pSudoku = sudoku;
}

void DataManagement::setReportDataBase(ReportDataBase *reportDataBase)
{
    this->m_pReportDataBase = reportDataBase;
}

void DataManagement::setDeviceDatabase(DeviceDatabase *deviceDatabase)
{
    this->m_pDeviceDatabase = deviceDatabase;
}

void DataManagement::recordPosition(QString position)
{
    m_recordInfo.pos = position;
//    m_recordInfo.posture = position == tr("半卧") ? "1" : (position == tr("平躺") ? "2" : "3");
    saveInfo(m_recordInfo);
    m_pdZ->grab().save(m_filePath.record_dz());
    isRecord = true;
}

QString DataManagement::saveReport(QDateTime curTime, QString position, bool record)
{
    emit startCheck(false);
    m_newReportName.clear();
    m_currentInfo.pos = position;
    saveInfo(m_currentInfo, record);
    m_pdZ->grab().save(m_filePath.current_dz());
    m_pSudoku->grab().save(m_filePath.sudoku());

    reportThread->init();
    connect(reportThread, &CreateReportThread::finished, this, &DataManagement::clear);
    // 打开模板
    if (m_pHospitalInfo->pType == Printer_Type::General) {        // 常规打印机
        if (record) {   // 多体位
            if (m_pHospitalInfo->cMode == Check_Mode::Hypertension) {
                reportThread->setOpenArg(m_filePath._dot(), false);
            }
            else if (m_pHospitalInfo->cMode == Check_Mode::InternalMedicine) {
                reportThread->setOpenArg(m_filePath._dot(), false);
            }
            else if (m_pHospitalInfo->cMode == Check_Mode::Critical) {
                reportThread->setOpenArg(m_filePath._dot(), false);
            }
            else if (m_pHospitalInfo->cMode == Check_Mode::PhysicalExamination) {
                reportThread->setOpenArg(m_filePath.many_dot(), false);
            }
            reportThread->addMarks("rpos", (m_recordInfo.pos));
            // 记录体位
            QStringList rList;
            for (int i = 0; i < 29; ++i) {
                rList<<QString("r%1").arg(i, 2, 10, QLatin1Char('0'));
            }
            reportThread->addBatchMarks(rList, m_recordInfo.values);
            reportThread->addMarks("rname", (m_recordInfo.pos + tr(" 心阻抗图(dZ)")));
            // 插入图片
            reportThread->addPic("rimage",m_filePath.record_dz());
            auto isiCtrl = m_pRegulator->getCustomCtrl("ISI");
            if (isiCtrl->getRecordValue() != 0 || isiCtrl->getCurrentValue() != 0) {
                reportThread->addMarks("aname", (tr("容量@泵力分析图")));
                reportThread->addPic("aimage", m_filePath.isiCurve());
            }
            // PLRT
            reportThread->addMarks("plrttitle", tr("被动抬腿试验"));
            reportThread->addPic("plrtfimage", posImagePath(m_recordInfo.pos));
            reportThread->addPic("plrtsimage", posImagePath(m_currentInfo.pos));
            reportThread->addMarks("plrtunit", "%");
            addPlrt(0, Type::HR);
            addPlrt(1, Type::SI);
            addPlrt(2, Type::CI);
            addPlrt(3, Type::SV);
            addPlrt(4, Type::CO);
            addPlrt(5, Type::DO2);
            addPlrt(6, Type::TFC);
            addPlrt(7, Type::ISI);
            reportThread->addMarks("plrtevaluation", tr("被动抬腿试验测试报告的建议：\n"
                "1.阳性的判定：被动抬腿试验结束后，SV，SI，CO，CI大于第一体位10%~15%。视同为被动抬腿试验阳性（液体负荷试验阳性）。\n"
                "2.阴性的判定：被动抬腿试验结束后，SV，SI，CO，CI小于第一体位10%。视同为被动抬腿试验阴性（液体负荷试验阴性）。\n"
                "请结合临床慎重处置液体管理问题。\n"));
            // 趋势图
            if (instance.getHospitalInfo()->cMode != Check_Mode::PhysicalExamination) {
                for (int index = 0; index < m_filePath.trendchartspic().size(); ++index) {
                    reportThread->addPic(QString("trendchart%1").arg(index, 2, 10, QLatin1Char('0')),
                                         m_filePath.trendchartspic().at(index));
                }
            }
            // hrv
            reportThread->addMarks("hrvtitle", tr("心率变异性分析"));
            auto hrvalues = m_pRegulator->getCustomCtrl(typeName(Type::HR))->getArgItems().values;
            int count = hrvalues.count();
            reportThread->addMarks("hrcount", QString::number(count));
            reportThread->addMarks("hrmax", 0 == count ? "-" : QString::number(*std::max_element(hrvalues.begin(), hrvalues.end())));
            reportThread->addMarks("hrmin", 0 == count ? "-" : QString::number(*std::min_element(hrvalues.begin(), hrvalues.end())));
            reportThread->addMarks("hravg", 0 == count ? "-" : QString::number(std::accumulate(hrvalues.begin(), hrvalues.end(), 0)/count));
            reportThread->addMarks("argname00", "NNVGR(ms)");
            reportThread->addMarks("argname01", "SDNN(ms)");
            reportThread->addMarks("argname02", "PNN50(%)");
            reportThread->addMarks("argname03", "RMSSD(ms)");
            reportThread->addMarks("argvalue00", 0 == count ? "-" : QString::number(DatCa::cNnvgr(hrvalues)));
            reportThread->addMarks("argvalue01", 0 == count ? "-" : QString::number(DatCa::cSdnn(hrvalues)));
            reportThread->addMarks("argvalue02", 0 == count ? "-" : QString::number(DatCa::cPnn50(hrvalues)));
            reportThread->addMarks("argvalue03", 0 == count ? "-" : QString::number(DatCa::cRmssd(hrvalues)));
            reportThread->addMarks("arglimits00", getScope(HrvArg::Nnvgr, m_pBodyValue->age));
            reportThread->addMarks("arglimits01", getScope(HrvArg::Sdnn, m_pBodyValue->age));
            reportThread->addMarks("arglimits02", getScope(HrvArg::Pnn50, m_pBodyValue->age));
            reportThread->addMarks("arglimits03", getScope(HrvArg::Rmssd, m_pBodyValue->age));
            reportThread->addMarks("argevaluation", tr("说明：\n"
                "1.心血管疾病患者（房颤、早搏、高血压、糖尿病等）心率变异性数据通常会偏高，高于正常值范围；\n"
                "2.同年龄段，女性心率变异性数据普遍略高于男性；\n"
                "3.运动人群心率变异值高于普通人群；\n"
                "4.心率变异性会随着年龄的增长而逐渐下降；\n"
                "5.测量姿势不正确，心电图存在干扰，都会影响心率变异性的准确性。"));
        }
        else {  // 单体位
            if (m_pHospitalInfo->cMode == Check_Mode::Hypertension) {
                reportThread->setOpenArg(m_filePath.single_dot(), false);
            }
            else if (m_pHospitalInfo->cMode == Check_Mode::InternalMedicine) {
                reportThread->setOpenArg(m_filePath.single_dot(), false);
            }
            else if (m_pHospitalInfo->cMode == Check_Mode::Critical) {
                reportThread->setOpenArg(m_filePath.single_dot(), false);
            }
            else if (m_pHospitalInfo->cMode == Check_Mode::PhysicalExamination) {
                reportThread->setOpenArg(m_filePath.single_dot(), false);
            }
        }
        reportThread->addMarks("cpos", (m_currentInfo.pos));
        QStringList cList;
        for (int i = 0; i < 29; ++i) {
            cList<<QString("c%1").arg(i, 2, 10, QLatin1Char('0'));
        }
        reportThread->addBatchMarks(cList, m_currentInfo.values);
        reportThread->addMarks("cname", (m_currentInfo.pos + tr(" 心阻抗图(dZ)")));
        reportThread->addPic("cimage", m_filePath.current_dz());
        reportThread->addMarks("room" , m_pHospitalInfo->roomName);
        // 二维码
//        getQrCodeUrlPixmap(m_pHospitalInfo->deviceId, curTime.toString("yyyyMMddHHmmss")).save(m_filePath.qrCode());
//        reportThread->addPic("qrcode", m_filePath.qrCode());
    }
    else if (m_pHospitalInfo->pType == Printer_Type::Thermal) {   // 热敏打印机
        if (record) {
            reportThread->setOpenArg(m_filePath.xmany_dot(), false);
            QStringList rList;
            for (int i = 0; i < 12; ++i) {
                rList<<QString("r%1").arg(i,2,10,QLatin1Char('0'));
            }
            QStringList values;
            values<<m_recordInfo.values.at(0)
                  <<m_recordInfo.values.at(1)
                  <<m_recordInfo.values.at(3)
                  <<m_recordInfo.values.at(4)
                  <<m_recordInfo.values.at(9)
                  <<m_recordInfo.values.at(14)
                  <<m_recordInfo.values.mid(22, 6);
            reportThread->addBatchMarks(rList, values);
        }
        else {
            reportThread->setOpenArg(m_filePath.xsingle_dot(), false);
        }
        QStringList cList;
        for (int i = 0; i < 12; ++i) {
            cList<<QString("c%1").arg(i,2,10,QLatin1Char('0'));
        }
        QStringList values;
        values<<m_currentInfo.values.at(0)
              <<m_currentInfo.values.at(1)
              <<m_currentInfo.values.at(3)
              <<m_currentInfo.values.at(4)
              <<m_currentInfo.values.at(9)
              <<m_currentInfo.values.at(14)
              <<m_currentInfo.values.mid(22, 6);
        reportThread->addBatchMarks(cList, values);
    }
    QString result = reportResult(record);
    if (result.isEmpty()) {
        result = "null";
    }
    else {
        reportThread->addMarks("result", result);
    }
    // 风险提示
    if (m_pHospitalInfo->tip) {
        auto hrv = m_pRegulator->getCustomCtrl(typeName(Type::HRV));
        if (hrv->getCurrentValue() < hrv->getMinValue()) {
            reportThread->addMarks("tip", tr("心血管疾患高风险人群提示。"));
        }
    }
    // title
    reportThread->addMarks("title", tr("无创心功能监测报告"));
    // dot normal data
    reportThread->addMarks("sname", tr("血压靶向分析图"));
    reportThread->addPic("simage", m_filePath.sudoku());
    // 医院、时间、检查人员
    reportThread->addMarks("hospital" , m_pHospitalInfo->hospitalName);
    reportThread->addMarks("time" , curTime.toString("yyyy-MM-dd hh:mm"));
    reportThread->addMarks("doctor" , m_pHospitalInfo->doctorName);
    // 患者基本信息
    reportThread->addMarks("name" , m_pBodyValue->name);
    reportThread->addMarks("sex", (0 == m_pBodyValue->sex) ? tr("男") : tr("女"));
    reportThread->addMarks("age" , QString::number(m_pBodyValue->age) + tr(" 岁"));
    reportThread->addMarks("height" , QString::number(m_pBodyValue->height) + " cm");
    reportThread->addMarks("weight" , QString::number(m_pBodyValue->weight) + " kg");
    reportThread->addMarks("number" , m_pBodyValue->id);
    reportThread->addMarks("area" , QString::number(m_pBodyValue->BSA(), 'f', 1) + " m²");
    // 保存
    m_newReportName = QString("%1/%2-%3-%4.docx").arg(m_filePath.reports(),
                      curTime.toString("yyyyMMddhhmm"), m_pBodyValue->id, m_pBodyValue->name);
    reportThread->setSaveAs(m_newReportName);
    reportThread->start();
    return result;
}

void DataManagement::clearSlot()
{
    //断开数据传输
    m_pTebco->startCheckSlot(false);
    m_pTebco->clearSerial();
    // 清空保存的信息
    m_recordInfo = Cdata();
    m_currentInfo = Cdata();
    isRecord = false;
}

void DataManagement::reportPreview(const QString &path)
{
    reportThread->init();
    reportThread->setStartupMode(Mode::PrintPreview);
    reportThread->setOpenArg(path,true);
    reportThread->start();
}

void DataManagement::reportPrintOut(const QString &path)
{
    reportThread->init();
    reportThread->setStartupMode(Mode::PrintOut);
    reportThread->setOpenArg(path,false);
    reportThread->start();
}

void DataManagement::customCtrlTimer(bool start)
{
    auto hrCtrl = m_pRegulator->getCustomCtrl(typeName(Type::HR));
    auto svCtrl = m_pRegulator->getCustomCtrl(typeName(Type::SV));
    auto isiCtrl = m_pRegulator->getCustomCtrl(typeName(Type::ISI));
    foreach (auto ctrl, m_pRegulator->getAllCustomCtrls()) {
        ctrl->smoothTransitionTimer(start);
    }
    if (hrCtrl == nullptr || svCtrl == nullptr || isiCtrl == nullptr)
        return;
    if (start) {
        hrCtrl->startTimer(1);
        svCtrl->startTimer(10);
        isiCtrl->startTimer(100);
    }
    else {
        hrCtrl->stopTimer();
        svCtrl->stopTimer();
        isiCtrl->stopTimer();
    }
}

void DataManagement::requestConsumableList()
{
    if (m_pHttpPost != nullptr && m_pDeviceDatabase != nullptr) {
        emit onlineConsumableList("1", "10000", m_pDeviceDatabase->getDeviceInfo("deviceId"), nullptr, nullptr);
    }
}

void DataManagement::saveInfo(Cdata &cdata, bool second)
{
    cdata.values.clear();
    auto customCtrls = m_pRegulator->getAllCustomCtrls();
    foreach (CustomCtrl *customCtrl, customCtrls) {
        if (customCtrl->getName() != "SBP/DBP") {
            // 第一体位<<"CO"<<"CI"<<"SV"<<"SI"<<"TFC"<<"EPCI"<<"ISI"(<<"Ino")<<"HR"<<"MAP"(<<"Vol"<<"HRV"<<"Vas")
            // 第二体位<<"CO"<<"CI"<<"SV"<<"SI"<<"TFC"<<"EPCI"<<"ISI"(<<"Ino")<<"HR"<<"MAP"
            QStringList list = QStringList()<<"CO"<<"CI"<<"SV"<<"SI"<<"TFC"<<"EPCI"<<"ISI"<<"HR"<<"MAP";

            QString name = customCtrl->getName();
            double min = customCtrl->getMinValue();
            double max = customCtrl->getMaxValue();
            double rValue = customCtrl->getRecordValue();
            double cValue = customCtrl->getCurrentValue();

            QString str = QString::number(cValue);
            // list比较
            if (list.indexOf(name) != -1) {
                if (!second) {  // 第一体位
                    if (cValue != 0) {
                        str = tip(min, max, cValue);
                    }
                    else {
                        str = QString("-");
                    }
                }
                else {  // 第二体位
                    if (cValue != 0 && rValue != 0) {
                        str = tip(rValue, cValue);
                    }
                    else if (cValue != 0 && rValue == 0) {
                        str = QString::number(cValue);
                    }
                    else {
                        str = QString("-");
                    }
                }
            }
            else {
                if (cValue != 0) {
                    str = QString::number(cValue);
                }
                else if (name == "Ino" || (!second && (name == "HRV" || name == "Vol" || name == "Vas"))
                         || (name == "SVV" && m_pRegulator->getCustomCtrl("SV")->getCurrentValue() != 0)) {
                    if (m_pRegulator->getCustomCtrl("HR")->getCurrentValue() != 0) {    // 真实数据
                        if (!second) {
                            str = tip(min, max, cValue);
                        }
                        else {
                            str = tip(rValue, cValue);
                        }
                    }
                    else {
                        str = QString("-");
                    }
                }
                else {
                    str = QString("-");
                }
            }
            cdata.values<<str;
        }
        else {
            auto sbp = customCtrl->getArgItems();
            auto dbp = customCtrl->getDbpArgItems();
            if (!second) {
                if (sbp.currentValue == 0) {
                    cdata.values<<QString("-");
                }
                else {
                    cdata.values<<tip(sbp.minValue, sbp.maxValue, sbp.currentValue);
                }
                if (dbp.currentValue == 0) {
                    cdata.values<<QString("-");
                }
                else {
                    cdata.values<<tip(dbp.minValue, dbp.maxValue, dbp.currentValue);
                }
            }
            else {
                if (sbp.currentValue == 0) {
                    cdata.values<<QString("-");
                }
                else {
                    if (sbp.recordValue == 0) {
                        cdata.values<<QString::number(sbp.currentValue);
                    }
                    else {
                        cdata.values<<tip(sbp.recordValue, sbp.currentValue);
                    }
                }
                if (dbp.currentValue == 0) {
                    cdata.values<<QString("-");
                }
                else {
                    if (dbp.recordValue == 0) {
                        cdata.values<<QString::number(dbp.currentValue);
                    }
                    else {
                        cdata.values<<tip(dbp.recordValue, dbp.currentValue);
                    }
                }
            }
        }
    }
}

QString DataManagement::flag(CustomCtrl *customCtrl, bool second)
{
    QStringList fList = QStringList()<<"CO"<<"CI"<<"SV"<<"SI"<<"TFC"<<"EPCI"<<"ISI"<<"Ino"<<"HR"<<"MAP";
    QStringList sList = fList<<"Vol"<<"HRV"<<"Vas";

    if (!second) {  // 第一体位
        if (fList.indexOf(customCtrl->getName()) != -1) {
            if (customCtrl->getCurrentValue() != 0) {
                if ((customCtrl->getName() == "Vol" || customCtrl->getName() == "Vas") &&
                        m_pRegulator->getCustomCtrl("MAP")->getCurrentValue() == 0) {   // Vol或Vas参数时MAP为零
                    return QString("-");
                }
                else {
                    return tip(customCtrl->getMinValue(), customCtrl->getMaxValue(), customCtrl->getCurrentValue());
                }
            }
        }
    }
    else {  // 第二体位
        if (sList.indexOf(customCtrl->getName()) != -1) {
            if (customCtrl->getCurrentValue() != 0) {
                if (customCtrl->getRecordValue() != 0) {
                    return tip(customCtrl->getRecordValue(), customCtrl->getCurrentValue());
                }
            }
            else {
                if ((customCtrl->getName() == "Vol" || customCtrl->getName() == "Vas") &&
                    m_pRegulator->getCustomCtrl("MAP")->getCurrentValue() != 0) {
                    return QString::number(customCtrl->getCurrentValue());
                }
                return QString("-");
            }
        }
    }
    return QString::number(customCtrl->getCurrentValue());
}

QString DataManagement::tip(qreal min, qreal max, qreal value)
{
    if (value < min) {
        return QString("%1 ↓").arg(value);
    }
    else if (value > max) {
        return QString("%1 ↑").arg(value);
    }
    return QString::number(value);
}

QString DataManagement::tip(qreal rValue, qreal cValue)
{
    if (cValue < rValue) {
        return QString("%1 ↓").arg(cValue);
    }
    else if (cValue > rValue) {
        return QString("%1 ↑").arg(cValue);
    }
    return QString::number(cValue);
}

QString DataManagement::mArg(const Type &type, const int &printer, bool isMany)
{
    ArgItems items;
    CustomCtrl *ctrl = m_pRegulator->getCustomCtrl(typeName(type));
    if (type == Type::SBP || type == Type::DBP) ctrl = m_pRegulator->getCustomCtrl("SBP/DBP");
    if (ctrl == nullptr) return nullptr;
    items = ctrl->getArgItems();
    if (type == Type::DBP) items = ctrl->getDbpArgItems();
    qreal value = items.currentValue;
    if (isMany) value = items.recordValue;
    QString result;
    if (value > items.maxValue) result = tr("偏高");
    else if (value < items.minValue) result = tr("偏低");
    else result = tr("正常");
    if (printer == 0) return QString("%1(%2)%3;").arg(items.dataName_cn, items.dataName, result);  // 常规
    return QString("%1%2;").arg(items.dataName, result); // 热敏
}

QString DataManagement::reportResult(bool record)
{
    if (m_pRegulator->getCustomCtrl("HR")->getCurrentValue() == 0 ||
            m_pRegulator->getCustomCtrl("MAP")->getCurrentValue() == 0) {
        return nullptr;
    }
    else if (record && (m_pRegulator->getCustomCtrl("HR")->getRecordValue() == 0 ||
                        m_pRegulator->getCustomCtrl("MAP")->getRecordValue() == 0)) {
        return nullptr;
    }
    QString result;
    if (record) {   // 双体位
        if (m_pHospitalInfo->pType == Printer_Type::General) {
            // 双体位常规打印机报告
            auto isi = m_pRegulator->getCustomCtrl(typeName(Type::ISI));
            auto sv = m_pRegulator->getCustomCtrl(typeName(Type::SV));
            auto svr = m_pRegulator->getCustomCtrl(typeName(Type::SVR));
            auto map = m_pRegulator->getCustomCtrl(typeName(Type::MAP));
            QString fstr = (isi->getCurrentValue() > isi->getRecordValue() && sv->getCurrentValue() > sv->getRecordValue() ? tr("正常") : tr("异常"));
            QString sstr = (isi->getCurrentValue() <= isi->getMaxValue() && isi->getCurrentValue() >= isi->getMinValue() ? tr("正常") : tr("异常"));
            QString tstr = (svr->getCurrentValue() <= svr->getMaxValue() && svr->getCurrentValue() >= svr->getMinValue() ? tr("正常") : tr("异常"));
            if (map->getCurrentValue() < map->getMinValue() && svr->getCurrentValue() > svr->getMaxValue()) {
                tstr += tr("(提示：病人血压低，后负荷被动性代偿)");
            }
            result = tr("连续无创血流动力学对高血压病靶向分析报告如下：\n"
                        "1.第一体位：心输出量(CO)%1，心脏指数(CI)%2，心搏量(SV)%3，心搏指数(SI)%4，"
                        "心率(HR)%5，血管顺应性(Vas)%6，血管容量(Vol)%7，收缩变力性(Ino)%8，"
                        "收缩压(SBP)%9，舒张压(DBP)%10，胸液传导性(TFC)%11；\n"
                        "2.第二体位增加容量负荷实验后：心搏量(SV)%12，变力状态指数(ISI)%13，%14；\n")
                    .arg(pevl(Type::CO), pevl(Type::CI), pevl(Type::SV), pevl(Type::SI), pevl(Type::HR),
                         pevl(Type::Vas), pevl(Type::Vol), pevl(Type::Ino), pevl(Type::SBP))
                    .arg(pevl(Type::DBP), pevl(Type::TFC), compare(Type::SV), compare(Type::ISI), preload());
            if (m_pHospitalInfo->cMode == Check_Mode::Hypertension) {
                // 高血压模式
                result += tr("3.前负荷(容量负荷)：%1；\n心肌力(心脏泵力)：%2；\n后负荷(张力负荷)：%3。").arg(fstr, sstr, tstr);
            }
            else if (m_pHospitalInfo->cMode == Check_Mode::InternalMedicine) {
                // 内科模式
                result += tr("3.前负荷(容量负荷)：%1；\n心肌力(心脏泵力)：%2；\n后负荷(张力负荷)：%3。").arg(fstr, sstr, tstr);
            }
            else if (m_pHospitalInfo->cMode == Check_Mode::Critical) {
                // 重症模式
                result += tr("3.前负荷(容量负荷)：%1；\n心肌力(心脏泵力)：%2；\n后负荷(张力负荷)：%3。").arg(fstr, sstr, tstr);
            }
            else if (m_pHospitalInfo->cMode == Check_Mode::PhysicalExamination) {
                // 体检模式
            }
        }
        else if (m_pHospitalInfo->pType == Printer_Type::Thermal) {
            //双体位热敏打印机报告
            result = tr("连续无创血流动力学对高血压病靶向分析报告如下：\n"
                         "1.第一体位：CO%1，CI%2，SV%3，SI%4，HR%5，Vas%6，Vol%7，Ino%8，SBP%9，DBP%10，TFC%11；\n"
                         "2.第二体位增加容量负荷实验后：SV%12，ISI%13，%14。")
                    .arg(pevl(Type::CO), pevl(Type::CI), pevl(Type::SV), pevl(Type::SI), pevl(Type::HR),
                         pevl(Type::Vas), pevl(Type::Vol), pevl(Type::Ino), pevl(Type::SBP))
                    .arg(pevl(Type::DBP), pevl(Type::TFC), compare(Type::SV), compare(Type::ISI), preload());
        }
    }
    else {  // 单体位
        result = tr("连续无创血流动力学对高血压病靶向分析报告如下：\n"
                    "1.心脏功能：心输出量(CO)%1，心搏量(SV)%2，心搏指数(SI)%3，心脏指数(CI)%4，血管容量(Vol)%5，"
                    "血管顺应性(Vas)%6，收缩变力性(Ino)%7；\n"
                    "2.血压管理：收缩压(SBP)%8，舒张压(DBP)%9，心率(HR)%10；")
                .arg(pevl(Type::CO, false), pevl(Type::SV, false), pevl(Type::SI, false),
                     pevl(Type::CI, false), pevl(Type::Vol, false), pevl(Type::Vas, false),
                     pevl(Type::Ino, false), pevl(Type::SBP, false), pevl(Type::DBP, false))
                .arg(pevl(Type::HR, false));
        if (m_pHospitalInfo->pType == Printer_Type::General) {
            // 单体位常规打印机报告
            // 体检模式
        }
        else if (m_pHospitalInfo->pType == Printer_Type::Thermal) {
            // 单体位热敏打印机报告
            // 体检模式
            result = tr("无创血流动力学检测系统评价，心脏动力，血管阻力，血液容量，血压等循环系统情况结论如下：\n"
                        "1.心脏功能：CO%1，SV%2，SI%3，CI%4，Vol%5，Vas%6，Ino%7；\n"
                        "2.血压管理：SBP%8，DBP%9，HR%10；")
                    .arg(pevl(Type::CO, false), pevl(Type::SV, false), pevl(Type::SI, false),
                         pevl(Type::CI, false), pevl(Type::Vol, false), pevl(Type::Vas, false),
                         pevl(Type::Ino, false), pevl(Type::SBP, false), pevl(Type::DBP, false))
                    .arg(pevl(Type::HR, false));
        }
    }
    return result;
}

QString DataManagement::pevl(const Type &type, bool many)
{
    CustomCtrl *temp = m_pRegulator->getCustomCtrl(typeName(type));
    if (type == Type::SBP || type == Type::DBP) {
        temp = m_pRegulator->getCustomCtrl("SBP/DBP");
    }
    if (temp == nullptr)
        return nullptr;

    qreal value = temp->getRecordValue();
    qreal min = temp->getMinValue();
    qreal max = temp->getMaxValue();
    if (type == Type::DBP) {
        value = temp->getDbpArgItems().recordValue;
        min = temp->getDbpArgItems().minValue;
        max = temp->getDbpArgItems().maxValue;
    }
    if (!many) {
        value = temp->getCurrentValue();
        if (type == Type::DBP) {
            value = temp->getDbpArgItems().currentValue;
        }
    }
    if (value < min)
        return tr("偏低");
    else if (value > max)
        return tr("偏高");
    return tr("正常");
}

QString DataManagement::compare(const Type &type)
{
    auto temp = m_pRegulator->getCustomCtrl(typeName(type));
    if (temp == nullptr) return nullptr;
    if (temp->getRecordValue() > temp->getCurrentValue()) return tr("下降");
    else if (temp->getRecordValue() < temp->getCurrentValue()) return tr("上升");
    return tr("不变");
}

QString DataManagement::preload()
{
    auto svCtrl = m_pRegulator->getCustomCtrl(typeName(Type::SV));
    auto isiCtrl = m_pRegulator->getCustomCtrl(typeName(Type::ISI));
    if (svCtrl == nullptr || isiCtrl == nullptr)
        return nullptr;
    qreal rsv = svCtrl->getRecordValue();
    qreal csv = svCtrl->getCurrentValue();
    qreal risi = isiCtrl->getRecordValue();
    qreal cisi = isiCtrl->getCurrentValue();
    QString strSv, strIsi;
    if (rsv < csv) {
        if (risi < cisi) {
            strSv = "正常";
            strIsi = "正常";
        }
        else if (risi == cisi) {
            strSv = "正常";
            strIsi = "下降";
        }
        else {
            strSv = "饱和";
            strIsi = "下降，疑似瓣膜反流";
        }
    }
    else {
        if (risi < cisi) {
            strSv = "正常，疑似瓣膜反流";
            strIsi = "正常";
        }
        else if (risi == cisi) {
            strSv = "饱和";
            strIsi = "下降";
        }
        else {
            strSv = "饱和";
            strIsi = "下降";
        }
    }
    return tr("前负荷%1，心力贮备%2").arg(strSv, strIsi);
}

QString DataManagement::riskTip(bool many)
{
    auto co = m_pRegulator->getCustomCtrl(typeName(Type::CO));
    auto isi = m_pRegulator->getCustomCtrl(typeName(Type::ISI));
    auto hr = m_pRegulator->getCustomCtrl(typeName(Type::HR));
    auto sv = m_pRegulator->getCustomCtrl(typeName(Type::SV));
    if (co == nullptr || isi == nullptr || hr == nullptr || sv == nullptr)
        return nullptr;
    int rank = 0;
    qreal coValue = co->getCurrentValue();
    qreal isiValue = isi->getCurrentValue();
    qreal hrValue = hr->getCurrentValue();
    qreal svValue = sv->getCurrentValue();
    if (many) {
        coValue = co->getRecordValue();
        isiValue = isi->getRecordValue();
        hrValue = hr->getRecordValue();
        svValue = sv->getRecordValue();
    }
    if (svValue < sv->getMinValue() && (hrValue > hr->getMaxValue() || isiValue < isi->getMinValue())) {
        rank = 3;
    }
    else {
        if (coValue > co->getMaxValue()) {
            ++rank;
        }
        if (isiValue > isi->getMaxValue()) {
            ++rank;
        }
        if (hrValue > hr->getMaxValue()) {
            ++rank;
        }
    }
    QString str;
    switch (rank) {
    case 2:
        str = "中";
        break;
    case 3:
        str = "高";
        break;
    default:
        str = "低";
        break;
    }
    return tr("心源性猝死风险%1。").arg(str);
}

QPixmap DataManagement::getQrCodeUrlPixmap(const QString &deviceId, const QString &reportTime)
{
    QString url = QString("https://s.zeyaotebco.com/tempAuth?type=bindDeviceReport&param=%1_%2")
            .arg(deviceId, reportTime);
    QRcode *qrcode; //二维码数据
    //QR_ECLEVEL_Q 容错等级
    qrcode = QRcode_encodeString(url.toStdString().c_str(), 2, QR_ECLEVEL_Q, QR_MODE_8, 1);
    qint32 temp_width = 100*zoom(); //二维码图片的大小
    qint32 temp_height = 100*zoom();
    qint32 qrcode_width = qrcode->width > 0 ? qrcode->width : 1;
    double scale_x = (double)temp_width/(double)qrcode_width; //二维码图片的缩放比例
    double scale_y = (double)temp_height/(double)qrcode_width;
    QImage mainimg = QImage(temp_width,temp_height,QImage::Format_ARGB32);
    QPainter painter(&mainimg);
    QColor background(Qt::white);
    painter.setBrush(background);
    painter.setPen(Qt::NoPen);
    painter.drawRect(0, 0, temp_width, temp_height);
    QColor foreground(Qt::black);
    painter.setBrush(foreground);
    for( qint32 y = 0; y < qrcode_width; y ++)
    {
        for(qint32 x = 0; x < qrcode_width; x++)
        {
            unsigned char b = qrcode->data[y * qrcode_width + x];
            if(b & 0x01)
            {
                QRectF r(x * scale_x, y * scale_y, scale_x, scale_y);
                painter.drawRects(&r, 1);
            }
        }
    }
    QRcode_free(qrcode);
    return QPixmap::fromImage(mainimg);
}

void DataManagement::addPlrt(const int &num, const Type &type)
{
    auto ctrl = m_pRegulator->getCustomCtrl(typeName(type));
    qreal rvalue = ctrl->getRecordValue();
    qreal cvalue = ctrl->getCurrentValue();
    reportThread->addMarks(rvalue == 0 ? "-" : QString("plrt0%1").arg(num), QString::number(rvalue));
    reportThread->addMarks(cvalue == 0 ? "-" : QString("plrt1%1").arg(num), QString::number(cvalue));
    double percent = (int(((rvalue - cvalue)/cvalue + 0.05)*10))/10.0;
    reportThread->addMarks(rvalue == 0 || cvalue == 0 ? "-" : QString("plrt2%1").arg(num), QString::number(percent));
}

QString DataManagement::posImagePath(const QString &posture)
{
    QImage image;
    if (posture == "半卧") {
        image.load(":/images/halfsleeper.png");
    }
    else if (posture == "平躺") {
        image.load(":/images/lieflat.png");
    }
    else if (posture == "抬腿") {
        image.load(":/images/leglift.png");
    }
    QString fileName = m_filePath.tempDir() + posture + ".png";
    image.save(fileName, "PNG");
    return fileName;
}

QString DataManagement::getScope(const double &init, const double &offset)
{
    return QString("%1~%2").arg(init - offset).arg(init + offset);
}

QString DataManagement::getScope(const HrvArg &hrvArg, const int &age)
{
    Q_UNUSED(age);
    QString scope;
    switch (hrvArg) {
    case HrvArg::Nnvgr:
        scope = "-";
        break;
    case HrvArg::Sdnn:
//        if (age < 30) {
//            scope = getScope(169.92, 41.01);
//        }
//        else if (age < 50) {
//            scope = getScope(148.31, 32.80);
//        }
//        else {
//            scope = getScope(121.19, 29.27);
//        }
        scope = getScope(141, 39);
        break;
    case HrvArg::Pnn50:
        scope = getScope(16.7, 12.3);
        break;
    case HrvArg::Rmssd:
//        if (age < 30) {
//            scope = getScope(12.39, 47.10);
//        }
//        else if (age < 50) {
//            scope = getScope(48.40, 20.90);
//        }
//        else {
//            scope = getScope(40.40, 18.29);
//        }
        scope = getScope(39, 15);
        break;
    }
    return scope;
}
