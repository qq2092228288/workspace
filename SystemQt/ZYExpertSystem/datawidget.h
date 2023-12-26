#ifndef DATAWIDGET_H
#define DATAWIDGET_H

#include <QWidget>
#include <xlsxdocument.h>
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
    void showEvent(QShowEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
private:
    void xlsxSetSpan(QXlsx::Document &xlsx, const int &columnIndex);
    void getList();
private slots:
    void exportXlsxButtonSlot();
    void resetButtonSlot();
    void confirmButtonSlot();
    void setColumns();
    void setSpan();
private:
    Ui::DataWidget *ui;
    DataTableModel *model;
    QVector<DatanFilterWidgetNs::BaseFilter *> filterWidgets;
};

#endif // DATAWIDGET_H
