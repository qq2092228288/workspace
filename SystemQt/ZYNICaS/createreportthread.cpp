#include "createreportthread.h"
#include "reportword.h"
#include <windows.h>


void CreateReportThread::init()
{
    m_mode = Mode::SaveAs;
    m_filePath.clear();
    m_visible = false;
    m_savePath.clear();
    m_marksList.clear();
    m_contentList.clear();
    m_picLabelList.clear();
    m_picPathList.clear();
}

void CreateReportThread::setStartupMode(const Mode &mode)
{
    this->m_mode = mode;
}

void CreateReportThread::setOpenArg(const QString &filePath, bool visible)
{
    this->m_filePath = filePath;
    this->m_visible = visible;
}

void CreateReportThread::setSaveAs(const QString &savePath)
{
    this->m_savePath = savePath;
}

void CreateReportThread::addMarks(const QString &mark, const QString &content)
{
    this->m_marksList<<mark;
    this->m_contentList<<content;
}

void CreateReportThread::addBatchMarks(const QStringList &markList, const QStringList &contentList)
{
    this->m_marksList<<markList;
    this->m_contentList<<contentList;
}

void CreateReportThread::addPic(const QString &sLabel, const QString &picPath)
{
    this->m_picLabelList<<sLabel;
    this->m_picPathList<<picPath;
}

void CreateReportThread::run()
{
    QMutexLocker locker(&mutex);
    HRESULT r = OleInitialize(0);
    if (r != S_OK && r != S_FALSE)
    {
        qWarning("Qt:初始化Ole失败（error %x）",(unsigned int)r);
    }
    ReportWord report;
    // 打印预览
    if (m_mode == Mode::PrintPreview) {
        report.open(m_filePath,true);
        report.printPreview();
        OleUninitialize();
        return;
    }
    else if (m_mode == Mode::PrintOut) {
        report.open(m_filePath,false);
        report.printOut();
        report.close();
        OleUninitialize();
        return;
    }
    report.open(m_filePath,m_visible);
    report.setBatchMarks(m_marksList,m_contentList);
    int size = m_picLabelList.size();
    if (size > m_picPathList.size()) {
        size = m_picPathList.size();
    }
    for (int index = 0; index < size; ++index) {
        report.insertPic(m_picLabelList.at(index), m_picPathList.at(index));
    }
    report.saveAs(m_savePath);
    report.close();
    OleUninitialize();
}
