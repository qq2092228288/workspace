//#ifndef CREATEREPORTTHREAD_H
//#define CREATEREPORTTHREAD_H

//#include <QObject>
//#include <QThread>
//#include <QMutexLocker>

//enum Mode
//{
//    SaveAs,
//    PrintOut,
//    PrintPreview
//};

//class CreateReportThread : public QThread
//{
//    Q_OBJECT
//public:
//    void init();
//    void setStartupMode(const Mode &mode);
//    void setOpenArg(const QString &filePath, bool visible);
//    void setSaveAs(const QString &savePath);
//    void addMarks(const QString &mark, const QString &content);
//    void addBatchMarks(const QStringList &markList, const QStringList &contentList);
//    void addPic(const QString &sLabel, const QString &picPath);
//protected:
//    void run();
//private:
//    QMutex mutex;
//    Mode m_mode;
//    QString m_filePath;
//    bool m_visible;
//    QString m_savePath;
//    QStringList m_marksList;
//    QStringList m_contentList;
//    QStringList m_picLabelList;
//    QStringList m_picPathList;
//};

//#endif // CREATEREPORTTHREAD_H
