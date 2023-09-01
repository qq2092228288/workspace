#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QComboBox>
#include <QLineEdit>
#include <QGridLayout>
#include "adsmodule.h"
#include "waveform.h"

typedef AdsModule::MessageDataType MsgDT;

typedef struct GraphicProperties
{
    GraphicProperties(const MsgDT &_type, const QString &_name) : type(_type),name(_name) {}
    MsgDT type;
    QString name;
}GProp;

class MainWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MainWidget(QWidget *parent = nullptr);
private slots:
    void operationButtonClicked();
    void newdata(AdsModule::MessageDataType type, double voltage);
signals:
private:
    QComboBox *comboBox;
    QLabel *label;
    QLineEdit *lineEdit;
    QPushButton *operationButton;
    QString openStr = QString("打开");
    QString closeStr = QString("关闭");
    QList<GProp> gpropList;
    QMap<MsgDT, Waveform *> waveformMap;
private:
    AdsModule *ads;
};

#endif // MAINWIDGET_H
