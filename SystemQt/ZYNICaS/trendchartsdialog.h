#ifndef TRENDCHARTSDIALOG_H
#define TRENDCHARTSDIALOG_H

#include <QDialog>
#include <QHBoxLayout>
#include <QGridLayout>

class TrendChartsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit TrendChartsDialog(QWidget *parent = nullptr);
    void changeShow(const QString &current, const QString &change);
signals:
private:
    QGridLayout *dataGLayout;
    QList<QHBoxLayout *> hLayouts;
};

#endif // TRENDCHARTSDIALOG_H
