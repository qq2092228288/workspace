#ifndef REPORTDATAJSON_H
#define REPORTDATAJSON_H

#include <QObject>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QMap>

#include "datatype.h"

class ReportDataJson : public QObject
{
    Q_OBJECT
public:
    explicit ReportDataJson(QObject *parent = nullptr);
    QJsonObject getJson(const QString &hospitalName, const QString &roomName,  const QString &doctorName,
                        const QString &consultationHospitalName, const QString &mac);
    QJsonObject getJson();
    QString getReportTime();
    static QMap<Type, qreal> valueMap(const QJsonObject &info, const QJsonObject &data, const QJsonArray &alldata,
                                      const QJsonArray &reportStruct);
    static QVector<qreal> allValue(Type type, const QJsonArray &alldata);
public slots:
    // 清空
    void clear();
    void clearCheckData();
    // 患者信息
    void setPatientInfo(int age, float hb, int height, int weight, int sex,
                        const QString &patientName, const QString &medicalRecordNumber,
                        int fhh, int edh, int ltsh, int lthms, int ptm, int al);
    void setPatientInfo(const QString &name, const QString &id, int gender, int age, int height, int weight);
    // 新增数据
    void appendPosition(int sbp, int dbp, int cvp, int lap, int pos);
    // 报告结论
    void setReportConclusion(const QString &reportConclusion);
    // 新增检测数据
    void appendData(uchar type, short value);
    // 新增波形数据
    void appendWaveformData(const QByteArray &data);
private:
    QJsonArray getPosition();
    QString time() const;
    static qreal intercept(qreal value, int digit);
private:
    QJsonObject json;
    QJsonObject data;
    QJsonObject tempData;
    QJsonArray allData;
    QJsonArray waveform;
};

#endif // REPORTDATAJSON_H
