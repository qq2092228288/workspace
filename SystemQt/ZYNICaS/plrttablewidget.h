#ifndef PLRTTABLEWIDGET_H
#define PLRTTABLEWIDGET_H

#include <QDialog>
#include <QGridLayout>
#include <QLabel>
#include <QPixmap>
#include <QShowEvent>
#include <QDebug>

// PLRT被动抬腿试验参数类型
typedef enum PlrtArgType
{
    Hr = 1,
    Si,
    Ci,
    Sv,
    Co,
    Do2,
    Tfc,
    Isi
}PaType;
// 体位类型
typedef enum PostureType
{
    None,           // 初始
    HalfSleeper,    // 半卧
    LieFlat,        // 平躺
    LegLift         // 抬腿
}PosType;


class PlrtTableWidget : public QDialog
{
    Q_OBJECT
public:
    explicit PlrtTableWidget(QWidget *parent = nullptr);

    int rowCount() const;

    int columnCount() const;

    QStringList header() const;
public slots:
    // 清空
    void clear();
    // 设置图片
    void setPic(const PosType &fpos, const PosType &spos);
    // 设置体位
    void setPos(const QString &fpos, const QString &spos);
    // 设置数据
    void setPlrtData(const PaType &type, const double &fdata, const double &sdata);

    void setData(const QString &name, const double &fdata, const double &sdata);
signals:

private:
    enum LabelStatus
    {
        None,
        Normal,
        Warning
    };
    // label qss
    static QString labelQss(LabelStatus status);
    // data digit
    static double dataDigit(const double &value, const int &decimals);
    // 标签
    QList<QLabel*> labels() const;
    // 获取体位图片路径
    QString getPicFileName(const PosType &type);
    // 获取标签index
    int getLabelNumber(const int &row, const int &col);
    // 设置颜色
    void dataWarning(const PaType &type, bool w);

private:
    // 行数
    const int m_rows;
    // 列数
    const int m_columns;

    QGridLayout *mainLayout;
    // 列头
    QStringList m_headerText;
    // 标签
    QList<QLabel*> m_pLabels;
};

#endif // PLRTTABLEWIDGET_H
