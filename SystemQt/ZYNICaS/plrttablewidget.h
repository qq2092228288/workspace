#ifndef PLRTTABLEWIDGET_H
#define PLRTTABLEWIDGET_H

#include <QDialog>
#include <QGridLayout>
#include <QLabel>
#include <QPixmap>
#include <QShowEvent>
#include <QTimer>
#include <QMessageBox>
#include <QPushButton>
#include <QProgressBar>
#include <QDebug>

// PLRT被动抬腿试验参数类型
typedef enum PlrtArgType
{
    Hr = 1,
    Si,
    Ci,
    Sv,
    Co,
//    Do2,
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

typedef enum TestStatus
{
    Started,    // 启动试验
    Paused,     // 试验暂停/重新启动
    Stopped,    // 终止试验
    Completed   // 试验完成
}TestS;

class PlrtTableWidget : public QDialog
{
    Q_OBJECT
public:
    explicit PlrtTableWidget(QWidget *parent = nullptr);

    int rowCount() const;

    int columnCount() const;

    QStringList header() const;

    void setCountDown(const int &sec);    // 秒
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
protected:
    virtual void closeEvent(QCloseEvent *event);

    virtual void showEvent(QShowEvent *event);

    virtual void mouseMoveEvent(QMouseEvent *event);

    virtual void mousePressEvent(QMouseEvent *event);

    virtual void mouseReleaseEvent(QMouseEvent *event);
protected slots:
//    void startSlot();

    void timeroutSlot();

    void stopSlot();

    void pauseSlot();
signals:
    void editBp();      // 编辑血压

    void status(TestS status);
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
    // 设置进度条文本
    void setProcessBarTxt(const int &value);

private:
    QPoint m_mousePoint;

    bool m_mousePress;
    // 行数
    const int m_rows;

    QGridLayout *mainLayout;
    // 列头
    QStringList m_headerText;
    // 标签
    QList<QLabel*> m_pLabels;
    // 开始
//    QPushButton *m_pStartBtn;
    // 输入血压
    QPushButton *m_pEditBpBtn;
    // 终止试验
    QPushButton *m_pStopBtn;
    // pause
    QPushButton *m_pPauseBtn;
    // 定时器
    QTimer *m_pTimer;
    // 计时时间
    int m_sec;
    QProgressBar *m_pProgressBar;
};

#endif // PLRTTABLEWIDGET_H
