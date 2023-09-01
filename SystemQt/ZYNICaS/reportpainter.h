#ifndef REPORTPAINTER_H
#define REPORTPAINTER_H

#include <QPainter>
#include <QPrinter>
#include <QObject>
#include <QJsonObject>

#include "reportset.h"
#include "customctrl.h"

struct ReportInfo
{
    ReportInfo(Printer_Type type, Check_Mode mode, bool paging, QJsonObject data)
        : type(type),mode(mode),paging(paging),data(data) {}
    Printer_Type type;
    Check_Mode mode;
    bool paging; // 分页
    QJsonObject data;
};

class ReportPainter : public QPainter
{
public:
    ReportPainter(const ReportInfo &info, QPrinter *printer);
    struct ReportTableSet
    {
        ReportTableSet(bool paging):paging(paging){}
        QPointF start()
        {
            return QPointF(30, 150);
        }
        qreal rowHeight(Check_Mode mode)
        {
            switch (mode) {
            case Check_Mode::Hypertension:
                return 40;
            case Check_Mode::InternalMedicine:
                return 25;
            case Check_Mode::IntensiveCareUnit:
                return 22;
            case Check_Mode::PhysicalExamination:
                return 25;
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
                return QVector<qreal>()<<160<<70<<100<<130;
            }
            return QVector<qreal>()<<160<<55<<55<<85<<115;
        }
        bool paging;
    };
private:
    void generalHeader();
    void generalDataPage(int page = 0);
    void generalFooter();
    void setFontSize(int pointSize, bool bold = false);
    QRectF drawTableCell(QRectF rect, const QString &txt, int align = Qt::AlignCenter, int offset = 0);
    QMap<Type, qreal> valueMap(const QJsonObject &data);
    void drawValue(const QJsonObject &parameter, QMap<Type, qreal> map,
                    QRectF rect, qreal min, qreal max, qreal _y);
    QString positionCn(int pos);
    void drawSudoku(QRectF rect, const QJsonObject &data);
    void drawCurve(QList<QPointF> points);
    QPointF intersection(qreal ssvri, qreal lswi, qreal cvp, qreal lap);
    void positionSymbol(int pos, QPointF point);
    QRectF rectF(qreal x, qreal y);
    template <class T>
    QString ekey(const T &t);
private:
    ReportInfo m_info;
    /** General paper size 793x1123 pixels, thermal printer paper size 272x1123 pixels. */
    const QSizeF m_size;
private:
    QPrinter *m_printer;
};

#endif // REPORTPAINTER_H
