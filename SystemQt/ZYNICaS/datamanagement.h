#ifndef DATAMANAGEMENT_H
#define DATAMANAGEMENT_H

#include <QObject>
#include <QtCharts>
#include <QChartView>
#include <QPixmap>
#include <QFileInfo>
#include <windows.h>
#include <QDateTime>
#include <QUrlQuery>

#include "systemconfigdialog.h"
#include "customctrl.h"
#include "infoeditdialog.h"
#include "drawsudoku.h"
#include "zytebco.h"
#include "createreportthread.h"
#include "idcheck.h"


QString ArgsNameToHttp(const QString &argsName);

// 文件路径
class MyFilePath
{
public:
    MyFilePath(){}
    MyFilePath(const QString &path) {appPath = path;}
    QString initDir()     const {return appPath + "init/";}
    QString tempDir()     const {return appPath + "temp/";}
    QString baseInfo()    const {return initDir() + "baseInfo.ini";}
    QString showItems()   const {return initDir() + "showItems.ini";}
    QString trendCharts() const {return initDir() + "trendCharts.ini";}
    QString single_dot()  const {return appPath + "dot/singlePositionTemplate.dot";}
    QString many_dot()    const {return appPath + "dot/manyPositionTemplate.dot";}
    QString xsingle_dot() const {return appPath + "dot/singlePositionXprinterTemplate.dot";}
    QString xmany_dot()   const {return appPath + "dot/manyPositionXprinterTemplate.dot";}
    QString record_dz()   const {return tempDir() + "record.png";}
    QString current_dz()  const {return tempDir() + "current.png";}
    QString sudoku()      const {return tempDir() + "sudoku.png";}
    QString reports()     const {return appPath + "reports";}
    QString config_zip()  const {return initDir() + "config.zip";}
private:
    QString appPath;
};

typedef struct CheckedData
{
    QString pos;        //体位
    QString posture;        // 1半卧,2平躺,3抬腿
    QStringList values; //检查获取的数据
}Cdata, *p_Cdata;

struct Argument
{
    Argument(){}
    Argument(QString _cn, QString _en, QString _unit, qreal _min, qreal _max, int _digit)
        : cn(_cn),en(_en),unit(_unit),min(_min),max(_max),digit(_digit){}
    Argument(QString _cn, QString _en, QString _unit, qreal _sbpmin, qreal _sbpmax, int _digit, qreal _dbpmin, qreal _dbpmax)
        : cn(_cn),en(_en),unit(_unit),min(_sbpmin),max(_sbpmax),digit(_digit),dbpmin(_dbpmin),dbpmax(_dbpmax){}
    QString cn;
    QString en;
    QString unit;
    qreal min = 0;
    qreal max = 0;
    int digit = 0;
    qreal dbpmin = 0;
    qreal dbpmax = 0;
};

