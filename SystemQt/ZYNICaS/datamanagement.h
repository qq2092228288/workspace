#ifndef DATAMANAGEMENT_H
#define DATAMANAGEMENT_H

#include <QObject>
#include <QtCharts>
#include <QChartView>
#include <QPixmap>
#include <QFileInfo>
#include <windows.h>
#include <QDateTime>


#include "systemconfigdialog.h"
#include "customctrl.h"
#include "infoeditdialog.h"
//#include "drawsudoku.h"
#include "zytebco.h"
//#include "httppost.h"
//#include "reportdatabase.h"
//#include "devicedatabase.h"
#include "mqttclient.h"

// 文件路径
class MyFilePath
{
public:
    MyFilePath(const QString &path = nullptr);
    QString initDir()           const;
    QString baseInfo()          const;
    QString reports()           const;
    QString showItems()         const;
    QString trendCharts()       const;
    QString hospitalLogo()      const;
    QString userAccountInfo()   const;
private:
    QString appPath;
};

typedef struct CheckedData
{
    QString pos;        // 体位
//    QString posture;    // 1半卧,2平躺,3抬腿
    QStringList values; // 检查获取的数据
}Cdata, *p_Cdata;

struct Argument
{
    Argument();
    Argument(QString _cn, QString _en, QString _unit, qreal _min, qreal _max, int _digit);
    Argument(QString _cn, QString _en, QString _unit, qreal _sbpmin, qreal _sbpmax,
             int _digit, QString _dbpcn, QString _dbpen, qreal _dbpmin, qreal _dbpmax);
    QString cn;
    QString en;
    QString unit;
    qreal min = 0;
    qreal max = 0;
    int digit = 0;
    QString dbpcn;
    QString dbpen;
    qreal dbpmin = 0;
    qreal dbpmax = 0;
};

typedef struct Arguments
{
    Arguments();
    Argument findArgument(const QString &en);
    qreal CVP = 4;
    qreal LAP = 9;
    QList<Argument> arguments;
    Argument find(const Type &type)
    {
        auto en = typeName(type);
        foreach (Argument arg, arguments) {
            if (arg.en == en) {
                return arg;
            }
        }
        return Argument();
    }
}Args;

// 数据管理
class DataManagement : public QObject
{
    Q_OBJECT
private:
    explicit DataManagement();
    virtual ~DataManagement();
    DataManagement(const DataManagement &) = delete;
    DataManagement &operator=(const DataManagement &) = delete;
private:
    static DataManagement instance;
public:
    static DataManagement &getInstance(){return instance;}
signals:
    void clear();
    void startCheck(bool);
    void sendSerialName(const QString &portName);
public:
    ZyTebco *getTebco() const;
    void setSize(const QSize &size);
    qreal wZoom() const;
    qreal hZoom() const;
    qreal zoom() const;
    QFont font(const int &size, const QString &family = "Microsoft YaHei") const;
    QSize rectSize(const int &w, const int &h);
    QString dialogQss(const double scale) const;
    void initCurrentPath();
    MyFilePath getPaths() const;
    Args &getArgs();
    CustomCtrlRegulator *getRegulator() const;
    HospitalInfo *getHospitalInfo() const;
    QString getMac() const;
    MqttClient *mqttClient() const;
    QString departmentName() const;
    QString idName() const;
public:
    void setHospitalInfo(HospitalInfo *hospitalInfo);
    void setBodyValue(BodyValue *bodyValue);
    void setRegulator(CustomCtrlRegulator *regulator);
public slots:
    QString reportCreated(bool record);
    void clearSlot();
    void customCtrlTimer(bool start);
private:
    QString flag(CustomCtrl *customCtrl, bool second);
    QString tip(qreal min, qreal max, qreal value);
    QString tip(qreal rValue, qreal cValue);
    // 报告结论
    QString reportResult(bool record);
    // 第一体位评价
    QString pevl(const Type &type, bool many = true);
    // 两个体位对比
    QString compare(const Type &type);
    // 前负荷，心力贮备
    QString preload();
    // 猝死风险提示
    QString riskTip(bool many);
//    QPixmap getQrCodeUrlPixmap(const QString &deviceId, const QString &reportTime);
private:
    QString m_departmentName;
    QString m_idName;
    MyFilePath m_filePath;
    HospitalInfo *m_pHospitalInfo;
    BodyValue *m_pBodyValue;
    CustomCtrlRegulator *m_pRegulator;
    Args args;
private:
    // 串口
    ZyTebco *m_pTebco;
    // 屏幕大小
    QSize m_winSize;
    MqttClient *m_client;
};

#endif // DATAMANAGEMENT_H
