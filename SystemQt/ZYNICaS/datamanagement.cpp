#include "datamanagement.h"
#include "qrencode.h"

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
        <<Argument("心搏量", "SV", "mL/beat", 50, 120, 1)
        <<Argument("心搏指数", "SI", "mL/beat·m²", 35, 65, 0)
        <<Argument("心率变异性", "HRV", "%", -50, 50, 0)
        <<Argument("胸液传导性", "TFC", "1/Ω", 0.025, 0.045, 3)
        <<Argument("舒张末期指数", "EDI", "mL/beat·m²", 54, 130, 0)
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

QString DataManagement::dialogQss() const
{
    int fsize = 18*zoom()+1;
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

void DataManagement::setSudoku(DrawSudoku *sudoku)
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
    m_recordInfo.posture = position == tr("半卧") ? "1" : (position == tr("平躺") ? "2" : "3");
    saveInfo(m_recordInfo);
    m_pdZ->grab().scaled(300,120,Qt::IgnoreAspectRatio,Qt::SmoothTransformation).save(m_filePath.record_dz());
    isRecord = true;
}

QString DataManagement::saveReport(QDateTime curTime, QString position, bool record)
{
    emit startCheck(false);
    m_newReportName.clear();
    m_currentInfo.pos = position;
    saveInfo(m_currentInfo,record);
    m_pdZ->grab().scaled(300,120,Qt::IgnoreAspectRatio,Qt::SmoothTransformation).save(m_filePath.current_dz());
    m_pSudoku->grab().save(m_filePath.sudoku());

    reportThread->init();
    connect(reportThread,&CreateReportThread::finished,this,&DataManagement::clear);
    // 打开模板
    QString result = tr("无创血流动力学检测系统评价，心脏动力，血管阻力，血液容量，血压等循环系统情况结论如下：\n");
    if (record) {       // 多体位
        reportThread->addMarks("rpos", m_recordInfo.pos);
        if (!m_pHospitalInfo->xprinter) {
            reportThread->setOpenArg(m_filePath.many_dot(),false);
            // 记录体位
            QStringList rList;
            for (int i = 0; i < 27; ++i) {
                rList<<QString("r%1").arg(i,2,10,QLatin1Char('0'));
            }
            reportThread->addBatchMarks(rList, m_recordInfo.values);
            reportThread->addMarks("rname", (m_recordInfo.pos + tr(" 心阻抗图(dZ)")));
            // 插入图片
            reportThread->addPic("rimage",m_filePath.record_dz());
            result += tr("1.第一体位：心输出量(CO)%1，心脏指数(CI)%2，心搏量(SV)%3，心搏指数(SI)%4，心率(HR)%5，血管顺应性(Vas)%6，"
                "血管容量(Vol)%7，收缩变力性(Ino)%8，收缩压(SBP)%9，舒张压(DBP)%10，胸液传导性(TFC)%11；\n"
                "2.第二体位增加容量负荷实验后：心搏量(SV)%12，变力状态指数(ISI)%13，%14。")
                    .arg(pevl(Type::CO),pevl(Type::CI),pevl(Type::SV),pevl(Type::SI),pevl(Type::HR),
                         pevl(Type::Vas),pevl(Type::Vol),pevl(Type::Ino),pevl(Type::SBP))
                    .arg(pevl(Type::DBP),pevl(Type::TFC),compare(Type::SV),compare(Type::ISI),preload());
        }
        else {
            reportThread->setOpenArg(m_filePath.xmany_dot(),false);
            QStringList rList;
            for (int i = 0; i < 12; ++i) {
                rList<<QString("r%1").arg(i,2,10,QLatin1Char('0'));
            }
            QStringList values = m_recordInfo.values.mid(0,4);
            values<<m_recordInfo.values.at(7)<<m_recordInfo.values.at(12)
                 <<m_recordInfo.values.mid(20,6);
            reportThread->addBatchMarks(rList, values);
            result += tr("1.第一体位：CO%1，CI%2，SV%3，SI%4，HR%5，Vas%6，Vol%7，Ino%8，SBP%9，DBP%10，TFC%11；\n"
                "2.第二体位增加容量负荷实验后：SV%12，ISI%13，%14。")
                    .arg(pevl(Type::CO),pevl(Type::CI),pevl(Type::SV),pevl(Type::SI),pevl(Type::HR),
                         pevl(Type::Vas),pevl(Type::Vol),pevl(Type::Ino),pevl(Type::SBP))
                    .arg(pevl(Type::DBP),pevl(Type::TFC),compare(Type::SV),compare(Type::ISI),preload());
        }
    }
    else {      // 单体位
        if (!m_pHospitalInfo->xprinter) {
            reportThread->setOpenArg(m_filePath.single_dot(),false);
            result += tr("1.心脏功能：心输出量(CO)%1，心搏量(SV)%2，心搏指数(SI)%3，心脏指数(CI)%4，血管容量(Vol)%5，血管顺应性(Vas)%6，收缩变力性(Ino)%7；\n"
                "2.血压管理：收缩压(SBP)%8，舒张压(DBP)%9，心率(HR)%10；")
                    .arg(pevl(Type::CO,false),pevl(Type::SV,false),pevl(Type::SI,false),pevl(Type::CI,false),pevl(Type::Vol,false),
                         pevl(Type::Vas,false),pevl(Type::Ino,false),pevl(Type::SBP,false),pevl(Type::DBP,false))
                    .arg(pevl(Type::HR,false));
        }
        else {
            reportThread->setOpenArg(m_filePath.xsingle_dot(),false);
            result += tr("1.心脏功能：CO%1，SV%2，SI%3，CI%4，Vol%5，Vas%6，Ino%7；\n"
                "2.血压管理：SBP%8，DBP%9，HR%10；")
                    .arg(pevl(Type::CO,false),pevl(Type::SV,false),pevl(Type::SI,false),pevl(Type::CI,false),pevl(Type::Vol,false),
                         pevl(Type::Vas,false),pevl(Type::Ino,false),pevl(Type::SBP,false),pevl(Type::DBP,false))
                    .arg(pevl(Type::HR,false));
        }
    }
    reportThread->addMarks("result", result);
    QString tip = tr("提示：血流动力学检测异常会增加心源性猝死的风险，请结合临床相关检查。");
    if (m_pHospitalInfo->tip) {
        tip = QString("%1(%2)").arg(riskTip(record),tip);
        reportThread->addMarks("tick", tr("血流动力学检测： 未见明显异常 [  ]    异常 [  ]"));
    }
    reportThread->addMarks("tip", tip);
    // 当前体位
    reportThread->addMarks("cpos", m_currentInfo.pos);
    if (!m_pHospitalInfo->xprinter) {
        QStringList cList;
        for (int i = 0; i < 27; ++i) {
            cList<<QString("c%1").arg(i,2,10,QLatin1Char('0'));
        }
        reportThread->addBatchMarks(cList, m_currentInfo.values);
        reportThread->addMarks("cname", (m_currentInfo.pos + tr(" 心阻抗图(dZ)")));
        reportThread->addPic("cimage",m_filePath.current_dz());
        reportThread->addMarks("room" , m_pHospitalInfo->roomName);
    }
    else {
        QStringList rList;
        for (int i = 0; i < 12; ++i) {
            rList<<QString("c%1").arg(i,2,10,QLatin1Char('0'));
        }
        QStringList values = m_currentInfo.values.mid(0,4);
        values<<m_currentInfo.values.at(7)<<m_currentInfo.values.at(12)
             <<m_currentInfo.values.mid(20,6);
        reportThread->addBatchMarks(rList, values);
    }
    reportThread->addMarks("sname", tr("九宫分析图"));
    reportThread->addPic("simage",m_filePath.sudoku());
    // 医院、时间、检查人员
    reportThread->addMarks("hospital" , m_pHospitalInfo->hospitalName);
    reportThread->addMarks("time" , curTime.toString("yyyy-MM-dd hh:mm"));
    reportThread->addMarks("doctor" , m_pHospitalInfo->doctorName);
    // 患者基本信息
    reportThread->addMarks("name" , m_pBodyValue->name);
    reportThread->addMarks("sex", (0 == m_pBodyValue->sex) ? tr("男") : tr("女"));
    reportThread->addMarks("age" , QString::number(m_pBodyValue->age)+tr(" 岁"));
    reportThread->addMarks("height" , QString::number(m_pBodyValue->height)+" cm");
    reportThread->addMarks("weight" , QString::number(m_pBodyValue->weight)+" kg");
    reportThread->addMarks("number" , m_pBodyValue->id);
    reportThread->addMarks("area" , QString::number(m_pBodyValue->BSA(),'f',1)+" m²");
    // 二维码
    getQrCodeUrlPixmap(m_pHospitalInfo->deviceId, curTime.toString("yyyyMMddHHmmss")).save(m_filePath.qrCode());
    reportThread->addPic("qrcode", m_filePath.qrCode());
    // 保存
    m_newReportName = QString("%1/%2-%3-%4.docx").arg(m_filePath.reports(),
                      curTime.toString("yyyyMMddhhmm"),m_pBodyValue->id,m_pBodyValue->name);
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
            cdata.values<<flag(customCtrl, second);
            auto args = customCtrl->getArgItems();
        }
        else {
            auto sbp = customCtrl->getArgItems();
            auto dbp = customCtrl->getDbpArgItems();
            if (!second) {
                cdata.values<<tip(sbp.minValue,sbp.maxValue,sbp.currentValue)
                            <<tip(dbp.minValue,dbp.maxValue,dbp.currentValue);
            }
            else {
                cdata.values<<tip(sbp.recordValue,sbp.currentValue)
                            <<tip(dbp.recordValue,dbp.currentValue);
            }
        }
    }
}

