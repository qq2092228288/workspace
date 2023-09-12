#include "reporttablemodel.h"
#include "reportset.h"

ReportModelItem::ReportModelItem(const qint64 &_time, const QJsonObject &data)
    : time(_time)
{
    auto patientInfo = data.value(ReportDataName::ekey(ReportDataName::patientInfo)).toObject();
    medicalRecordNumber = patientInfo.value(ReportDataName::ekey(ReportDataName::medicalRecordNumber)).toString();
    patientName = patientInfo.value(ReportDataName::ekey(ReportDataName::patientName)).toString();
    sex = patientInfo.value(ReportDataName::ekey(ReportDataName::sex)).toString();
    age = patientInfo.value(ReportDataName::ekey(ReportDataName::age)).toString();
    height = patientInfo.value(ReportDataName::ekey(ReportDataName::height)).toString();
    weight = patientInfo.value(ReportDataName::ekey(ReportDataName::weight)).toString();
    mode = data.value(ReportDataName::ekey(ReportDataName::position)).toArray().at(1).toObject()
            .value(ReportDataName::ekey(ReportDataName::reportTime)).toString().isEmpty() ? "单" : "双";
}

ReportTableModel::ReportTableModel(QObject *parent)
    : QAbstractTableModel{parent}
{
    m_header<<"报告时间"<<"病历号"<<"姓名"<<"性别"<<"年龄"<<"身高"<<"体重"<<"体位模式";
}

int ReportTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_list.count();
}

int ReportTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_header.count();
}

QVariant ReportTableModel::data(const QModelIndex &index, int role) const
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
                return QDateTime::fromMSecsSinceEpoch(crow.time);
            case 1:
                return crow.medicalRecordNumber;
            case 2:
                return crow.patientName;
            case 3:
                return crow.sex;
            case 4:
                return crow.age;
            case 5:
                return crow.height;
            case 6:
                return crow.weight;
            case 7:
                return crow.mode;
            }
        }
        return QVariant();
    }
    return QVariant();
}

QVariant ReportTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation == Qt::Horizontal) {    //水平表头
        if(Qt::DisplayRole == role && section < m_header.count()) {
            return m_header[section];
        }
    }
    else if(orientation == Qt::Vertical) {
        //操作差不多，返回垂直表头，如果需要的话
    }
    return QAbstractTableModel::headerData(section, orientation, role);
}

void ReportTableModel::updateList(const QVector<ReportModelItem> &list)
{
    beginResetModel();
    m_list = list;
    endResetModel();
}

void ReportTableModel::filterTime(const qint64 &start, const qint64 &end)
{
    QVector<ReportModelItem> list;
    foreach (auto value, m_list) {
        if (value.time >= start && value.time <= end) {
            list.append(value);
        }
    }
    updateList(list);
}

void ReportTableModel::filterMedicalRecordNumber(const QString &medicalRecordNumber)
{
    QVector<ReportModelItem> list;
    foreach (auto value, m_list) {
        if (medicalRecordNumber == value.medicalRecordNumber) {
            list.append(value);
        }
    }
    updateList(list);
}

void ReportTableModel::filterPatientName(const QString &patientName)
{
    QVector<ReportModelItem> list;
    foreach (auto value, m_list) {
        if (patientName == value.patientName) {
            list.append(value);
        }
    }
    updateList(list);
}

