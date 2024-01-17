#include "painterconfig.h"
#include "reportset.h"
#include "reportdataname.h"
#include <QPageSize>
#include <QDateTime>

PainterConfig::PainterConfig(QPainter *painter, const int &width, const int &height)
    : m_painter{painter}
    , m_width{width}
    , m_height{height}
{
    m_font.setFamily("Microsoft YaHei");
}

PainterConfig::PainterConfig(QPainter *painter, const QSize &size)
    : PainterConfig{painter, size.width(), size.height()}
{

}

QPainter *PainterConfig::painter()
{
    return m_painter;
}

void PainterConfig::initPage()
{
    // 页面背景
    painter()->setPen(QPen(QColor(Qt::white), 1));
    painter()->setBrush(QBrush(Qt::white));
    painter()->drawRect(0, 0, m_width, m_height);
    // 重置样式
    painter()->setPen(QPen(QColor(Qt::black), 1));
    painter()->setBrush(Qt::NoBrush);
}

double PainterConfig::paintHeader(const HeaderMsg &msg)
{
    // 最小值
    constexpr double min = 0;
    // 最大值
    constexpr double max = 100;
    // 开始高度
    constexpr double sh = 2.0;
    // 标题字体高度
    constexpr double th = 1.7;
    // 标题字体框高度
    constexpr double tfh = th + 0.3;
    // 标题框大小
    const auto tsize = size(max, tfh);
    // 标题字体
    painter()->setFont(font(th, true));
    // 医院
    painter()->drawText(rect(min, sh, tsize), Qt::AlignCenter, msg.hospital);
    // 标题
    painter()->drawText(rect(min, sh + tfh, tsize), Qt::AlignCenter, msg.title);

    // 信息高度
    constexpr double ih = 1.5;
    // 水平起始起始位置
    constexpr double iw = 6.0;
    // 信息字体框高度
    constexpr double ifh = ih + 0.3;
    // 信息框间距
    constexpr double step = 19.0;
    // 信息框大小
    const auto isize = size(max, ifh);
    // 信息字体
    painter()->setFont(font(ih, true));
    // 第一行y坐标
    constexpr double ipy1 = sh + tfh + ifh;
    // 姓名
    painter()->drawText(rect(iw, ipy1, isize), QString("姓名: ") + msg.name);
    // 性别
    painter()->drawText(rect(iw + step, ipy1, isize), QString("性别: ") + msg.gender);
    // 年龄
    painter()->drawText(rect(iw + step * 2, ipy1, isize), QString("年龄: %1 岁").arg(msg.age));
    // 身高
    painter()->drawText(rect(iw + step * 3, ipy1, isize), QString("身高: %1 cm").arg(msg.height));
    // 体重
    painter()->drawText(rect(iw + step * 4, ipy1, isize), QString("体重: %1 kg").arg(msg.weight));
    // 第二行y坐标
    constexpr double ipy2 = ipy1 + ifh;
    // 体表面积
    painter()->drawText(rect(iw, ipy2, isize), QString("体表面积: %1 m²").arg(msg.bsa));
    // 科室
    painter()->drawText(rect(iw + step, ipy2, isize), QString("科室:") + msg.department);
    // 病历号
    painter()->drawText(rect(iw + step * 3, ipy2, isize), QString("病历号: ") + msg.id);
    // 页头结束位置
    constexpr double hEnd = ipy2 + ifh + 0.3;
    // 分割线
    painter()->drawLine(line(3, hEnd, 97, hEnd));

    return hEnd + 0.5;
}

