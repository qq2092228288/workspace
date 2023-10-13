#ifndef REPORTPAINTER_H
#define REPORTPAINTER_H

#include <QPainter>
#include <QPrinter>
#include <QObject>
#include <QJsonObject>

#include "reportset.h"
#include "customctrl.h"
#include "reportdatajson.h"

struct ReportStruct
{
    ReportStruct(Printer_Type type, Check_Mode mode, bool paging, QJsonObject data)
        : type(type),mode(mode),paging(paging),data(data) {}
    Printer_Type type;
    Check_Mode mode;
    bool paging; // 分页
    QJsonObject data;
};

typedef QVector<QMap<Type, qreal>> ValueMaps;
struct TrendChartsData
{
    TrendChartsData(const QJsonObject &object)
    {
        startTime = object.value(ekey(ReportDataName::startTime)).toString();
        auto patientInfo = object.value(ekey(ReportDataName::patientInfo)).toObject();
        auto position = object.value(ekey(ReportDataName::position)).toArray();
        foreach (auto pos, position) {
            auto index = pos.toObject();
            endTime = index.value(ekey(ReportDataName::reportTime)).toString();
            auto allData = index.value(ekey(ReportDataName::allData)).toArray();
            auto dataMap = index.value(ekey(ReportDataName::data)).toObject().toVariantMap();
            ValueMaps maps;
            foreach (auto cdata, allData) {
                dataMap.insert(cdata.toObject().toVariantMap());
                maps<<ReportDataJson::valueMap(patientInfo, QJsonObject::fromVariantMap(dataMap), QJsonArray());
            }
            mapsVector<<maps;
        }
    }
    QVector<QVector<qreal>> alldata(Type type)
    {
        QVector<QVector<qreal>> datas;
        foreach (auto maps, mapsVector) {
            QVector<qreal> data;
            foreach (auto map, maps) {
                data<<map.value(type);
            }
            datas<<data;
        }
        return datas;
    }
    template <class T>
    QString ekey(const T &t)
    {
        return ReportDataName::ekey(t);
    }
    QString startTime;
    QString endTime;
    QVector<ValueMaps> mapsVector;
};

class ReportPainter : public QPainter
{
public:
    ReportPainter(const ReportStruct &info, QPrinter *printer);
    struct ReportTableSet
    {
        ReportTableSet(bool paging) : paging(paging) {}
        QPointF start()
        {
            return QPointF(30, 170);
        }
        qreal rowHeight(Check_Mode mode)
        {
            switch (mode) {
            case Check_Mode::Hypertension:
                return 35;
            case Check_Mode::InternalMedicine:
                return 22;
            case Check_Mode::IntensiveCareUnit:
                return 22;
            case Check_Mode::PhysicalExamination:
                return 22;
            }
            return -1;
        }
        qreal groupWidth()
        {
            return 20;
        }
        QVector<qreal> colWidth()
        {
            if (paging) {
                return QVector<qreal>()<<170<<70<<100<<130;
            }
            return QVector<qreal>()<<160<<55<<55<<85<<115;
        }
        bool paging;
    };
private:
    void thermalPage();
    void generalHeader(const QString &title);
    void generalDataPage(int page = 0);
    void consultationPage(const QJsonObject &consultation);
    void trendChartPage(QVector<Type> types);
    void plrPage();
    void generalFooter();
    void setGeneralHeaderAndFooter();

    QString getInquiry(int value);
    void setFontSize(int pointSize, bool bold = false);
    QRectF drawTableCell(QRectF rect, const QString &txt, int align = Qt::AlignCenter, int offset = 0);
    QMap<Type, qreal> valueMap(const QJsonObject &data, const QJsonArray &alldata);
    void drawValue(const QJsonObject &parameter, QMap<Type, qreal> map, QRectF rect, qreal _y);
    QString positionCn(int pos);
    void drawWaveform(QRectF rect, int pos, const QJsonArray &waveform, bool index);
    void drawSudoku(QRectF rect, QMap<Type, qreal> fMap, QMap<Type, qreal> sMap = QMap<Type, qreal>());
    void drawIsiAndSv(QRectF rect, qreal fIsi, qreal fSv, qreal sIsi, qreal sSv);
    void drawArrow(QPointF start, QPointF end);
    void drawCurve(QList<QPointF> points);
    void drawTrendChart(QRectF rect, Type type, QString stime, QString etime,
                        QVector<qreal> fvect, QVector<qreal> svect = QVector<qreal>());
    QPointF intersection(qreal ssvri, qreal lswi, qreal cvp, qreal lap);
    void positionSymbol(int pos, QPoint point, QColor color);
    void positionText(int pos, QPoint point, QColor color);
    qreal actualValue(qreal value, int digit);
    qreal limits(qreal actual, qreal min, qreal max);
    QString compare(qreal v1, qreal v2);
    qreal circleX(qreal y, qreal r, qreal ox, qreal oy, bool negative);
    QRectF rectF(qreal x, qreal y);
    QString getPicFileName(int pos);
    QString timeStyle(const QString &otime);
    template <class T>
    QString ekey(const T &t);
private:
    ReportStruct m_info;
    /** General paper size 793x1123 pixels, thermal printer paper size 272x1123 pixels. */
    const QSizeF m_size;
private:
    QPrinter *m_printer;
};

#endif // REPORTPAINTER_H
