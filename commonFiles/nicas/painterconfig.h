#ifndef PAINTERCONFIG_H
#define PAINTERCONFIG_H

#include <QPainter>

struct HeaderMsg
{
    HeaderMsg(){}
    HeaderMsg(const QString &title, const QString &hospital, const QString &department,
              const QString &name, const QString &id, const QString &gender,
              const int &age, const int &height, const int &weight, const double &bsa)
        : title(title), hospital(hospital), department(department)
        , name(name), id(id), gender(gender)
        , age(age), height(height), weight(weight), bsa(bsa)
    {

    }
    QString title;
    QString hospital;
    QString department;
    QString name;
    QString id;
    QString gender;
    int age;
    int height;
    int weight;
    double bsa;
};

class PainterConfig
{
public:
    explicit PainterConfig(QPainter *painter, const int &width, const int &height);
    explicit PainterConfig(QPainter *painter, const QSize &size);
    QPainter *painter();
public:
    void initPage();
    double paintHeader(const HeaderMsg &msg);
    void paintFooter(const QDateTime &time, const QString &name);
    void paintMain(const double &hEnd, const QJsonObject &object);
private:
    /*! percent: px, py, pw, ph, ps (0, 100) */
    QFont font(const qreal &ps, const bool &bold);
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
    QFont m_font;
};

#endif // PAINTERCONFIG_H
