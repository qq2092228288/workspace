#include "datatablemodel.h"
#include "reportdataname.h"
#include "reportdatajson.h"
#include "reportset.h"
#include "tcpclientsocket.h"
#include "singleton.h"
#include "databasens.h"
#include "datacalculation.h"

using namespace DatabaseEnumNs;

const auto REPORT_TIME_STR              = ReportDataName::ekey(ReportDataName::reportTime);
const auto PRIMARY_PLACE_STR            = ReportDataName::ekey(ReportDataName::primaryPlace);
const auto SECONDARY_PLACE_STR          = ReportDataName::ekey(ReportDataName::secondaryPlace);
const auto PATIENT_NAME_STR             = ReportDataName::ekey(ReportDataName::patientName);
const auto MEDICAL_RECORD_NUMBER_STR    = ReportDataName::ekey(ReportDataName::medicalRecordNumber);
const auto SEX_STR                      = ReportDataName::ekey(ReportDataName::sex);
const auto AGE_STR                      = ReportDataName::ekey(ReportDataName::age);
const auto HEIGHT_STR                   = ReportDataName::ekey(ReportDataName::height);
const auto WEIGHT_STR                   = ReportDataName::ekey(ReportDataName::weight);
const auto DATA_STR                     = ReportDataName::ekey(ReportDataName::data);
const auto PLACE_STR                    = ReportDataName::ekey(ReportDataName::place);
const auto PATIENT_INFO_STR             = ReportDataName::ekey(ReportDataName::patientInfo);
const auto POSITION_STR                 = ReportDataName::ekey(ReportDataName::position);

bool myfunction (ExeclData i, ExeclData j)
{
    if (i.medicalRecordNumber == j.medicalRecordNumber) {
        return i.time > j.time;
    }
    return i.medicalRecordNumber > j.medicalRecordNumber;
}

ExeclData::ExeclData(const QString &id, const QJsonObject &place, const QJsonObject &patientInfo,
                     const QString &tstr, const QJsonObject &pdata, const QJsonArray &parameters, bool _first)
    : first(_first)
{
    time = QDateTime::fromString(tstr, "yyyyMMddhhmmsszzz");
    if (!time.isValid()) {
        time = QDateTime::fromString(tstr, "yyyyMMddhhmmss");
    }
    deviceId = id;
    primaryPlace = place.value(PRIMARY_PLACE_STR).toString();
    secondaryPlace = place.value(SECONDARY_PLACE_STR).toString();
    patientName = patientInfo.value(PATIENT_NAME_STR).toString().trimmed();
    medicalRecordNumber = patientInfo.value(MEDICAL_RECORD_NUMBER_STR).toString().trimmed();
    sex = patientInfo.value(SEX_STR).toString();
    age = patientInfo.value(AGE_STR).toString().toInt();
    height = patientInfo.value(HEIGHT_STR).toString().toInt();
    weight = patientInfo.value(WEIGHT_STR).toString().toInt();
    bsa = (static_cast<int>(DatCa::cBsa(height, weight) * 100)) / 100.0;
    pos = pdata.value(QString::number(Type::Pos)).toInt();
    map = ReportDataJson::valueMap(patientInfo, pdata, QJsonArray(), parameters);
}

DataTableModel::DataTableModel(const QStringList &header, QObject *parent)
    : QAbstractTableModel{parent}
    , m_header{header}
{

}

int DataTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_list.count();
}

int DataTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_header.count();
}

