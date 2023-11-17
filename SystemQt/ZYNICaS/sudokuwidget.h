#ifndef SUDOKUWIDGET_H
#define SUDOKUWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QPoint>
#include <QPainterPath>
#include <QPolygonF>
#include <QDebug>


struct ArgScope
{
    ArgScope(){}
    ArgScope(const double &min, const double &max) : min(min),max(max) {}
    double min;
    double max;
};

enum class SignType
{
    None,
    Circular,
    Square,
    Delta
};

struct DataPoint
{
    DataPoint(const double &map = 0, const double &si = 0, const SignType &type = SignType::Circular)
        : map(map),si(si),type(type) {}
    double map;
    double si;
    SignType type;
};

typedef struct SudokuBaseArg
{
    double cvp;
    double lap;
    ArgScope map;
    ArgScope si;
    ArgScope ssvri;
    ArgScope lswi;
    QList<DataPoint> points;
}SuBArg;

class SudokuWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SudokuWidget(const SuBArg &suBArg, QWidget *parent = nullptr);

public slots:
    void clear();

    void setMapScope(const double &min, const double &max);

    void setSiScope(const double &min, const double &max);

    void setSsvriScope(const double &min, const double &max);

    void setLswiScope(const double &min, const double &max);

    void setPoint(const double &map, const double &si, const SignType &type, const bool &firstPos);

protected:
    virtual void paintEvent(QPaintEvent *event);

private:
    double winScale() const;

    QString getSign(const SignType &type) const;

    void drawCurve(QPainter &painter, QList<QPointF> &points, QString name, bool flag);

    qreal intersectAxisX(qreal lswi, qreal ssvri);

    qreal intersectAxisY(qreal lswi, qreal ssvri);

    void removePoints(QList<QPointF> &points, int min, int max);

    QPolygonF getNormalPoint(QList<QPointF> &points, qreal minX, qreal maxX, qreal minY, qreal maxY, bool flag);
signals:
private:
    SuBArg m_suBArg;
};

#endif // SUDOKUWIDGET_H
