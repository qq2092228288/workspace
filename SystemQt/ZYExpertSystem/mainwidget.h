#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QScopedPointer>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWidget; }
QT_BEGIN_NAMESPACE

class ReportDialog;
class ReportConfigDialog;

class MainWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MainWidget(QWidget *parent = nullptr);
    virtual ~MainWidget();
private slots:
    void showReportButtonClicked();
signals:
private:
    Ui::MainWidget *ui;
    ReportDialog *reportDialog;
    ReportConfigDialog *configDialog;
};
typedef QScopedPointer<MainWidget> MainWidget_PTR;

#endif // MAINWIDGET_H