QVariant DataTableModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid()) {
        return QVariant();
    }
    int row = index.row();
    if (row < m_list.count()) {
        if (Qt::DisplayRole == role) {
            auto crow = m_list.at(row);
            switch (index.column()) {
            case 0:
                return crow.deviceId;
            case 1:
                return crow.primaryPlace;
            case 2:
                return crow.secondaryPlace;
            case 3:
                return crow.medicalRecordNumber;
            case 4:
                return QString("%1**").arg(crow.patientName.at(0));
            case 5:
                return crow.time;
            case 6:
                return positionString(crow.first, crow.pos);
            case 7:
                return crow.sex;
            case 8:
                return crow.age;
            case 9:
                return crow.height;
            case 10:
                return crow.weight;
            case 11:
                return crow.bsa;
            case 12:
                return nicasData(crow.map.value(Type::CO));
            case 13:
                return nicasData(crow.map.value(Type::CI));
            case 14:
                return nicasData(crow.map.value(Type::SV));
            case 15:
                return nicasData(crow.map.value(Type::SI));
            case 16:
                return nicasData(crow.map.value(Type::TFC));
            case 17:
                return nicasData(crow.map.value(Type::EDI));
            case 18:
                return nicasData(crow.map.value(Type::Vol));
            case 19:
                return nicasData(crow.map.value(Type::SVR));
            case 20:
                return nicasData(crow.map.value(Type::SSVR));
            case 21:
                return nicasData(crow.map.value(Type::SSVRI));
            case 22:
                return nicasData(crow.map.value(Type::SVRI));
            case 23:
                return nicasData(crow.map.value(Type::Vas));
            case 24:
                return nicasData(crow.map.value(Type::PEP));
            case 25:
                return nicasData(crow.map.value(Type::LVET));
            case 26:
                return nicasData(crow.map.value(Type::LSW));
            case 27:
                return nicasData(crow.map.value(Type::LSWI));
            case 28:
                return nicasData(crow.map.value(Type::LCW));
            case 29:
                return nicasData(crow.map.value(Type::LCWI));
            case 30:
                return nicasData(crow.map.value(Type::STR));
            case 31:
                return nicasData(crow.map.value(Type::EPCI));
            case 32:
                return nicasData(crow.map.value(Type::ISI));
            case 33:
                return nicasData(crow.map.value(Type::Ino));
            case 34:
                return nicasData(crow.map.value(Type::HR));
            case 35:
                return nicasData(crow.map.value(Type::HRV));
            case 36:
                return nonZeroData(crow.map.value(Type::SBP));
            case 37:
                return nonZeroData(crow.map.value(Type::DBP));
            case 38:
                return nonZeroData(crow.map.value(Type::MAP));
            }
        }
        else {
            return QVariant();
        }
        return QVariant();
    }
    return QVariant();
}

QVariant DataTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation == Qt::Horizontal) {    //水平表头
        if(Qt::DisplayRole == role && section < m_header.count()) {
            return m_header[section];
        }
    }
    return QAbstractTableModel::headerData(section, orientation, role);
}

void DataTableModel::setHeader(const QStringList &header)
{
    m_header = header;
}

void DataTableModel::resetList()
{
    m_list.clear();
    auto parameters = ReportParameters::array(Check_Mode::IntensiveCareUnit);
    QSqlQuery query(TcpClientSocket::getInstance()->db());
    query.exec(QString("SELECT %1, %2 FROM %3 ORDER BY %1 DESC")
                   .arg(Singleton::enumValueToKey(ReportInfo::deviceId),
                        Singleton::enumValueToKey(ReportInfo::reportData),
                        Singleton::enumName<ReportInfo>()));
    while (query.next()) {
        auto deviceId = query.value(0).toString();
        auto reportData = QJsonDocument::fromJson(query.value(1).toString().toUtf8()).object();
        auto place = reportData.value(PLACE_STR).toObject();
        auto patientInfo = reportData.value(PATIENT_INFO_STR).toObject();
        auto position = reportData.value(POSITION_STR).toArray();
        auto ststr = position.at(1).toObject().value(REPORT_TIME_STR).toString();
        if (!ststr.isEmpty()) {
            m_list.append(ExeclData(deviceId,
                                    place,
                                    patientInfo,
                                    ststr,
                                    position.at(1).toObject().value(DATA_STR).toObject(),
                                    parameters,
                                    false));
        }
        m_list.append(ExeclData(deviceId, place, patientInfo,
                                position.at(0).toObject().value(REPORT_TIME_STR).toString(),
                                position.at(0).toObject().value(DATA_STR).toObject(),
                                parameters,
                                true));
    }
    std::sort(m_list.begin(), m_list.end(), myfunction);
    update();
}

void DataTableModel::update()
{
    beginResetModel();
    endResetModel();
}

QStringList DataTableModel::header() const
{
    return m_header;
}

QList<ExeclData> DataTableModel::list() const
{
    return m_list;
}

QVariant DataTableModel::nicasData(const double &value)
{
    return DatCa::invalid() != value ? value : QVariant("-");
}

QVariant DataTableModel::nonZeroData(const double &value)
{
    if (value == 0) {
        return QVariant("-");
    }
    return nicasData(value);
}

QString DataTableModel::positionString(bool first, int pos)
{
    auto str = first ? QString::fromUtf8("第一体位: ") : QString::fromUtf8("第二体位: ");
    switch (pos) {
    case 1:
        str += QString::fromUtf8("半卧");
        break;
    case 2:
        str += QString::fromUtf8("平躺");
        break;
    case 3:
        str += QString::fromUtf8("抬腿");
        break;
    default:
        break;
    }
    return str;
}

QString DataTableModel::privacy(const QString &patientName, const QString &medicalRecordNumber)
{
    return QString("%1%2&%3**").arg(patientName.at(0),
                                  QString(patientName.length() - 1, '*'),
                                  medicalRecordNumber.mid(0, medicalRecordNumber.length() - 2));
}
