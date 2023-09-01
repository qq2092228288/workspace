#ifndef REPORTDIALOG_H
#define REPORTDIALOG_H

#include <QDialog>
#include <QGridLayout>
#include <QJsonObject>

#include "reportset.h"

struct ReportInfo
{
    ReportInfo(Printer_Type _type, Check_Mode _mode, bool _paging, QJsonObject _data)
        : type(_type),mode(_mode),paging(_paging),data(_data){}
    Printer_Type type;
    Check_Mode mode;
    bool paging;
    QJsonObject data;
};

class ReportDialog : QDialog
{
    Q_OBJECT
public:
    explicit ReportDialog(const ReportInfo &info, QWidget *parent = nullptr);
private:
    ReportInfo m_info;
};


#endif // REPORTDIALOG_H
