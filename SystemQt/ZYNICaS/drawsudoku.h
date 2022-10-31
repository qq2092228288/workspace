//#ifndef DRAWSUDOKU_H
//#define DRAWSUDOKU_H

//#include <QDialog>
//#include <QPainter>
//#include <QPoint>
//#include <QPainterPath>
//#include <QPolygonF>
//#include <QDebug>



//class DrawSudoku : public QDialog
//{
//    Q_OBJECT
//public:
//    explicit DrawSudoku(QWidget *parent = nullptr);
//public slots:
//    void clear();
//    void setSi(char position, qreal si, bool record = false);
//    void setMap(char position, qreal map, bool record = false);
//protected:
//    virtual void paintEvent(QPaintEvent *event);
//    void drawCurve(QPainter &painter, QList<QPointF> &points, QString name);
//    QPolygonF getNormalPoint(QList<QPointF> &points, int si1, int si2,
//                             int map1, int map2, bool flag = true);
//    qreal intersectAxisX(qreal lswi, qreal ssvri);
//    qreal intersectAxisY(qreal lswi, qreal ssvri);
//    void removePoints(QList<QPointF> &points, int min, int max);
//    void drawPosition(QPainter &painter, char position, QPointF point, bool record);
//    QString getPosStr(char position);
//signals:

//private:
//    //体位
//    char m_cPositon = -1;
//    qreal m_cSi = 0;
//    qreal m_cMap = 0;
//    //记录的体位
//    char m_rPositon = -1;
//    qreal m_rSi = 0;
//    qreal m_rMap = 0;

//};

//#endif // DRAWSUDOKU_H
