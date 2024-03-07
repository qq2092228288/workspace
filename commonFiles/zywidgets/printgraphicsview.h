#ifndef PRINTGRAPHICSVIEW_H
#define PRINTGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QPushButton>
#include <QToolBar>
#include <QGridLayout>
#include <QPrinter>
#include <QComboBox>
#include <QListView>
#include <QDialog>
#include <QDebug>

class PrintGraphicsView : public QGraphicsView
{
    Q_OBJECT

public:
    PrintGraphicsView(const bool &samepage, const QPixmap &logo, const QJsonObject &object,
                      QGraphicsScene *scene, QWidget *parent = nullptr);
signals:
    void startRecache();
    void endRecache();
protected:
    bool viewportEvent(QEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void changeScale();
private slots:
    void zoom(bool in);
    void editingFinished();
    void scaleComboBoxActivated();
    void pageSettingButtonClicked();
    void printerButtonClicked();
private:
    QSize pageSize() const;
private:
    const bool &m_samepage;
    const QPixmap m_logo;
    const QJsonObject &m_object;
private:
    const QStringList itemList;
    const qreal minFactor = 0.5;
    const qreal maxFactor = 10.0;
    QComboBox *scaleComboBox;
    QPrinter m_printer;
    qreal totalScaleFactor = 1;
};

#endif // PRINTGRAPHICSVIEW_H
