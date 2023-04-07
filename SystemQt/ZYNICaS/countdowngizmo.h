#ifndef COUNTDOWNGIZMO_H
#define COUNTDOWNGIZMO_H

#include <QWidget>
#include <QTimer>
#include <QLabel>
#include <QGridLayout>
#include <QMessageBox>
#include <QMouseEvent>
#include <QDebug>

class CountDownGizmo : public QWidget
{
    Q_OBJECT
public:
    explicit CountDownGizmo(const QString &timeHeader = nullptr, const QString &stopTxt = nullptr,
                            QWidget *parent = nullptr);

    void startCountDown(const int &sec);    // 秒

signals:
    void isStopStatus(bool status);

protected:
    virtual void mouseMoveEvent(QMouseEvent *event);

    virtual void mousePressEvent(QMouseEvent *event);

    virtual void mouseReleaseEvent(QMouseEvent *event);

private:
    QString getTimeTxt();

private slots:
    void timeoutSlot();

private:
    QPoint m_mousePoint;

    bool m_mousePress;

    QString m_timeHeader;

    QString m_stopTxt;
    // 定时器
    QTimer *m_pTimer;
    // 标签
    QLabel *m_pTimeLabel;
    // 计时时间
    int m_sec;
};

#endif // COUNTDOWNGIZMO_H
