#ifndef BASEARGWIDGET_H
#define BASEARGWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QDateTime>


class BaseArgWidget : public QWidget
{
    Q_OBJECT
public:
    explicit BaseArgWidget(const qreal &fontZoom = 1, QWidget *parent = nullptr);
    explicit BaseArgWidget(const QString &main, const QString &secondary, const QString &scope,
                           const QString &unit, const int &digit, const qreal &fontZoom,
                           QWidget *parent = nullptr);
    virtual ~BaseArgWidget();
    // 获取主标签
    QString getMainString() const;
    // 获取副标签
    QString getSecondaryString() const;
    // 获取参数值
    QString getArgValueString() const;
    // 获取范围值标签
    QString getScopeString() const;
    // 获取单位
    QString getUnitString() const;
    // 获取精度
    int getDigit() const;
public slots:
    // 清空参数值
    void clearArgValue();
    // 设置主标签
    void setMainLabel(const QString &str);
    // 设置副标签
    void setSecondaryLabel(const QString &str);
    // 设置参数值
    void setArgValueLindeEditSlot(const qreal &value);
    // 设置范围值标签
    void setScopeLabel(const QString &str);
    // 设置单位
    void setUnitLabel(const QString &str);
    // 设置精度
    void setDigit(const int &digit);
signals:
    // 值发生改变
    void argValueChanged(const qint64 &time, const QString &oldValue, const QString &newValue);
    // 双击控件
    void mouseDoubleClicked();
protected:
    // 双击事件
    virtual void mouseDoubleClickEvent(QMouseEvent *event);
private slots:

private:
    // 主标签
    QLabel *m_pMainLabel;
    // 副标签
    QLabel *m_pSecondaryLabel;
    // 参数值
    QLineEdit *m_pArgValueLineEdit;
    // 正常范围值
    QLabel *m_pScopeLabel;
    // 单位
    QLabel *m_pUnitLabel;
    // 值精度
    int m_digit;
};

#endif // BASEARGWIDGET_H
