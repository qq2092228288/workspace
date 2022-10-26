#ifndef REPORTWORD_H
#define REPORTWORD_H

#include <QObject>
#include <QAxObject>
#include <QDebug>

class ReportWord : public QObject
{
    Q_OBJECT
public:
    explicit ReportWord(QObject *parent = nullptr);
    virtual ~ReportWord();
public slots:
    // 打开
    bool open(const QString &path, bool visible = false);
    // 关闭
    void close();
    // 保存
    void save();
    // 另存为
    bool saveAs(const QString &saveName);
    // 设置标签内容
    bool setMarks(const QString &mark, const QString &content);
    // 批量设置
    bool setBatchMarks(QStringList &items, QStringList values);
    // 插入图片
    void insertPic(QString sLabel, const QString &picPath);
    // 打印预览
    void printPreview();
    // 打印
    void printOut();

signals:
    void opened();
    void closeEdit();
private:
    bool m_bOpened;
    QAxObject *m_pDocuments;
    QAxObject *m_pWordWidget;
};

#endif // REPORTWORD_H
