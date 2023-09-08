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
#include "createreportthread.h"
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
    QString tempDir()           const;
    QString baseInfo()          const;
    QString showItems()         const;
    QString trendCharts()       const;
    QString single_dot()        const;
    QString many_dot()          const;
    QString xsingle_dot()       const;
    QString xmany_dot()         const;
    QString psingle_dot()       const;
    QString pmany_dot()         const;
    QString hypertension_dot()  const;
    QString hypertension1_dot() const;
    QString interMedicine_dot() const;
    QString interMedicine1_dot()const;
    QString icu_dot()           const;
    QString icu1_dot()          const;
    QString mPhyExam_dot()      const;
    QString mPhyExam1_dot()     const;
    QString sPhyExam_dot()      const;
    QString record_dz()         const;
    QString current_dz()        const;
    QString sudoku()            const;
    QString isiCurve()          const;
    QStringList trendchartspic()const;
    QString reports()           const;
    QString config_zip()        const;
    QString qrCode()            const;
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

enum HrvArg
{
    Nnvgr,
    Sdnn,
    Pnn50,
    Rmssd
};

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
    // 请求耗材列表
    void onlineConsumableList(const QString &pageNum, const QString &pageSize, const QString &deviceId,
                              const QString &id, const QString &consumableTypeId);
    void startCheck(bool);
    void sendSerialName(const QString &portName);
public:
    ZyTebco *getTebco() const;
//    IdCheck *getIdCheck() const;
    void setSize(const QSize &size);
    qreal wZoom() const;
    qreal hZoom() const;
    qreal zoom() const;
    QFont font(const int &size, const QString &family = "Microsoft YaHei") const;
    QSize rectSize(const int &w, const int &h);
    QString dialogQss(const double scale) const;
    void initCurrentPath();
    MyFilePath getPaths() const;
    bool isRecordPos() const;
    QString getNewReportName() const;
    Args &getArgs();
    CustomCtrlRegulator *getRegulator() const;
    HospitalInfo *getHospitalInfo() const;
//    HttpPost *httpPost() const;
    QString getMac() const;
//    ReportDataBase *reportDataBase() const;
//    DeviceDatabase *deviceDatabase() const;
//    int surplus() const;
    MqttClient *mqttClient() const;
public:
    void setHospitalInfo(HospitalInfo *hospitalInfo);
    void setBodyValue(BodyValue *bodyValue);
    void setRegulator(CustomCtrlRegulator *regulator);
    // 心阻抗图
    void setdZ(QChartView *dZ);
    void setSudoku(QWidget *sudoku);
//    void setReportDataBase(ReportDataBase *reportDataBase);
//    void setDeviceDatabase(DeviceDatabase *deviceDatabase);
public slots:
    void recordPosition(QString position);
    QString saveReport(QDateTime curTime, QString position, bool record);
    QString reportCreated(bool record);
    void clearSlot();
    void reportPreview(const QString &path);
    void reportPrintOut(const QString &path);
    void customCtrlTimer(bool start);
//    void requestConsumableList();
private:
    void saveInfo(Cdata &cdata, bool second = false);
    QString flag(CustomCtrl *customCtrl, bool second);
    QString tip(qreal min, qreal max, qreal value);
    QString tip(qreal rValue, qreal cValue);
    QString mArg(const Type &type, const int &printer, bool isMany);
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
    void addPlrt(const int &num, const Type &type);
    QString posImagePath(const QString &posture);
    QString getScope(const double &init, const double &offset);
    QString getScope(const HrvArg &hrvArg, const int &age);
private:
    CreateReportThread *reportThread;
    MyFilePath m_filePath;
    HospitalInfo *m_pHospitalInfo;
    BodyValue *m_pBodyValue;
    CustomCtrlRegulator *m_pRegulator;
    QChartView *m_pdZ;       //阻抗图
    QWidget *m_pSudoku;      //九宫格图
    //当前、记录的信息
    bool isRecord;
    Cdata m_recordInfo;
    Cdata m_currentInfo;
    QImage m_pSudokuImage;
    QString m_newReportName;
    Args args;
private:
    // 串口
    ZyTebco *m_pTebco;
    // 屏幕大小
    QSize m_winSize;
//    IdCheck *m_pIdCheck;
//    HttpPost *m_pHttpPost;
//    ReportDataBase *m_pReportDataBase;
//    DeviceDatabase *m_pDeviceDatabase;
    MqttClient *m_client;
};

#endif // DATAMANAGEMENT_H