QString DataManagement::flag(CustomCtrl *customCtrl, bool second)
{
    QStringList list = QStringList()<<"CO"<<"CI"<<"SV"<<"SI"<<"TFC"<<"EPCI"<<"ISI"<<"Ino"<<"HR"<<"MAP";
    if (second) {   // 第二体位
        if (list.indexOf(customCtrl->getName()) != -1) {
            return tip(customCtrl->getRecordValue(),customCtrl->getCurrentValue());
        }
    }
    else {          // 第一体位
        list<<"Vol"<<"HRV"<<"Vas";
        if (list.indexOf(customCtrl->getName()) != -1) {
            return tip(customCtrl->getMinValue(),customCtrl->getMaxValue(),customCtrl->getCurrentValue());
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
    }
    if (!many) {
        value = temp->getCurrentValue();
        if (type == Type::DBP) {
            value = temp->getDbpArgItems().currentValue;
            min = temp->getDbpArgItems().minValue;
            max = temp->getDbpArgItems().maxValue;
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
    if (temp == nullptr)
        return nullptr;
    if (temp->getRecordValue() > temp->getCurrentValue())
        return tr("下降");
    else if (temp->getRecordValue() < temp->getCurrentValue())
        return tr("上升");
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
    return tr("前负荷%1，心力贮备%2").arg(strSv,strIsi);
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