void PainterConfig::paintFooter(const QDateTime &time, const QString &name)
{
    // 开始高度
    constexpr double sh = 95.0;
    // 字体高度
    constexpr double fh = 1.2;
    // 字体框高度
    constexpr double rh = fh + 0.3;
    // 水平起始起始位置
    constexpr double iw = 3.0;
    // 标题字体
    painter()->setFont(font(fh, false));
    // 分割线
    painter()->drawLine(line(3, sh, 97, sh));
    // 时间
    painter()->drawText(rect(iw, sh, 100 - iw, rh), QString("报告时间: ") + time.toString("yyyy-MM-dd hh:mm:ss"));
    // 检测人员
    painter()->drawText(rect(80, sh, 20, rh), QString("检测人员: ") + name);
    // 提示
    painter()->drawText(rect(iw, sh + rh, 100 - iw, rh),
                        QString("此报告只说明检测当时状态的血流动力学情况，请结合临床具体情况综合分析。"));
}

void PainterConfig::paintMain(const double &hEnd, const QJsonObject &object)
{
    Q_UNUSED(object);
    // 单双体位
    bool single = false;
    // 字体高度
    constexpr double fh = 1.2;
    // 列头和行头字体
    painter()->setFont(font(fh, true));
    // 列宽
    QVector<double> vectW;
    // 设置列头
    QStringList columns { QString("参数"), QString("参数描述"), QString("第一体位") };
    if (!single) {
        columns<<QString("第二体位");
        vectW<<16<<40<<9<<9<<18;
    }
    else {
        vectW<<16<<40<<13<<23;
    }
    columns<<QString("量规(单位)");
    // 起始x坐标
    constexpr double sx = 4.0;
    // 列头高度
    constexpr double ch = fh + 0.5;
    // 绘制列头
    for (int i = 0, count = columns.count(); i < count; ++i) {
        auto vect = vectW.mid(0, i);
        auto colRect = rect(sx + std::accumulate(vect.constBegin(), vect.constEnd(), 0), hEnd, size(vectW.at(i), ch));
        painter()->drawRect(colRect);
        painter()->drawText(colRect, Qt::AlignCenter, columns.at(i));
    }
    // 值字体
    painter()->setFont(font(1.0, false));
    // 行高
    constexpr double rh = 4.5;
    // 组宽
    constexpr double gw = 2.6;
    // 绘制行头
    auto array = ReportParameters::compact();
    // parameter 计数
    int pCount = 0;
    for (int i = 0; i < array.count(); ++i) {
        auto group = array.at(i).toObject();
        auto module = group.value(ReportDataName::ekey(ReportDataName::module)).toString();
        auto parameters = group.value(ReportDataName::ekey(ReportDataName::parameters)).toArray();
        // 当前组y坐标
        auto gy = hEnd + ch + pCount * rh;
        auto groupRect = rect(sx, gy, gw, rh * parameters.size());
        painter()->drawRect(groupRect);
        painter()->drawText(groupRect, Qt::AlignCenter | Qt::TextSingleLine | Qt::TextWordWrap, module);
        pCount += parameters.count();
        for (int j = 0; j < parameters.count(); ++j) {
            auto parameter = parameters.at(j).toObject();
            // 第一列(参数)偏移
            auto offset = sx;
            // 当前行y坐标
            auto ry = gy + j * rh;
            // 参数
            auto en = parameter.value(ReportDataName::ekey(ReportDataName::en)).toString();
            auto cn = parameter.value(ReportDataName::ekey(ReportDataName::cn)).toString();
            auto enRect = rect(sx + gw, ry, vectW.at(0) - gw, rh);
            painter()->drawRect(enRect);
            painter()->drawText(enRect, Qt::AlignCenter, en + "\n" + cn);
            // 第二列(参数描述)偏移
            offset += vectW.at(0);
            // 参数描述
            auto desc = parameter.value(ReportDataName::ekey(ReportDataName::desc)).toString();
            painter()->drawRect(rect(offset, ry, vectW.at(1), rh));
            painter()->drawText(rect(offset + 0.5, ry + 0.3, vectW.at(1) - 0.5, rh - 0.6),
                                Qt::AlignVCenter | Qt::TextWordWrap,
                                desc);
            // 第三列(第一体位)偏移
            offset += vectW.at(1);
            // 第一体位
            auto firstRect = rect(offset, ry, size(vectW.at(2), rh));
            painter()->drawRect(firstRect);
            // 第四列偏移
            offset += vectW.at(2);
            // 第二体位
            if (!single) {
                auto secondaryRect = rect(offset, ry, size(vectW.at(3), rh));
                painter()->drawRect(secondaryRect);
                // 第五列(量规)偏移
                offset += vectW.at(3);
            }
            // 量规
            auto gaugeRect = rect(offset, ry, vectW.last(), rh);
            painter()->drawRect(gaugeRect);
            // 量规各段宽度
            const double gaugeW = (vectW.last() - 2) / 3;
            // 量规各段大小
            const auto gaugeS = size(gaugeW, rh / 3);
            // 量规y坐标
            const double gaugeY = ry + rh / 3;
            // 范围值
            auto min = parameter.value(ReportDataName::ekey(ReportDataName::min)).toDouble();
            auto max = parameter.value(ReportDataName::ekey(ReportDataName::max)).toDouble();
            painter()->drawText(rect(offset + 1, ry, gaugeW * 2, rh / 3), Qt::AlignCenter, QString::number(min));
            painter()->drawText(rect(offset + 1 + gaugeW, ry, gaugeW * 2, rh / 3), Qt::AlignCenter, QString::number(max));
            // 单位
            auto unit = parameter.value(ReportDataName::ekey(ReportDataName::unit)).toString();
            painter()->drawText(gaugeRect, Qt::AlignHCenter | Qt::AlignBottom, QString("(%1)").arg(unit));
            // 低于
            painter()->setBrush(QBrush(Qt::yellow));
            auto lowRect = rect(offset + 1, gaugeY, gaugeS);
            painter()->drawRect(lowRect);
            // 正常
            painter()->setBrush(QBrush(Qt::green));
            auto normalRect = rect(offset + 1 + gaugeW, gaugeY, gaugeS);
            painter()->drawRect(normalRect);
            // 高于
            painter()->setBrush(QBrush(Qt::red));
            auto highRect = rect(offset + 1 + gaugeW * 2, gaugeY, gaugeS);
            painter()->drawRect(highRect);
            // 重置brush
            painter()->setBrush(Qt::NoBrush);
        }
    }
    // 提示
    painter()->drawText(point(sx, 94.5), QString("注：第一体位为基础体位；第二体位为回心血量增加。"));
}

