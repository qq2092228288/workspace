#include "datamanagement.h"


DataManagement::DataManagement()
{
    isRecord = false;
    m_pThread = new QThread;
    m_pTebco = new ZyTebco;
    m_pTebco->moveToThread(m_pThread);
    reportThread = new CreateReportThread;
    m_pIdCheck = new IdCheck;
}

DataManagement::~DataManagement()
{
    reportThread->quit();
    reportThread->wait();
    m_pThread->quit();
    m_pThread->wait();
    delete reportThread;
    delete m_pTebco;
    delete m_pThread;
    delete m_pIdCheck;
    QDir dir(m_filePath.tempDir()); // delete temp dir
    dir.removeRecursively();
//    qDebug()<<"~DataManagement()";
}

void DataManagement::startThread()
{
    m_pThread->start();
}

ZyTebco *DataManagement::getTebco() const
{
    return m_pTebco;
}

IdCheck *DataManagement::getIdCheck() const
{
    return m_pIdCheck;
}

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

void DataManagement::recordPosition(QString position)
{
    m_recordInfo.pos = position;
    m_recordInfo.posture = position == tr("半卧") ? "1" : (position == tr("平躺") ? "2" : "3");
    saveInfo(m_recordInfo);
    m_pdZ->grab().scaled(300,120,Qt::IgnoreAspectRatio,Qt::SmoothTransformation).save(m_filePath.record_dz());
    isRecord = true;
}

QString DataManagement::saveReport(QString position, bool record)
{
    emit startCheck(false);
    m_newReportName.clear();
    m_currentInfo.pos = position;
    saveInfo(m_currentInfo,record);
    m_pdZ->grab().scaled(300,120,Qt::IgnoreAspectRatio,Qt::SmoothTransformation).save(m_filePath.current_dz());
    m_pSudoku->grab().save(m_filePath.sudoku());

    reportThread->init();
    connect(reportThread,&CreateReportThread::finished,this,&DataManagement::clear);
    //当前时间
    QDateTime curTime = QDateTime::currentDateTime();
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
