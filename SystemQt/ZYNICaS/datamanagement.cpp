#include "datamanagement.h"


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

QString MyFilePath::hypertension_dot() const
{
    return appPath + "dot/hypertensionTemplate.dot";
}

QString MyFilePath::hypertension1_dot() const
{
    return appPath + "dot/hypertensionTemplate1.dot";
}

QString MyFilePath::interMedicine_dot() const
{
    return appPath + "dot/interMedicineTemplate.dot";
}

QString MyFilePath::interMedicine1_dot() const
{
    return appPath + "dot/interMedicineTemplate1.dot";
}

QString MyFilePath::icu_dot() const
{
    return appPath + "dot/icuTemplate.dot";
}

QString MyFilePath::icu1_dot() const
{
    return appPath + "dot/icuTemplate1.dot";
}

QString MyFilePath::mPhyExam_dot() const
{
    return appPath + "dot/manyPhyExamTemplate.dot";
}

QString MyFilePath::mPhyExam1_dot() const
{
    return appPath + "dot/manyPhyExamTemplate1.dot";
}

QString MyFilePath::sPhyExam_dot() const
{
    return appPath + "dot/singlePhyExamTemplate.dot";
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

QString MyFilePath::hospitalLogo() const
{
    return initDir() + "hospitalLogo.png";
}

QString MyFilePath::userAccountInfo() const
{
    return initDir() + "userAccountInfo.ini";
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
        <<Argument("搏排量", "SV", "mL/beat", 50, 120, 1)
        <<Argument("心搏指数", "SI", "mL/beat·m²", 35, 65, 0)
        <<Argument("心率变异性", "HRV", "%", -50, 50, 0)
        <<Argument("胸液传导性", "TFC", "1/Ω", 0.025, 0.045, 3)
        <<Argument("舒张末期指数", "EDI", "mL/beat·m²", 54, 130, 0)
        <<Argument("每搏输出变异性", "SVV", "%", 0, 10, 0)
        <<Argument("血管容积", "Vol", "%", -50, 50, 0)
        <<Argument("系统阻力", "SVR", "dyn·s·m²/cm⁵", 742, 1500, 0)
        <<Argument("每搏外周阻力", "SSVR", "dyn·s/cm⁵", 119.6, 429.2, 1)
        <<Argument("每搏外周阻力指数", "SSVRI", "dyn·s/cm⁵/beat/m²", 99.7, 185.1, 1)
        <<Argument("系统阻力指数", "SVRI", "dyn·s·m²/cm⁵", 1337, 2483, 0)
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
    m_pTebco = new ZyTebco;
    m_client = new MqttClient;
}

DataManagement::~DataManagement()
{
    delete m_pTebco;
    delete m_client;
}

ZyTebco *DataManagement::getTebco() const
{
    return m_pTebco;
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
    return QSize(w*wZoom(), h*hZoom());
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

MqttClient *DataManagement::mqttClient() const
{
    return m_client;
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

QString DataManagement::reportCreated(bool record)
{
    emit startCheck(false);
    return reportResult(record);
}

void DataManagement::clearSlot()
{
    //断开数据传输
    m_pTebco->startCheckSlot(false);
    m_pTebco->clearSerial();
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
    if (m_pHospitalInfo->cMode == Check_Mode::Hypertension ||
            m_pHospitalInfo->cMode == Check_Mode::PhysicalExamination) {
        result = tr("连续无创血流动力学对高血压病靶向分析报告如下：\n");
    }
    else {
        result = tr("连续无创血流动力学分析报告如下：\n");
    }
    if (record) {   // 双体位
        if (m_pHospitalInfo->pType == Printer_Type::General) {
            // 双体位常规打印机报告
            auto isi = m_pRegulator->getCustomCtrl(typeName(Type::ISI));
            auto sv = m_pRegulator->getCustomCtrl(typeName(Type::SV));
            auto vas = m_pRegulator->getCustomCtrl(typeName(Type::Vas));
            auto hrv = m_pRegulator->getCustomCtrl(typeName(Type::HRV));
            auto map = m_pRegulator->getCustomCtrl(typeName(Type::MAP));
            QString fstr = tr("前负荷(容量负荷)：") + (isi->getCurrentValue() > isi->getRecordValue() && sv->getCurrentValue() > sv->getRecordValue() ?
                                tr("正常") : tr("偏高(建议使用：减少钠盐摄入，使用利尿剂)，请结合临床分析"));
            QString tstr = tr("后负荷(张力负荷)：");
            if (map->getRecordValue() >= map->getMinValue() && map->getRecordValue() <= map->getMaxValue()) {
                if (vas->getRecordValue() >= vas->getMinValue() && vas->getRecordValue() <= vas->getMaxValue()) {
                    tstr += tr("正常");
                }
                else {
                    tstr += tr("正常(因血压正常，后负荷有益代偿)");
                }
            }
            else if (map->getRecordValue() > map->getMaxValue()) {
                if (vas->getRecordValue() >= vas->getMinValue() && vas->getRecordValue() <= vas->getMaxValue()) {
                    tstr += tr("正常");
                }
                else if (vas->getRecordValue() > vas->getMaxValue()) {
                    tstr += tr("偏高(建议使用：ACEI，ARB)，请结合临床分析");
                }
            }
            else if (map->getRecordValue() < map->getMinValue()) {
                if (vas->getRecordValue() <= vas->getMaxValue()) {
                    tstr += tr("偏低");
                }
            }
            QString sstr = tr("心肌力(心脏泵力)：");
            if (isi->getRecordValue() < isi->getMinValue()) {
                sstr += tr("偏低");
            }
            else if (isi->getRecordValue() > isi->getMaxValue()) {
                sstr += tr("偏高(建议使用：β阻滞剂类，CCB)，请结合临床分析");
            }
            else {
                sstr += tr("正常");
            }
            QString hstr = tr("心率变异性（交感神经兴奋度）：");
            if (hrv->getRecordValue() < hrv->getMinValue()) {
                hstr += tr("偏低");
            }
            else if (hrv->getRecordValue() > hrv->getMaxValue()) {
                hstr += tr("偏高(建议使用：β阻滞剂，镇静剂类)，请结合临床分析");
            }
            else {
                hstr += tr("正常");
            }

            if (m_pHospitalInfo->cMode == Check_Mode::Hypertension) {
                // 高血压模式
                result += tr("1.%1；\n2.%2；\n3.%3；\n4.%4；\n").arg(fstr, tstr, sstr, hstr);
            }
            else if (m_pHospitalInfo->cMode == Check_Mode::InternalMedicine) {
                // 内科模式
                result += tr("1.%1；\n2.%2；\n3.%3；\n4.%4；\n").arg(fstr, tstr, sstr, hstr);
            }
            else if (m_pHospitalInfo->cMode == Check_Mode::IntensiveCareUnit) {
                // 重症模式
                result += tr("1.第一体位：心输出量(CO)%1，心脏指数(CI)%2，搏排量(SV)%3，心搏指数(SI)%4，"
                            "心率(HR)%5，血管顺应性(Vas)%6，血管容量(Vol)%7，收缩变力性(Ino)%8，"
                            "收缩压(SBP)%9，舒张压(DBP)%10，胸液传导性(TFC)%11；\n"
                            "2.第二体位增加容量负荷实验后：搏排量(SV)%12，变力状态指数(ISI)%13，%14；\n")
                        .arg(pevl(Type::CO), pevl(Type::CI), pevl(Type::SV), pevl(Type::SI), pevl(Type::HR),
                             pevl(Type::Vas), pevl(Type::Vol), pevl(Type::Ino), pevl(Type::SBP))
                        .arg(pevl(Type::DBP), pevl(Type::TFC), compare(Type::SV), compare(Type::ISI), preload());
                result += tr("3.%1；\n  %2；\n  %3；\n  %4；").arg(fstr, tstr, sstr, hstr);
            }
            else if (m_pHospitalInfo->cMode == Check_Mode::PhysicalExamination) {
                // 体检模式
                result += tr("1.%1；\n2.%2；\n3.%3；\n4.%4；\n").arg(fstr, tstr, sstr, hstr);
            }
        }
        else if (m_pHospitalInfo->pType == Printer_Type::Thermal) {
            //双体位热敏打印机报告
            result = tr("无创血流动力学检测系统评价，心脏动力，血管阻力，血液容量，血压等循环系统情况结论如下：\n"
                        "1.第一体位：CO%1，CI%2，SV%3，SI%4，HR%5，Vas%6，Vol%7，Ino%8，SBP%9，DBP%10，TFC%11；\n"
                        "2.第二体位增加容量负荷实验后：SV%12，ISI%13，%14。")
                    .arg(pevl(Type::CO), pevl(Type::CI), pevl(Type::SV), pevl(Type::SI), pevl(Type::HR),
                         pevl(Type::Vas), pevl(Type::Vol), pevl(Type::Ino), pevl(Type::SBP))
                    .arg(pevl(Type::DBP), pevl(Type::TFC), compare(Type::SV), compare(Type::ISI), preload());
        }
    }
    else {  // 单体位
        auto vol = m_pRegulator->getCustomCtrl(typeName(Type::Vol));
        auto isi = m_pRegulator->getCustomCtrl(typeName(Type::ISI));
        auto vas = m_pRegulator->getCustomCtrl(typeName(Type::Vas));
        auto hrv = m_pRegulator->getCustomCtrl(typeName(Type::HRV));
        auto map = m_pRegulator->getCustomCtrl(typeName(Type::MAP));
        QString fstr = tr("前负荷(容量负荷)：");
        if (vol->getCurrentValue() < vol->getMinValue()) {
            fstr += tr("偏低");
        }
        else if (vol->getCurrentValue() > vol->getMaxValue()) {
            fstr += tr("偏高(建议使用：减少钠盐摄入，使用利尿剂)，请结合临床分析；");
        }
        else {
            fstr += tr("正常");
        }
        QString tstr = tr("后负荷(张力负荷)：");
        if (vas->getCurrentValue() < vas->getMinValue()) {
            tstr += tr("偏低");
        }
        else if (vas->getCurrentValue() > vas->getMaxValue()) {
            tstr += tr("偏高(建议使用：ACEI，ARB)，请结合临床分析；");
        }
        else {
            tstr += tr("正常");
        }
        QString sstr = tr("心肌力(心脏泵力)：");
        if (isi->getCurrentValue() < isi->getMinValue()) {
            sstr += tr("偏低");
        }
        else if (isi->getCurrentValue() > isi->getMaxValue()) {
            sstr += tr("偏高(建议使用：β阻滞剂类，CCB)，请结合临床分析；");
        }
        else {
            sstr += tr("正常");
        }
        QString hstr = tr("心率变异性（交感神经兴奋度）：");
        if (hrv->getCurrentValue() < hrv->getMinValue()) {
            hstr += tr("偏低");
        }
        else if (hrv->getCurrentValue() > hrv->getMaxValue()) {
            hstr += tr("偏高(建议使用：β阻滞剂，镇静剂类)，请结合临床分析；");
        }
        else {
            hstr += tr("正常");
        }
        if (map->getCurrentValue() < map->getMinValue() && vas->getCurrentValue() > vas->getMaxValue()) {
            tstr += tr("(提示：病人血压低，后负荷被动性代偿)");
        }
        result += tr("1.%1\n2.%2\n3.%3\n4.%4\n").arg(fstr, tstr, sstr, hstr);
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
    strSv = (cisi > risi && csv > rsv ? tr("正常") : tr("异常"));

    if (rsv < csv) {
        if (risi < cisi) {
//            strSv = "正常";
            strIsi = "正常";
        }
        else if (risi == cisi) {
//            strSv = "正常";
            strIsi = "下降";
        }
        else {
//            strSv = "饱和";
            strIsi = "下降，疑似瓣膜反流";
        }
    }
    else {
        if (risi < cisi) {
//            strSv = "正常，疑似瓣膜反流";
            strIsi = "正常";
        }
        else if (risi == cisi) {
//            strSv = "饱和";
            strIsi = "下降";
        }
        else {
//            strSv = "饱和";
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

//QPixmap DataManagement::getQrCodeUrlPixmap(const QString &deviceId, const QString &reportTime)
//{
//    QString url = QString("https://s.zeyaotebco.com/tempAuth?type=bindDeviceReport&param=%1_%2")
//            .arg(deviceId, reportTime);
//    QRcode *qrcode; //二维码数据
//    //QR_ECLEVEL_Q 容错等级
//    qrcode = QRcode_encodeString(url.toStdString().c_str(), 2, QR_ECLEVEL_Q, QR_MODE_8, 1);
//    qint32 temp_width = 100*zoom(); //二维码图片的大小
//    qint32 temp_height = 100*zoom();
//    qint32 qrcode_width = qrcode->width > 0 ? qrcode->width : 1;
//    double scale_x = (double)temp_width/(double)qrcode_width; //二维码图片的缩放比例
//    double scale_y = (double)temp_height/(double)qrcode_width;
//    QImage mainimg = QImage(temp_width,temp_height,QImage::Format_ARGB32);
//    QPainter painter(&mainimg);
//    QColor background(Qt::white);
//    painter.setBrush(background);
//    painter.setPen(Qt::NoPen);
//    painter.drawRect(0, 0, temp_width, temp_height);
//    QColor foreground(Qt::black);
//    painter.setBrush(foreground);
//    for( qint32 y = 0; y < qrcode_width; y ++)
//    {
//        for(qint32 x = 0; x < qrcode_width; x++)
//        {
//            unsigned char b = qrcode->data[y * qrcode_width + x];
//            if(b & 0x01)
//            {
//                QRectF r(x * scale_x, y * scale_y, scale_x, scale_y);
//                painter.drawRects(&r, 1);
//            }
//        }
//    }
//    QRcode_free(qrcode);
//    return QPixmap::fromImage(mainimg);
//}
