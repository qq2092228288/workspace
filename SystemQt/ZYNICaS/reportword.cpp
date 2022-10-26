#include "reportword.h"
#include <QCoreApplication>
#include <QFileInfo>

ReportWord::ReportWord(QObject *parent) :
    QObject{parent},
    m_bOpened{false},
    m_pDocuments{nullptr}
{

}

ReportWord::~ReportWord()
{
    if(m_pWordWidget) {
        delete m_pWordWidget;
    }
    emit closeEdit();
//    qDebug()<<"~ReportWord()";
}

bool ReportWord::open(const QString &path, bool visible)
{
    // 文件不存在
    if (!QFileInfo::exists(path)) {
        return false;
    }
    //新建一个kwps应用程序
    m_pWordWidget = new QAxObject;
    bool bFlag = m_pWordWidget->setControl( "kwps.Application" );
    if(!bFlag)
    {
        // 用word打开
        bFlag = m_pWordWidget->setControl( "word.Application" );
        if(!bFlag)
        {
            return false;
        }
    }
    m_pWordWidget->setProperty("Visible", visible);
    //获取所有的工作文档
    QAxObject *document = m_pWordWidget->querySubObject("Documents");
    if(!document)
    {
        return false;
    }
    //新建一个文档
    document->dynamicCall("Add(QString)", path);
    //获取当前激活的文档
    m_pDocuments = m_pWordWidget->querySubObject("ActiveDocument");
    m_bOpened = true;
    emit opened();
    return m_bOpened;
}

void ReportWord::close()
{
    if (m_bOpened)
    {
        if(m_pDocuments) {
            m_pDocuments->dynamicCall("Close (bool)", true);//关闭文本窗口
            m_pDocuments = nullptr;
        }
        if(m_pWordWidget) {
            m_pWordWidget->dynamicCall("Quit()");//退出word
            delete m_pWordWidget;
            m_pWordWidget = nullptr;
        }
        m_bOpened = false;
    }
}

void ReportWord::save()
{
    m_pDocuments->dynamicCall("Save()");
}

bool ReportWord::saveAs(const QString &saveName)
{
    return m_pDocuments->dynamicCall("SaveAs (const QString&)", saveName).toBool();
}

bool ReportWord::setMarks(const QString &mark, const QString &content)
{
    QAxObject *bookmarkCode = nullptr;
    bookmarkCode = m_pDocuments->querySubObject("Bookmarks(QVariant)", mark);
    if (bookmarkCode) {
        bookmarkCode->dynamicCall("Select(void)");
        bookmarkCode->querySubObject("Range")->setProperty("Text", content);
        return true;
    }
    return false;
}

bool ReportWord::setBatchMarks(QStringList &items, QStringList values)
{
    QAxObject *allBookmarks = nullptr;
    allBookmarks = m_pDocuments->querySubObject("Bookmarks");
    if (allBookmarks) {
        int count = allBookmarks->property("Count").toInt();
        for (int i = count; i > 0; --i) {
            QAxObject *bookmark = allBookmarks->querySubObject("Item(QVariant)",i);
            QString name = bookmark->property("Name").toString();
            for (int j = 0; j < items.size(); ++j) {
                if (name == items.at(j)) {
                    QAxObject *curBM = m_pDocuments->querySubObject("Bookmarks(QString)",name);
                    curBM->querySubObject("Range")->setProperty("Text",values.at(j));
                    break;
                }
            }
        }
        return true;
    }
    return false;
}

void ReportWord::insertPic(QString sLabel, const QString &picPath)
{
    QAxObject *bookmark=m_pDocuments->querySubObject("Bookmarks(QVariant)",sLabel);
    // 选中标签，将图片插入到标签位置
    if(bookmark){
        bookmark->dynamicCall("Select(void)");
        QAxObject *range = bookmark->querySubObject("Range");
        QVariant tmp = range->asVariant();
        QList<QVariant>qList;
        qList<<QVariant(picPath);
        qList<<QVariant(false);
        qList<<QVariant(true);
        qList<<tmp;
        QAxObject *Inlineshapes = m_pDocuments->querySubObject("InlineShapes");
        Inlineshapes->dynamicCall("AddPicture(const QString&, QVariant, QVariant ,QVariant)",qList);
    }
}

void ReportWord::printPreview()
{
    m_pDocuments->dynamicCall("PrintPreview()");
}

void ReportWord::printOut()
{
    m_pDocuments->dynamicCall("PrintOut()");
}