typedef struct Arguments
{
    qreal CVP = 4;
    qreal LAP = 9;
    QList<Argument> arguments = QList<Argument>()
    <<Argument("心输出量", "CO", "L/min", 3.5, 9.0, 1)
    <<Argument("心脏指数", "CI", "L/min·m²", 2.0, 5.0, 1)
    <<Argument("心搏量", "SV", "mL/beat", 50, 120, 1)
    <<Argument("心搏指数", "SI", "mL/beat·m²", 35, 65, 0)
    <<Argument("心率变异性", "HRV", "%", -50, 50, 0)
    <<Argument("胸液传导性", "TFC", "1/Ω", 0.025, 0.045, 3)
    <<Argument("舒张末期指数", "EDI", "mL/beat·m²", 54, 130, 0)
    <<Argument("血管容积", "Vol", "%", -50, 50, 0)
    <<Argument("系统阻力", "SVR", "dyn·s·m²/cm^5", 489, 2262, 0)
    <<Argument("每搏外周阻力", "SSVR", "dyn·s/cm^5", 119.6, 429.2, 1)
    <<Argument("每搏外周阻力指数", "SSVRI", "dyn·s/cm^5/beat/m²", 99.7, 185.1, 1)
    <<Argument("系统阻力指数", "SVRI", "dyn·s·m²/cm^5", 1056, 4000, 0)
    <<Argument("血管顺应性", "Vas", "%", -50, 50, 0)
    <<Argument("射血前期", "PEP", "ms", 50, 120, 0)
    <<Argument("左室射血时间", "LVET", "ms", 250, 350, 0)
    <<Argument("左心室每搏做功", "LSW", "g·m", 20, 90, 0)
    <<Argument("每搏做功指数", "LSWI", "g·m/beat/m²", 39.3, 73.0, 1)
    <<Argument("左心室做功", "LCW", "kg·m", 5.4, 10.0, 1)
    <<Argument("左心室做功指数", "LCWI", "kg·m/m²", 1.81, 7.06, 2)
    <<Argument("收缩时间比", "STR", "-", 0, 0.4, 1)
    <<Argument("射血期收缩指数", "EPCI", "1/s", 0.038, 0.076, 3)
    <<Argument("变力状态指数", "ISI", "1/s²", 0.90, 1.70, 2)
    <<Argument("收缩变力性", "Ino", "%", -50, 50, 0)
    <<Argument("心率", "HR", "bpm", 60, 100, 0)
    <<Argument("收缩压/舒张压", "SBP/DBP", "mmHg", 90, 140, 0, 60, 90)
    <<Argument("平均动脉压", "MAP", "mmHg", 70, 105, 0);
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
    void startThread();
    ZyTebco *getTebco() const;
    IdCheck *getIdCheck() const;
    void setSize(const QSize &size);
    qreal wZoom() const;
    qreal hZoom() const;
    qreal zoom() const;
    QFont font(const int &size, const QString &family = "Microsoft YaHei") const;
    QSize rectSize(const int &w, const int &h);
    QString dialogQss() const;
    void initCurrentPath();
    MyFilePath getPaths() const;
    bool isRecordPos() const;
    QString getNewReportName() const;
    Args &getArgs();
    CustomCtrlRegulator *getRegulator() const;
    HospitalInfo *getHospitalInfo() const;
public:
    void setHospitalInfo(HospitalInfo *hospitalInfo);
    void setBodyValue(BodyValue *bodyValue);
    void setRegulator(CustomCtrlRegulator *regulator);
    // 心阻抗图
    void setdZ(QChartView *dZ);
    void setSudoku(DrawSudoku *sudoku);
public slots:
    void recordPosition(QString position);
    void saveReport(QString position, bool record);
    void clearSlot();
    void reportPreview(const QString &path);
    void reportPrintOut(const QString &path);
    void customCtrlTimer(bool start);
    void setJsonArray(bool many);
private:
    void saveInfo(Cdata &cdata, bool second = false);
    QString flag(CustomCtrl *customCtrl, bool second);
    QString tip(qreal min, qreal max, qreal value);
    QString tip(qreal rValue, qreal cValue);
    // 第一体位评价
    QString pevl(const Type &type, bool many = true);
    // 两个体位对比
    QString compare(const Type &type);
    // 前负荷，心力贮备
    QString preload();
    // 猝死风险提示
    QString riskTip(bool many);
private:
    CreateReportThread *reportThread;
    MyFilePath m_filePath;
    HospitalInfo *m_pHospitalInfo;
    BodyValue *m_pBodyValue;
    CustomCtrlRegulator *m_pRegulator;
    QChartView *m_pdZ;       //阻抗图
    DrawSudoku *m_pSudoku;   //九宫格图
    //当前、记录的信息
    bool isRecord;
    Cdata m_recordInfo;
    Cdata m_currentInfo;
    QImage m_pSudokuImage;
    QString m_newReportName;
    Args args;
    QUrlQuery m_urlQuery;
private:
    // 串口
    QThread *m_pThread;
    ZyTebco *m_pTebco;
    // 屏幕大小
    QSize m_winSize;
    IdCheck *m_pIdCheck;
};

#endif // DATAMANAGEMENT_H
