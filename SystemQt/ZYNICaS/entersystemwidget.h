#ifndef ENTERSYSTEMWIDGET_H
#define ENTERSYSTEMWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QGridLayout>
#include <QButtonGroup>
#include <QLabel>
#include <QLineEdit>
#include <QFile>
#include <QMessageBox>
#include <QHideEvent>
#include <QTabWidget>
#include <QRadioButton>
#include <QButtonGroup>
#include <QDebug>
#include <QFrame>
#include <QMessageBox>
#include <QSharedPointer>
#include <QGroupBox>
#include <QTimer>
#include <QDateTime>
#include <QApplication>
#include <QThread>
#include <QQueue>
#include <QFuture>
#include <QtConcurrent>

#include "infoeditdialog.h"
#include "bpeditdialog.h"
#include "auxargdialog.h"
#include "drawwaveforms.h"
#include "sudokuwidget.h"
#include "customctrl.h"
#include "trendchartswidget.h"
#include "plrttablewidget.h"
#include "reportdatajson.h"
#include "reportset.h"

class EnterSystemWidget : public QWidget
{
    Q_OBJECT
public:
    explicit EnterSystemWidget(QWidget *parent = nullptr);
    virtual ~EnterSystemWidget();
    // 显示后再隐藏，处理QWidget没有show截屏导致控件大小改变不可控问题
    void trendChartLayout();
protected:
    void showEvent(QShowEvent *event);
    void closeEvent(QCloseEvent *event);
    //初始化
    void initInfoModule();
    void initBPModule();
    void initPosModule();
    void initOscModule();
    void initDataModule();
    void initReportModule();
    void signalsAndSlots();

public slots:
    void timeoutSlot();
    void changeShow(const Type &current, const Type &change);
    void setData(const uchar &type, const short &value);
    void showBpDialogSlot();
    void setBPValue(const QString &sbp, const QString &dbp);
    void changeMode(const int &id);
    void recordPosition();
    void changePosition(int id);
    void createReport();
    void clearUiSlot();
    void systemModeChanged(Check_Mode mode);
    void startupTestSlot();
private:
    bool detectedData();
    void setCtrlValue(const Type &type, const double &value);
    bool isStartCheck();
signals:
    void recordValue();
    void widgetClose();
    void createdReport(const qint64 &timestamp, const QString &baseDataString);
    void startDemoMode(bool);
private:
    QTimer *timer;
    QGridLayout *mainLayout;
    QVBoxLayout *firstColLayout;
    QVBoxLayout *secColLayout;

    //患者信息模块
    QHBoxLayout *infoLayout;
    QGroupBox *infoGroupBox;
    QPushButton *editInfoBtn;       //添加
    QLabel *nameLabel;              //姓名
    QLabel *numLabel;               //病历号
    QLineEdit *nameLineEdit;
    QLineEdit *numLineEdit;
    InfoEditDialog *infoDialog;

    //血压模块
    QGroupBox *bpGroupBox;
    QLabel *sbpLabel;               //收缩压
    QLabel *dbpLabel;               //舒张压
    QLineEdit *sbpLineEdit;
    QLineEdit *dbpLineEdit;
    QPushButton *mdiBtn;            //手动输入
    BPEditDialog *bpDialog;

    //体位模块
    QGroupBox *pldGroupBox;
    QButtonGroup patternGroup;
    QRadioButton *manyBtn;          //多体位
    QRadioButton *singleBtn;        //单体位
    QButtonGroup posGroup;
    QRadioButton *halfLieBtn;       //半卧
    QRadioButton *lieBtn;           //平躺
    QRadioButton *legLiftBtn;       //抬腿
    QPushButton *recordBtn;         //记录体位
    QPushButton *startupTestBtn;    //启动试验
    PlrtTableWidget *plrtWidget;
    PosType rpos;

    //图形模块
    QGroupBox *ecgGroupBox;
    QGroupBox *diffGroupBox;
    QGroupBox *admitGroupBox;
    DrawWaveforms *ecgDraw;
    DrawWaveforms *diffDraw;
    DrawWaveforms *admitDraw;

    //数据模块
    QGroupBox *dataGroupBox;
    QGridLayout *dataGLayout;
    CustomCtrlRegulator *regulator;
    BodyValue bodyValue;
    SudokuWidget *sudokuWidget;
    qreal efValue = 0;
    QVector<qreal> svValues;

    //返回、生成报告、九宫格图、辅助参数
    QGroupBox *operationGroupBox;
    QPushButton *backBtn;
    QPushButton *reportBtn;
    QPushButton *trendChartBtn;
    QPushButton *auxArgBtn;
    TrendChartsWidget *trendChartsWidget;
    AuxArgDialog *auxArgDialog;

    // 记录数据
    ReportDataJson *reportJson;
};

#endif // ENTERSYSTEMWIDGET_H
