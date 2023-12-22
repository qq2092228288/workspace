#ifndef DATATABLEMODEL_H
#define DATATABLEMODEL_H

#include <QAbstractTableModel>
#include <QDateTime>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QMap>

#include "datatype.h"

struct ExeclData
{
    ExeclData(const QString &id, const QJsonObject &place, const QJsonObject &patientInfo,
              const QString &tstr, const QJsonObject &pdata, bool _first);
    bool first;
    QDateTime time;
    QString deviceId, primaryPlace, secondaryPlace;
    QString sex;
    int age, height, weight;
    double bsa;
    int pos;
    QMap<Type, qreal> map;
};

class DataTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit DataTableModel(const QStringList &header, QObject *parent = nullptr);

    virtual int rowCount(const QModelIndex & parent = QModelIndex()) const;

    virtual int columnCount(const QModelIndex & parent = QModelIndex()) const;

    virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
public:
    void setHeader(const QStringList &header);
    void resetList();
    void update();
    QStringList header() const;
    static QString positionString(bool first, int pos);
signals:
private:
    QStringList m_header;
    QList<ExeclData> m_list;
};

#endif // DATATABLEMODEL_H
