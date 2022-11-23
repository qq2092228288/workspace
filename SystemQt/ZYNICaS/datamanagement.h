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
    MyFilePath(const QString &path = nullptr);
    QString initDir()     const;
    QString tempDir()     const;
    QString baseInfo()    const;
    QString showItems()   const;
    QString trendCharts() const;
    QString single_dot()  const;
    QString many_dot()    const;
    QString xsingle_dot() const;
    QString xmany_dot()   const;
    QString record_dz()   const;
    QString current_dz()  const;
    QString sudoku()      const;
    QString reports()     const;
    QString config_zip()  const;
private:
    QString appPath;
};

typedef struct CheckedData
{
    QString pos;        //体位
    QString posture;    // 1半卧,2平躺,3抬腿
    QStringList values; //检查获取的数据
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
    QString saveReport(QString position, bool record);
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
    DrawSudoku *m_pSudoku;   //九宫格图
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
