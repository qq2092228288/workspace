#ifndef PAINTERCONFIG_H
#define PAINTERCONFIG_H

#include <QPainter>
#include <QDateTime>
#include <QJsonObject>

#include "reportdataname.h"

enum PageType
{
    MainPage_0 = 0,
    MainPage_1 = 1,
    ImagePage  = 2
};

class PainterConfig
{
public:
    explicit PainterConfig(QPainter *painter, const int &width, const int &height,
                           const QJsonObject &object, bool samepage);
    explicit PainterConfig(QPainter *painter, const QSize &size, const QJsonObject &object, bool samepage);

    enum Arrow
    {
        Null,
        Up,
        Down
    };
    enum WaveformType
    {
        Electrocardiogram   = 0,
        Differential        = 1,
        Impedance           = 2,
        Resp                = 3
    };
public:
    void paintPage(const PageType &ptype);
protected:
    void paintMain(const int &page = 0);
    void paintImagePage();
    QPainter *painter();
    void initPage();
    double paintHeader(const QString &title);
    double paintFooter();
    void paintValue(const QRectF &rectf, const qreal &value, const qreal &min, const qreal &max);
    void paintArrow(const QRectF &rectf, const Arrow &arrow);
    void paintSymbol(const QRectF &rectf, const int &pos);
    void paintSymbol(const QPointF &pf, const int &pos);
    void paintSymbols(const QRectF &rectf, const int &pos1, const int &pos2, const qreal &fvalue, const qreal &svalue);
    void paintWaveform(const QRectF &rectf, bool first);
    void paintWaveform(const QRectF &rectf, const QJsonArray &waveform, const WaveformType &type, bool first);
    void paintBpChart(const QRectF &rectf);
    void paintIsiSvChart(const QRectF &rectf);
    void paintMaceChart(const QRectF &rectf);
    void paintClt(const QRectF &rectf);
    void paintArrow(QPointF start, QPointF end);
    void paintCurve(QList<QPointF> points);
    void initPaint();
private:
    inline QString waveformCn(const WaveformType &type) const;
    inline QString compare(qreal v1, qreal v2) const;
    inline qreal circleX(qreal y, qreal r, qreal ox, qreal oy, bool negative) const;
    inline qreal limits(qreal actual, qreal min, qreal max) const;
    inline QString symbol(const int &pos, bool hollow = true) const;
    inline int flag(const qreal &source, const qreal &target) const;
    inline Arrow arrow(const qreal &fvalue, const qreal &min, const qreal &max, const Type &type) const;
    inline Arrow arrow(const qreal &fvalue, const qreal &svalue, const Type &type) const;
    /*! percent: px, py, pw, ph, ps (0, 100) */
    QFont font(const qreal &ps, const bool &bold, const QString &family = "Microsoft YaHei");
    inline QPointF point(const qreal &px, const qreal &py) const;
    inline QSizeF size(const qreal &pw, const qreal &ph) const;
    inline QLineF line(const qreal &px1, const qreal &py1, const qreal &px2, const qreal &py2) const;
    inline QLineF line(const qreal &px1, const qreal &py1, const QPointF &p2) const;
    inline QLineF line(const QPointF &p1, const qreal &px2, const qreal &py2) const;
    inline QRectF rect(const qreal &px, const qreal &py, const qreal &pw, const qreal &ph) const;
    inline QRectF rect(const qreal &px, const qreal &py, const QSizeF &size) const;
    inline QRectF rect(const QPointF &point, const qreal &pw, const qreal &ph) const;
    /*! shrink by 100 times  */
    inline qreal shrink(const qreal &value) const;
private:
    QPainter *m_painter;
    const int m_width;
    const int m_height;
    const QJsonObject &m_object;
    const bool m_samepage;
    QFont m_font;
private:
    const PatientInfo m_info;
    const Place m_place;
    Position m_fp;
    Position m_sp;
    QVector<Type> m_fvect;
    QVector<Type> m_svect;
    QVector<Type> m_plrVect;
};

#endif // PAINTERCONFIG_H
