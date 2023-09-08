#ifndef REPORTDATAJSON_H
#define REPORTDATAJSON_H

#include <QObject>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QMap>
#include "reportset.h"
#include "customctrl.h"

class ReportDataJson : public QObject
{
    Q_OBJECT
public:
    explicit ReportDataJson(QObject *parent = nullptr);
    QJsonObject getJson();
    QString getJsonString();
    QString getReportTime();
    static QMap<Type, qreal> valueMap(const QJsonObject &info, const QJsonObject &data, const QJsonArray &alldata);
    static QVector<qreal> allValue(Type type, const QJsonArray &alldata);
public slots:
    // 清空
    void clear();
    // 患者信息
    void setPatientInfo(int age, float hb, int height, int weight, int sex,
                        const QString &patientName, const QString &medicalRecordNumber);
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
private:
    QJsonObject json;
    QJsonObject data;
    QJsonArray allData;
    QJsonArray waveform;
};

#endif // REPORTDATAJSON_H
