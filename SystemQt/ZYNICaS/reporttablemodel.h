#ifndef REPORTTABLEMODEL_H
#define REPORTTABLEMODEL_H

#include <QAbstractTableModel>
#include <QObject>
#include <QJsonObject>
#include <QJsonArray>
#include <QDateTime>


struct ReportModelItem
{
    ReportModelItem(const qint64 &_time, const int &_modified, const QJsonObject &data);
    qint64 time;
    QString medicalRecordNumber;
    QString patientName;
    QString sex;
    QString age;
    QString height;
    QString weight;
    QString mode;
    bool modified;
};

class ReportTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit ReportTableModel(QObject *parent = nullptr);

    virtual int rowCount(const QModelIndex & parent = QModelIndex()) const;

    virtual int columnCount(const QModelIndex & parent = QModelIndex()) const;

    virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    void updateList(const QVector<ReportModelItem> &list);

    void filterTime(const qint64 &start, const qint64 &end);

    void filterMedicalRecordNumber(const QString &medicalRecordNumber);

    void filterPatientName(const QString &patientName);
private:
    QVector<ReportModelItem> m_list;
    QStringList m_header;
};

#endif // REPORTTABLEMODEL_H
