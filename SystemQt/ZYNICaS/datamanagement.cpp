#include "datamanagement.h"
#include "reportdatajson.h"
#include "reportdataname.h"

MyFilePath::MyFilePath(const QString &path)
    : appPath{path}
{

}

QString MyFilePath::initDir() const
{
    return appPath + "init/";
}

QString MyFilePath::baseInfo() const
{
    return initDir() + "baseInfo.ini";
}

QString MyFilePath::reports() const
{
    return appPath + "reports/";
}

QString MyFilePath::showItems() const
{
    return initDir() + "showItems.ini";
}

QString MyFilePath::trendCharts() const
{
    return initDir() + "trendCharts.ini";
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

Argument::Argument(QString _cn, QString _en, QString _unit, qreal _min, qreal _max, int _digit, Type _type)
    : cn(_cn),en(_en),unit(_unit),min(_min),max(_max),digit(_digit),type(_type)
{

}

Argument::Argument(QString _cn, QString _en, QString _unit, qreal _sbpmin, qreal _sbpmax, int _digit, Type _type,
                   QString _dbpcn, QString _dbpen, qreal _dbpmin, qreal _dbpmax)
    : cn(_cn),en(_en),unit(_unit),min(_sbpmin),max(_sbpmax),digit(_digit),type(_type),
      dbpcn(_dbpcn),dbpen(_dbpen),dbpmin(_dbpmin),dbpmax(_dbpmax)
{

}

Arguments::Arguments()
{
//    arguments = QList<Argument>()
//        <<Argument("心输出量", "CO", "L/min", 3.5, 9.0, 1)
//        <<Argument("心脏指数", "CI", "L/min·m²", 2.0, 5.0, 1)
//        <<Argument("氧输送", "DO2", "mL/min", 1000, 1000, 0)
//        <<Argument("搏排量", "SV", "mL/beat", 50, 120, 1)
//        <<Argument("心搏指数", "SI", "mL/beat·m²", 35, 65, 0)
//        <<Argument("心率变异性", "HRV", "%", -50, 50, 0)
//        <<Argument("胸液传导性", "TFC", "1/Ω", 0.025, 0.045, 3)
//        <<Argument("舒张末期指数", "EDI", "mL/beat·m²", 54, 130, 0)
//        <<Argument("每搏输出变异性", "SVV", "%", 0, 10, 0)
//        <<Argument("血管容积", "Vol", "%", -50, 50, 0)
//        <<Argument("系统阻力", "SVR", "dyn·s·m²/cm⁵", 742, 1500, 0)
//        <<Argument("每搏外周阻力", "SSVR", "dyn·s/cm⁵", 119.6, 429.2, 1)
//        <<Argument("每搏外周阻力指数", "SSVRI", "dyn·s/cm⁵/beat/m²", 99.7, 185.1, 1)
//        <<Argument("系统阻力指数", "SVRI", "dyn·s·m²/cm⁵", 1337, 2483, 0)
//        <<Argument("血管顺应性", "Vas", "%", -50, 50, 0)
//        <<Argument("射血前期", "PEP", "ms", 50, 120, 0)
//        <<Argument("左室射血时间", "LVET", "ms", 250, 350, 0)
//        <<Argument("左心室每搏做功", "LSW", "g·m", 20, 90, 0)
//        <<Argument("每搏做功指数", "LSWI", "g·m/beat/m²", 39.3, 73.0, 1)
//        <<Argument("左心室做功", "LCW", "kg·m", 5.4, 10.0, 1)
//        <<Argument("左心室做功指数", "LCWI", "kg·m/m²", 1.81, 7.06, 2)
//        <<Argument("收缩时间比", "STR", "-", 0, 0.4, 1)
//        <<Argument("射血期收缩指数", "EPCI", "1/s", 0.038, 0.076, 3)
//        <<Argument("变力状态指数", "ISI", "1/s²", 0.90, 1.70, 2)
//        <<Argument("收缩变力性", "Ino", "%", -50, 50, 0)
//        <<Argument("心率", "HR", "bpm", 60, 100, 0)
//        <<Argument("收缩压", "SBP", "mmHg", 90, 140, 0, "舒张压", "DBP", 60, 90)
    //        <<Argument("平均动脉压", "MAP", "mmHg", 70, 105, 0);
}

void Arguments::init()
{
    // func函数获取object对应key的值
    auto func = [=](const QJsonObject &object, const ReportDataName::ReportJson &key) {
        return object.value(ReportDataName::ekey(key));
    };
    auto array = ReportParameters::array(Check_Mode::IntensiveCareUnit);

    foreach (auto module, array) {
        auto parameters = func(module.toObject(), ReportDataName::parameters).toArray();
        QJsonObject sbp, dbp;
        foreach (auto parameter, parameters) {
            auto obj = parameter.toObject();
            auto type = func(obj, ReportDataName::type).toInt();
            if (Type::SBP == type) {
                sbp = obj;
            }
            else if (Type::DBP == type) {
                dbp = obj;
            }
            else {
                arguments<<Argument(func(obj, ReportDataName::cn).toString(),
                                    func(obj, ReportDataName::en).toString(),
                                    func(obj, ReportDataName::unit).toString(),
                                    func(obj, ReportDataName::min).toDouble(),
                                    func(obj, ReportDataName::max).toDouble(),
                                    func(obj, ReportDataName::digit).toInt(),
                                    Type(func(obj, ReportDataName::type).toInt()));
            }
            if (!sbp.empty() && !dbp.empty()) {
                arguments<<Argument(func(sbp, ReportDataName::cn).toString(),
                                    func(sbp, ReportDataName::en).toString(),
                                    func(sbp, ReportDataName::unit).toString(),
                                    func(sbp, ReportDataName::min).toDouble(),
                                    func(sbp, ReportDataName::max).toDouble(),
                                    func(sbp, ReportDataName::digit).toInt(),
                                    Type(func(sbp, ReportDataName::type).toInt()),
                                    func(dbp, ReportDataName::cn).toString(),
                                    func(dbp, ReportDataName::en).toString(),
                                    func(dbp, ReportDataName::min).toDouble(),
                                    func(dbp, ReportDataName::max).toDouble());
                sbp = QJsonObject();
                dbp = QJsonObject();
            }
        }
    }
}

DataManagement::DataManagement()
{
    m_pTebco = new ZyTebco;
    m_client = new MqttClient;
    m_departmentName = "科室";
    m_idName = "病历号";
    QFile file(m_filePath.baseInfo());
    if(file.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream out(&file);
        out.setCodec(QTextCodec::codecForName("utf-8"));
        QRegExp nameExp("(.*)(?:=)");
        QRegExp valueExp("(?:\")(.*)(?:\")");
        while (!out.atEnd()) {
            QString strLine = out.readLine();
            if (strLine.indexOf(nameExp) >= 0 && strLine.indexOf(valueExp) >= 0) {
                QString name = nameExp.cap(1);
                QString value = valueExp.cap(1);
                if (name == "departmentname") {
                    m_departmentName = QString::fromStdString(value.toStdString());
                }
                else if (name == "idname") {
                    m_idName = QString::fromStdString(value.toStdString());
                }
            }
        }
        file.close();
    }
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

QString DataManagement::departmentName() const
{
    return m_departmentName;
}

QString DataManagement::idName() const
{
    return m_idName;
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

QString DataManagement::reportResult(const QJsonObject &json)
{
    auto position = json.value(ReportDataName::ekey(ReportDataName::position)).toArray();
    if (0 == position.count()) return QString();
    QStringList result;
    if (m_pHospitalInfo->cMode == Check_Mode::Hypertension ||
            m_pHospitalInfo->cMode == Check_Mode::PhysicalExamination) {
        result<<tr("连续无创血流动力学对高血压病靶向分析报告如下：");
    }
    else if (m_pHospitalInfo->cMode == Check_Mode::Health) {
        result<<tr("报告分析如下：");
    }
    else {
        result<<tr("连续无创血流动力学分析报告如下：");
    }
    auto min = ReportDataName::ekey(ReportDataName::min);
    auto max = ReportDataName::ekey(ReportDataName::max);
    auto info = json.value(ReportDataName::ekey(ReportDataName::patientInfo)).toObject();
    auto fdata = position.first().toObject().value(ReportDataName::ekey(ReportDataName::data)).toObject();
    auto parameters = ReportParameters::array(Check_Mode::IntensiveCareUnit);
    auto fmap = ReportDataJson::valueMap(info, fdata, QJsonArray(), parameters);
    auto isi = ReportParameters::find(Type::ISI);
    auto vas = ReportParameters::find(Type::Vas);
    auto hrv = ReportParameters::find(Type::HRV);
    auto map = ReportParameters::find(Type::MAP);
    auto vol = ReportParameters::find(Type::Vol);
    if (position.count() == 2) {   // 双体位
        auto sdata = position.last().toObject().value(ReportDataName::ekey(ReportDataName::data)).toObject();
        auto smap = ReportDataJson::valueMap(info, sdata, QJsonArray(), parameters);
        if (m_pHospitalInfo->pType == Printer_Type::General) {
            // 健康模式
            if (m_pHospitalInfo->cMode == Check_Mode::Health) {
                bool suggest = false;
                bool sport = false;
                bool drink = false;
                int bmi = 0;
                // 灌注
                if (fmap.value(Type::CI) < 2.5) {
                    result<<tr("1.灌注低；");
                    suggest = true;
                }
                else if (fmap.value(Type::CI) >= 2.5 && fmap.value(Type::CI) <= 5) {
                    result<<tr("1.灌注正常；");
                }
                else {
                    if (fmap.value(Type::MAP) < map.value(min).toDouble()) {
                        result<<tr("1.灌注正常，但MAP低；");
                        suggest = true;
                    }
                    else if (fmap.value(Type::MAP) >= map.value(min).toDouble() &&
                             fmap.value(Type::MAP) <= map.value(max).toDouble()) {
                        result<<tr("1.灌注正常；");
                    }
                    else {
                        result<<tr("1.灌注高；");
                        suggest = true;
                    }
                }
                // 前负荷
                if (smap.value(Type::TFC) < fmap.value(Type::TFC)) {
                    if (smap.value(Type::ISI) > fmap.value(Type::ISI)) {
                        result<<tr("2.左右心前负荷均过饱和；");
                        suggest = true;
                    }
                    else {
                        result<<tr("2.右心前负荷过饱和；");
                        sport = true;
                    }
                }
                else {
                    if (smap.value(Type::ISI) > fmap.value(Type::ISI)) {
                        result<<tr("2.前负荷正常；");
                    }
                    else {
                        result<<tr("2.前负荷过饱和；");
                        sport = true;
                    }
                }
                // 后负荷
                if (fmap.value(Type::MAP) >= map.value(min).toDouble() &&
                    fmap.value(Type::MAP) <= map.value(max).toDouble()) {
                    if (fmap.value(Type::Vas) >= vas.value(min).toDouble() &&
                        fmap.value(Type::Vas) <= vas.value(max).toDouble()) {
                        result<<tr("3.后负荷正常；");
                    }
                    else {
                        result<<tr("3.后负荷正常(因血压正常，后负荷有益代偿)；");
                    }
                }
                else if (fmap.value(Type::MAP) > map.value(max).toDouble()) {
                    if (fmap.value(Type::Vas) >= vas.value(min).toDouble() &&
                        fmap.value(Type::Vas) <= vas.value(max).toDouble()) {
                        result<<tr("3.后负荷正常；");
                    }
                    else if (fmap.value(Type::Vas) > vas.value(max).toDouble()) {
                        result<<tr("3.后负荷偏高；");
                    }
                    suggest = true;
                }
                else if (fmap.value(Type::MAP) < map.value(min).toDouble()) {
                    if (fmap.value(Type::Vas) <= vas.value(max).toDouble()) {
                        result<<tr("3.后负荷偏低；");
                    }
                    suggest = true;
                }
                // 心肌收缩力
                auto isiStr = tr("4.心肌收缩力");
                if (fmap.value(Type::ISI) < isi.value(min).toDouble()) {
                    if (smap.value(Type::ISI) >= isi.value(min).toDouble() &&
                        smap.value(Type::ISI) <= isi.value(max).toDouble()) {
                        drink = true;
                    }
                    else {
                        suggest = true;
                    }
                    isiStr += tr("偏低");
                }
                else if (fmap.value(Type::ISI) >= isi.value(min).toDouble() &&
                         fmap.value(Type::ISI) <= isi.value(max).toDouble()) {
                    isiStr += tr("正常");
                }
                else {
                    if (fmap.value(Type::MAP) > map.value(max).toDouble()) {
                        isiStr += tr("偏高");
                        suggest = true;
                    }
                    else {
                        isiStr += tr("正常");
                    }
                }
                if ((fmap.value(Type::ISI) - smap.value(Type::ISI)) / fmap.value(Type::ISI) >= 0.25) {
                    isiStr += tr("（左心室收缩功能储备下降）");
                }
                result<<(isiStr + "；");
                // BMI
                if (fmap.value(Type::BMI) < 18.5) {
                    result<<tr("5.BMI体型过瘦；");
                    bmi = 1;
                }
                else if (fmap.value(Type::BMI) > 24) {
                    result<<tr("5.BMI体型过胖；");
                    bmi = 2;
                }
                else {
                    result<<tr("5.BMI体型正常；");
                }
                if (suggest || sport || bmi != 0) {
                    QString space("           ");
                    result<<tr("\n小结：血流动力学异常；");
                    if (sport) {
                        result<<tr("%1减少钠摄入、多排汗、多喝水，根据自身情况增加运动量；").arg(space);
                    }
                    if (drink && !sport) {
                        result<<tr("%1建议多喝水；").arg(space);
                    }
                    if (bmi != 0 && sport) {
                        result<<tr("%1建议改善饮食、生活习惯；").arg(space);
                    }
                    else if (bmi == 2 && !sport) {
                        result<<tr("%1建议改善饮食、生活习惯，根据自身情况增加运动量；").arg(space);
                    }
                    auto sv = ReportParameters::find(Type::SV);
                    if (fmap.value(Type::SV) >= sv.value(min).toDouble() &&
                        fmap.value(Type::SV) <= sv.value(max).toDouble() &&
                        fmap.value(Type::CI) > 3.0 && fmap.value(Type::HR) > 90 && fmap.value(Type::HR) < 100) {
                        if (sport || bmi == 2) {
                            result<<tr("%1调整作息、睡眠时间；").arg(space);
                        }
                        else {
                            result<<tr("%1适当运动，调整作息、睡眠时间；").arg(space);
                        }
                    }
                    if (fmap.value(Type::TFC) > 0.046) {
                        if (suggest) {
                            result<<tr("%1请关注胸腔液体量过多；").arg(space);
                        }
                        else {
                            result<<tr("%1请关注胸腔液体量过多，建议去专科医院做进一步检查；").arg(space);
                        }
                    }
                    if (suggest) {
                        result<<tr("%1建议去专科医院做进一步检查。").arg(space);
                    }
                }
                else {
                    result<<tr("\n小结：血流动力学正常。");
                }
                return result.join("\n");
            }

            // 双体位常规打印机报告
            QString fstr = tr("前负荷(容量负荷)：");
            if (smap.value(Type::ISI) > fmap.value(Type::ISI)) {
                if (smap.value(Type::ISI) / fmap.value(Type::ISI) >= 1.1) {
                    if (smap.value(Type::TFC) < fmap.value(Type::TFC)) {
                        fstr += tr("正常(增加容量后，TFC下降，请关注容量)；");
                    }
                    else {
                        fstr += tr("正常；");
                    }
                }
                else {
                    fstr += tr("容量平台期(请结合临床)；");
                }
            }
            else {
                fstr += tr("偏高(建议使用：减少钠盐摄入，使用利尿剂)，请结合临床分析；");
            }
            QString tstr = tr("后负荷(张力负荷)：");
            if (fmap.value(Type::MAP) >= map.value(min).toDouble() && fmap.value(Type::MAP) <= map.value(max).toDouble()) {
                if (fmap.value(Type::Vas) >= vas.value(min).toDouble() &&
                        fmap.value(Type::Vas) <= vas.value(max).toDouble()) {
                    tstr += tr("正常；");
                }
                else {
                    tstr += tr("正常(因血压正常，后负荷有益代偿)；");
                }
            }
            else if (fmap.value(Type::MAP) > map.value(max).toDouble()) {
                if (fmap.value(Type::Vas) >= vas.value(min).toDouble() &&
                        fmap.value(Type::Vas) <= vas.value(max).toDouble()) {
                    tstr += tr("正常；");
                }
                else if (fmap.value(Type::Vas) > vas.value(max).toDouble()) {
                    tstr += tr("偏高(建议使用：ACEI，ARB)，请结合临床分析；");
                }
            }
            else if (fmap.value(Type::MAP) < map.value(min).toDouble()) {
                if (fmap.value(Type::Vas) <= vas.value(max).toDouble()) {
                    tstr += tr("偏低；");
                }
            }
            QString sstr = tr("心肌力(心脏泵力)：");
            if (fmap.value(Type::ISI) < isi.value(min).toDouble()) {
                sstr += tr("偏低；");
            }
            else if (fmap.value(Type::ISI) > isi.value(max).toDouble()) {
                if (fmap.value(Type::MAP) > map.value(max).toDouble()) {
                    sstr += tr("偏高(建议使用：β阻滞剂类，CCB)，请结合临床分析；");
                }
                else {
                    sstr += tr("偏高；");
                }
            }
            else {
                sstr += tr("正常；");
            }
            QString hstr = tr("心率变异性（交感神经兴奋度）：");
            if (fmap.value(Type::HRV) < hrv.value(min).toDouble()) {
                hstr += tr("偏低；");
            }
            else if (fmap.value(Type::HRV) > hrv.value(max).toDouble()) {
                hstr += tr("偏高(建议使用：β阻滞剂，镇静剂类)，请结合临床分析；");
            }
            else {
                hstr += tr("正常；");
            }

            if (m_pHospitalInfo->cMode == Check_Mode::Hypertension) {
                // 高血压模式
                result<<fstr<<tstr<<sstr<<hstr;
            }
            else if (m_pHospitalInfo->cMode == Check_Mode::InternalMedicine) {
                // 内科模式
                result<<tr("第一体位：心输出量(CO)%1，心脏指数(CI)%2，搏排量(SV)%3，心搏指数(SI)%4，"
                            "心率(HR)%5，血管顺应性(Vas)%6，血管容量(Vol)%7，收缩变力性(Ino)%8，"
                            "收缩压(SBP)%9，舒张压(DBP)%10，胸液传导性(TFC)%11；")
                        .arg(pevl(Type::CO), pevl(Type::CI), pevl(Type::SV), pevl(Type::SI), pevl(Type::HR),
                             pevl(Type::Vas), pevl(Type::Vol), pevl(Type::Ino), pevl(Type::SBP))
                        .arg(pevl(Type::DBP), pevl(Type::TFC));
                result<<tr("第二体位增加容量负荷实验后：搏排量(SV)%1，变力状态指数(ISI)%2，%3；")
                        .arg(compare(Type::SV), compare(Type::ISI), preload());
                result<<tr("%1%2%3%4").arg(fstr, tstr, sstr, hstr);
            }
            else if (m_pHospitalInfo->cMode == Check_Mode::IntensiveCareUnit) {
                // 重症模式
                result<<tr("第一体位：心输出量(CO)%1，心脏指数(CI)%2，搏排量(SV)%3，心搏指数(SI)%4，"
                            "心率(HR)%5，血管顺应性(Vas)%6，血管容量(Vol)%7，收缩变力性(Ino)%8，"
                            "收缩压(SBP)%9，舒张压(DBP)%10，胸液传导性(TFC)%11；")
                        .arg(pevl(Type::CO), pevl(Type::CI), pevl(Type::SV), pevl(Type::SI), pevl(Type::HR),
                             pevl(Type::Vas), pevl(Type::Vol), pevl(Type::Ino), pevl(Type::SBP))
                        .arg(pevl(Type::DBP), pevl(Type::TFC));
                result<<tr("第二体位增加容量负荷实验后：搏排量(SV)%1，变力状态指数(ISI)%2，%3；")
                        .arg(compare(Type::SV), compare(Type::ISI), preload());
                result<<tr("%1%2%3%4").arg(fstr, tstr, sstr, hstr);
            }
            else if (m_pHospitalInfo->cMode == Check_Mode::PhysicalExamination) {
                // 体检模式
                result<<fstr<<tstr<<sstr<<hstr;
            }
        }
        else if (m_pHospitalInfo->pType == Printer_Type::Thermal) {
            //双体位热敏打印机报告
            result<<tr("无创血流动力学检测系统评价，心脏动力，血管阻力，血液容量，血压等循环系统情况结论如下：\n"
                        "1.第一体位：CO%1，CI%2，SV%3，SI%4，HR%5，Vas%6，Vol%7，Ino%8，SBP%9，DBP%10，TFC%11；\n"
                        "2.第二体位增加容量负荷实验后：SV%12，ISI%13，%14。")
                    .arg(pevl(Type::CO), pevl(Type::CI), pevl(Type::SV), pevl(Type::SI), pevl(Type::HR),
                         pevl(Type::Vas), pevl(Type::Vol), pevl(Type::Ino), pevl(Type::SBP))
                    .arg(pevl(Type::DBP), pevl(Type::TFC), compare(Type::SV), compare(Type::ISI), preload());
        }
    }
    else {  // 单体位
        QString fstr = tr("前负荷(容量负荷)：");
        if (fmap.value(Type::Vol) < vol.value(min).toDouble()) {
            fstr += tr("偏低；");
        }
        else if (fmap.value(Type::Vol) > vol.value(max).toDouble()) {
            if (fmap.value(Type::MAP) > map.value(max).toDouble()) {
                fstr += tr("偏高(建议使用：减少钠盐摄入，使用利尿剂)，请结合临床分析；");
            }
            else {
                fstr += tr("偏高；");
            }
        }
        else {
            fstr += tr("正常；");
        }
        QString tstr = tr("后负荷(张力负荷)：");
        if (fmap.value(Type::Vas) < vas.value(min).toDouble()) {
            tstr += tr("偏低；");
        }
        else if (fmap.value(Type::Vas) > vas.value(max).toDouble()) {
            tstr += tr("偏高(建议使用：ACEI，ARB)，请结合临床分析；");
        }
        else {
            tstr += tr("正常；");
        }
        QString sstr = tr("心肌力(心脏泵力)：");
        if (fmap.value(Type::ISI) < isi.value(min).toDouble()) {
            sstr += tr("偏低；");
        }
        else if (fmap.value(Type::ISI) > isi.value(max).toDouble()) {
            if (fmap.value(Type::MAP) > map.value(max).toDouble()) {
                sstr += tr("偏高(建议使用：β阻滞剂类，CCB)，请结合临床分析；");
            }
            else {
                sstr += tr("偏高；");
            }
        }
        else {
            sstr += tr("正常；");
        }
        QString hstr = tr("心率变异性（交感神经兴奋度）：");
        if (fmap.value(Type::HRV) < hrv.value(min).toDouble()) {
            hstr += tr("偏低；");
        }
        else if (fmap.value(Type::HRV) > hrv.value(max).toDouble()) {
            hstr += tr("偏高(建议使用：β阻滞剂，镇静剂类)，请结合临床分析；");
        }
        else {
            hstr += tr("正常；");
        }
        if (fmap.value(Type::MAP) < map.value(min).toDouble() && fmap.value(Type::Vas) > vas.value(max).toDouble()) {
            tstr += tr("(提示：病人血压低，后负荷被动性代偿)；");
        }
        result<<fstr<<tstr<<sstr<<hstr;
        if (m_pHospitalInfo->pType == Printer_Type::General) {
            // 单体位常规打印机报告
            // 体检模式
        }
        else if (m_pHospitalInfo->pType == Printer_Type::Thermal) {
            // 单体位热敏打印机报告
            // 体检模式
            result.clear();
            result<<tr("无创血流动力学检测系统评价，心脏动力，血管阻力，血液容量，血压等循环系统情况结论如下：\n"
                        "1.心脏功能：CO%1，SV%2，SI%3，CI%4，Vol%5，Vas%6，Ino%7；\n"
                        "2.血压管理：SBP%8，DBP%9，HR%10；")
                    .arg(pevl(Type::CO, false), pevl(Type::SV, false), pevl(Type::SI, false),
                         pevl(Type::CI, false), pevl(Type::Vol, false), pevl(Type::Vas, false),
                         pevl(Type::Ino, false), pevl(Type::SBP, false), pevl(Type::DBP, false))
                    .arg(pevl(Type::HR, false));
        }
    }
    return result.join("|");
}

void DataManagement::clearSlot()
{
    //断开数据传输
    m_pTebco->startCheckSlot(false);
    m_pTebco->clearSerial();
}

// void DataManagement::customCtrlTimer(bool start)
// {
//     auto hrCtrl = m_pRegulator->getCustomCtrl(Type::HR);
//     auto svCtrl = m_pRegulator->getCustomCtrl(Type::SV);
//     auto isiCtrl = m_pRegulator->getCustomCtrl(Type::ISI);
//     if (hrCtrl == nullptr || svCtrl == nullptr || isiCtrl == nullptr)
//         return;
//     if (start) {
//         hrCtrl->startTimer(1);
//         svCtrl->startTimer(10);
//         isiCtrl->startTimer(100);
//     }
//     else {
//         hrCtrl->stopTimer();
//         svCtrl->stopTimer();
//         isiCtrl->stopTimer();
//     }
// }

QString DataManagement::flag(CustomCtrl *customCtrl, bool second)
{
    QVector<Type> fvect { Type::CO, Type::CI, Type::SV, Type::SI, Type::TFC, Type::EPCI, Type::ISI, Type::Ino, Type::HR, Type::MAP };
    QVector<Type> svect { Type::Vol, Type::HRV, Type::Vas };
    if (!second) {  // 第一体位
        if (fvect.cend() != std::find(fvect.cbegin(), fvect.cend(), customCtrl->getType())) {
            if (customCtrl->getCurrentValue() != 0) {
                if ((customCtrl->getType() == Type::Vol || customCtrl->getType() == Type::Vas) &&
                        m_pRegulator->getCustomCtrl(Type::MAP)->getCurrentValue() == 0) {   // Vol或Vas参数时MAP为零
                    return QString("-");
                }
                else {
                    return tip(customCtrl->getMinValue(), customCtrl->getMaxValue(), customCtrl->getCurrentValue());
                }
            }
        }
    }
    else {  // 第二体位
        if (svect.cend() != std::find(svect.cbegin(), svect.cend(), customCtrl->getType())) {
            if (customCtrl->getCurrentValue() != 0) {
                if (customCtrl->getRecordValue() != 0) {
                    return tip(customCtrl->getRecordValue(), customCtrl->getCurrentValue());
                }
            }
            else {
                if ((customCtrl->getType() == Type::Vol || customCtrl->getType() == Type::Vas) &&
                    m_pRegulator->getCustomCtrl(Type::MAP)->getCurrentValue() != 0) {
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

QString DataManagement::pevl(const Type &type, bool many)
{
    CustomCtrl *temp = m_pRegulator->getCustomCtrl(type);
    if (type == Type::SBP || type == Type::DBP) {
        temp = m_pRegulator->getCustomCtrl(Type::SBP);
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
    auto temp = m_pRegulator->getCustomCtrl(type);
    if (temp == nullptr) return nullptr;
    if (temp->getRecordValue() > temp->getCurrentValue()) return tr("下降");
    else if (temp->getRecordValue() < temp->getCurrentValue()) return tr("上升");
    return tr("不变");
}

QString DataManagement::preload()
{
    auto svCtrl = m_pRegulator->getCustomCtrl(Type::SV);
    auto isiCtrl = m_pRegulator->getCustomCtrl(Type::ISI);
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
    auto co = m_pRegulator->getCustomCtrl(Type::CO);
    auto isi = m_pRegulator->getCustomCtrl(Type::ISI);
    auto hr = m_pRegulator->getCustomCtrl(Type::HR);
    auto sv = m_pRegulator->getCustomCtrl(Type::SV);
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
