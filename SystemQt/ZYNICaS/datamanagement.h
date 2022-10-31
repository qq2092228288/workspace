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
#include "drawsudoku.h"
#include "zytebco.h"
#include "createreportthread.h"
#include "idcheck.h"

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
    QString single_dot()  const {return appPath + "dot/singlePositionTemplate.dot";}
    QString many_dot()    const {return appPath + "dot/manyPositionTemplate.dot";}
    QString xsingle_dot() const {return appPath + "dot/singlePositionXprinterTemplate.dot";}
    QString xmany_dot()   const {return appPath + "dot/manyPositionXprinterTemplate.dot";}
    QString record_dz()   const {return tempDir() + "record.png";}
    QString current_dz()  const {return tempDir() + "current.png";}
//    QString sudoku()      const {return tempDir() + "sudoku.png";}
    QString reports()     const {return appPath + "reports";}
    QString config_zip()  const {return initDir() + "config.zip";}
private:
    QString appPath;
};

typedef struct CheckedData
{
    QString pos;        //体位
    QStringList values; //检查获取的数据
}Cdata, *p_Cdata;

struct Argument
{
    Argument(QString _cn, QString _en, QString _unit, qreal _min, qreal _max)
        : cn(_cn),en(_en),unit(_unit),min(_min),max(_max){}
    QString cn;
    QString en;
    QString unit;
    qreal min;
    qreal max;
};

typedef struct Arguments
{
    qreal CVP = 4;
    qreal LAP = 9;
    Argument CO = Argument("心排量", "CO", "L/min", 1.5, 30);
    Argument CI = Argument("心脏指数", "CI", "L/min·m²", 1.4, 8.7);
    Argument SV = Argument("搏排量", "SV", "mL", 10, 500);
    Argument SI = Argument("心搏指数", "SI", "mL/m²", 15, 140);
    Argument HRV = Argument("心率变异性", "HRV", "%", -50, 50);
    Argument TFC = Argument("胸液传导性", "TFC", "1/Ω", 0.025, 0.045);
    Argument EDI = Argument("舒张末期指数", "EDI", "mL/beat·m²", 54, 130);
    Argument Vol = Argument("血管容积", "Vol", "%", -50, 50);
    Argument SVR = Argument("系统阻力", "SVR", "dyn·s·m²/cm^5", 489, 2262);
    Argument SSVR = Argument("每搏外周阻力", "SSVR", "dyn·s/cm^5", 119.6, 429.2);
    Argument SSVRI = Argument("每搏外周阻力指数", "SSVRI", "dyn·s/cm^5/beat/m²", 99.7, 185.1);
    Argument SVRI = Argument("系统阻力指数", "SVRI", "dyn·s·m²/cm^5", 1056, 4000);
    Argument Vas = Argument("血管顺应性", "Vas", "%", -50, 50);
    Argument EF = Argument("射血分数", "EF", "%", -45, 68);
    Argument PEP = Argument("射血前期", "PEP", "ms", 50, 120);
    Argument LVET = Argument("射血时间", "VET", "ms", 250, 360);
    Argument LSW = Argument("左心室每搏做功", "LSW", "g·m", 20, 90);
    Argument LSWI = Argument("每搏做功指数", "LSWI", "g·m/beat/m²", 39.3, 73.0);
    Argument LCW = Argument("左心室做功", "LCW", "kg·m", 5.4, 10.0);
    Argument LCWI = Argument("左心室做功指数", "LCWI", "kg·m/m²", 1.81, 7.06);
    Argument STR = Argument("收缩时间比", "STR", "-", 0, 0.4);
    Argument EPCI = Argument("射血收缩指数", "EPCI", "1/s", 0.038, 0.076);
    Argument ISI = Argument("变力状态指数", "ISI", "1/s²", 0.90, 1.70);
    Argument Ino = Argument("收缩变力性", "Ino", "%", -50, 50);
    Argument HR = Argument("心率", "HR", "beats/min", 40, 220);
//    Argument SBP = Argument("收缩压", "SBP", "mmHg", 90, 140);
    Argument BP = Argument("收缩压/舒张压", "SBP/DBP", "mmHg", 90, 140);
    Argument DBP = Argument("舒张压", "DBP", "mmHg", 60, 90);
    Argument MAP = Argument("平均动脉压", "MAP", "mmHg", 70, 105);
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
public:
    void setHospitalInfo(HospitalInfo *hospitalInfo);
    void setBodyValue(BodyValue *bodyValue);
    void setRegulator(CustomCtrlRegulator *regulator);
    // 心阻抗图
    void setdZ(QChartView *dZ);
//    void setSudoku(DrawSudoku *sudoku);
public slots:
    void recordPosition(QString position);
    void saveReport(QString position, bool record);
    void clearSlot();
    void reportPreview(const QString &path);
    void reportPrintOut(const QString &path);
    void customCtrlTimer(bool start);
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
//    DrawSudoku *m_pSudoku;      //九宫格图
    //当前、记录的信息
    bool isRecord;
    Cdata m_recordInfo;
    Cdata m_currentInfo;
    QImage m_pSudokuImage;
    QString m_newReportName;
    Args args;
private:
    // 串口
    QThread *m_pThread;
    ZyTebco *m_pTebco;
    // 屏幕大小
    QSize m_winSize;
    IdCheck *m_pIdCheck;
};

#endif // DATAMANAGEMENT_H
