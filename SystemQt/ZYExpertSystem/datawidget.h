#ifndef DATAWIDGET_H
#define DATAWIDGET_H

#include <QWidget>
#include "datanfilterwidgetns.h"

QT_BEGIN_NAMESPACE
namespace Ui { class DataWidget; }
QT_END_NAMESPACE

class DataTableModel;
class DataWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DataWidget(QWidget *parent = nullptr);
    virtual ~DataWidget();
signals:
    void hidden();
protected:
    void closeEvent(QCloseEvent *event) override;
private slots:
    void exportXlsxButtonSlot();
    void resetButtonSlot();
    void confirmButtonSlot();
    void setColumns();
private:
    Ui::DataWidget *ui;
    DataTableModel *model;
    QVector<DatanFilterWidgetNs::BaseFilter *> filterWidgets;
};

#endif // DATAWIDGET_H