QFont PainterConfig::font(const qreal &ps, const bool &bold)
{
    m_font.setPixelSize(shrink(ps * m_height));
    m_font.setBold(bold);
    return m_font;
}

QPointF PainterConfig::point(const qreal &px, const qreal &py) const
{
    return QPointF(shrink(px * m_width), shrink(py * m_height));
}

QSizeF PainterConfig::size(const qreal &pw, const qreal &ph) const
{
    return QSizeF(shrink(pw * m_width), shrink(ph * m_height));
}

QLineF PainterConfig::line(const qreal &px1, const qreal &py1, const qreal &px2, const qreal &py2) const
{
    return QLineF(point(px1, py1), point(px2, py2));
}

QLineF PainterConfig::line(const qreal &px1, const qreal &py1, const QPointF &p2) const
{
    return QLineF(point(px1, py1), p2);
}

QLineF PainterConfig::line(const QPointF &p1, const qreal &px2, const qreal &py2) const
{
    return QLineF(p1, point(px2, py2));
}

QRectF PainterConfig::rect(const qreal &px, const qreal &py, const qreal &pw, const qreal &ph) const
{
    return QRectF(point(px, py), size(pw, ph));
}

QRectF PainterConfig::rect(const qreal &px, const qreal &py, const QSizeF &size) const
{
    return QRectF(point(px, py), size);
}

QRectF PainterConfig::rect(const QPointF &point, const qreal &pw, const qreal &ph) const
{
    return QRectF(point, size(pw, ph));
}

qreal PainterConfig::shrink(const qreal &value) const
{
    return value / 100.0;
}
