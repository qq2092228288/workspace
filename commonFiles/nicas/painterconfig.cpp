#include "painterconfig.h"
#include "reportset.h"
#include "reportdataname.h"
#include "datacalculation.h"

#include <QPageSize>
#include <QDateTime>
#include <QPainterPath>
#include <QtMath>

PainterConfig::PainterConfig(QPainter *painter, const int &width, const int &height,
                             const QJsonObject &object, bool samepage)
    : m_painter{painter}
    , m_width{width}
    , m_height{height}
    , m_object{object}
    , m_samepage{samepage}
    , m_info{PatientInfo(object)}
    , m_place{Place(object)}
{
    auto positon = object.value(ReportDataName::ekey(ReportDataName::position)).toArray();
    auto info = object.value(ReportDataName::ekey(ReportDataName::patientInfo)).toObject();
    m_fp = Position(positon.first().toObject(), info);
    if (positon.count() > 1) {
        m_sp = Position(positon.last().toObject(), info);
    }

    m_fvect = QVector<Type>{Type::CO, Type::CI, Type::SV, Type::SI, Type::TFC, Type::EPCI, Type::ISI, Type::Ino,
                            Type::HR, Type::MAP, Type::Vol, Type::HRV, Type::Vas, Type::SBP, Type::DBP};
    m_svect = QVector<Type>{Type::CO, Type::CI, Type::SV, Type::SI, Type::TFC, Type::EPCI, Type::ISI,
                            Type::HR, Type::MAP, Type::SBP, Type::DBP};
    m_plrVect = QVector<Type>{Type::Pos, Type::HR, Type::SI, Type::CI, Type::SV, Type::CO, Type::TFC, Type::ISI};
}

PainterConfig::PainterConfig(QPainter *painter, const QSize &size, const QJsonObject &object, bool samepage)
    : PainterConfig{painter, size.width(), size.height(), object, samepage}
{

}

void PainterConfig::paintPage(const PageType &ptype)
{
    switch (ptype) {
    case PageType::MainPage_0:
    case PageType::MainPage_1:
        paintMain(ptype);
        break;
    case PageType::ImagePage:
        paintImagePage();
        break;
    default:
        break;
    }
}

void PainterConfig::paintMain(const int &page)
{
    // 初始化页面
    initPage();
    // 页眉
    const double hEnd = paintHeader(QString::fromUtf8("无创心功能监测报告"));
    // 页尾
    const double fEnd = paintFooter();
    // 单双体位
    bool single = m_sp.reportTime.isEmpty();
    // 字体高度
    constexpr double fh = 1.1;
    // 列头和行头字体
    painter()->setFont(font(fh, true));
    // 列宽
    QVector<double> vectW;
    // 设置列头
    QStringList columns { QString("参数"), QString("参数描述") };
    if (single) {   // 单体位
        columns<<QString("第一体位");
        vectW<<19<<37<<13<<23;
    }
    else {  // 双体位
        if (m_samepage) {   // 同页
            columns<<QString("第一体位")<<QString("第二体位");
            vectW<<19<<37<<9<<9<<18;
        }
        else {  // 分页
            if (0 == page) {    // 第一页
                columns<<QString("第一体位");
            }
            else {  // 第二页
                columns<<QString("第二体位");
            }
            vectW<<19<<37<<13<<23;
        }
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
    // 行高
    constexpr double rh = 4.02;
    // 组宽
    constexpr double gw = 2.6;
    // 值字体高度
    constexpr double vfh = 1.6;
    // 量规字体高度
    constexpr double gfh = 1.1;
    // 绘制行头
    auto array = ReportParameters::compact();
    // parameter 计数
    int pCount = 0;
    for (int i = 0; i < array.count(); ++i) {
        painter()->setFont(font(fh, true));
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
            painter()->setFont(font(fh, false));
            const auto parameter = parameters.at(j).toObject();
            // 参数
            const auto en = parameter.value(ReportDataName::ekey(ReportDataName::en)).toString();
            const auto cn = parameter.value(ReportDataName::ekey(ReportDataName::cn)).toString();
            // 范围值
            const auto min = parameter.value(ReportDataName::ekey(ReportDataName::min)).toDouble();
            const auto max = parameter.value(ReportDataName::ekey(ReportDataName::max)).toDouble();
            // 参数描述
            auto desc = parameter.value(ReportDataName::ekey(ReportDataName::desc)).toString();
            // 值类型
            auto type = Type(parameter.value(ReportDataName::ekey(ReportDataName::type)).toInt());
            // 单位
            auto unit = parameter.value(ReportDataName::ekey(ReportDataName::unit)).toString();
            // 当前列宽位置
            int cwIndex = 0;
            // 第一列(参数)偏移
            auto offset = sx;
            // 当前行y坐标
            auto ry = gy + j * rh;
            auto enRect = rect(sx + gw, ry, vectW.at(cwIndex) - gw, rh);
            painter()->drawRect(enRect);
            painter()->drawText(enRect, Qt::AlignCenter, en + "\n" + cn);
            // 第二列(参数描述)偏移
            offset += vectW.at(cwIndex++);
            painter()->drawRect(rect(offset, ry, vectW.at(cwIndex), rh));
            painter()->drawText(rect(offset + 0.5, ry + 0.3, vectW.at(cwIndex) - 0.5, rh - 0.6),
                                Qt::AlignVCenter | Qt::TextWordWrap,
                                desc);
            // 第三列(第一体位)偏移
            offset += vectW.at(cwIndex++);
            painter()->setFont(font(vfh, false));
            // 值列1
            auto firstRect = rect(offset, ry, size(vectW.at(cwIndex), rh));
            auto fvalue = m_fp.valueMap.value(type);
            if (0 == page) {    // 第一体位
                paintValue(firstRect, fvalue, min, max);
                paintArrow(firstRect, arrow(fvalue, min, max, type));
            }
            else {  // 第二体位
                auto svalue = m_sp.valueMap.value(type);
                paintValue(firstRect, svalue, min, max);
                paintArrow(firstRect, arrow(fvalue, svalue, type));
            }
            // 第四列偏移
            offset += vectW.at(cwIndex++);
            // 第二体位
            auto svalue = m_sp.valueMap.value(type);
            if (0 == page && !single && m_samepage) {
                auto secondaryRect = rect(offset, ry, size(vectW.at(cwIndex), rh));
                paintValue(secondaryRect, svalue, min, max);
                paintArrow(secondaryRect, arrow(fvalue, svalue, type));
                // 第五列(量规)偏移
                offset += vectW.at(cwIndex++);
            }
            // 量规
            painter()->setFont(font(gfh, false));
            auto gaugeRect = rect(offset, ry, vectW.last(), rh);
            painter()->drawRect(gaugeRect);
            // 量规各段宽度
            const double gaugeW = (vectW.last() - 2) / 3;
            // 量规各段大小
            const auto gaugeS = size(gaugeW, rh / 3);
            // 量规y坐标
            const double gaugeY = ry + rh / 3;
            // 绘制范围值
            painter()->drawText(rect(offset + 1, ry, gaugeW * 2, rh / 3), Qt::AlignCenter, QString::number(min));
            painter()->drawText(rect(offset + 1 + gaugeW, ry, gaugeW * 2, rh / 3), Qt::AlignCenter, QString::number(max));

            painter()->drawText(rect(offset, ry + rh * 2 / 3, vectW.last(), rh / 3),
                                Qt::AlignHCenter | Qt::AlignBottom,
                                QString("(%1)").arg(unit));
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
            // 符号字体
            painter()->setFont(font(gfh, true, "Simsun"));
            // 符号
            if (m_samepage && !single) {    // 双体位同页
                if (fvalue < min && svalue < min) {
                    paintSymbols(lowRect, m_fp.pos, m_sp.pos, fvalue, svalue);
                }
                else if (fvalue >= min && fvalue <= max && svalue >= min && svalue <= max) {
                    paintSymbols(normalRect, m_fp.pos, m_sp.pos, fvalue, svalue);
                }
                else if (fvalue > max && svalue > max) {
                    paintSymbols(highRect, m_fp.pos, m_sp.pos, fvalue, svalue);
                }
                else {
                    if (fvalue < min) {
                        paintSymbol(lowRect, m_fp.pos);
                    }
                    else if (fvalue > max) {
                        paintSymbol(highRect, m_fp.pos);
                    }
                    else {
                        paintSymbol(normalRect, m_fp.pos);
                    }
                    if (svalue < min) {
                        paintSymbol(lowRect, m_sp.pos);
                    }
                    else if (svalue > max) {
                        paintSymbol(highRect, m_sp.pos);
                    }
                    else {
                        paintSymbol(normalRect, m_sp.pos);
                    }
                }
            }
            else {
                if (0 == page) {
                    if (fvalue < min) {
                        paintSymbol(lowRect, m_fp.pos);
                    }
                    else if (fvalue > max) {
                        paintSymbol(highRect, m_fp.pos);
                    }
                    else {
                        paintSymbol(normalRect, m_fp.pos);
                    }
                }
                else {
                    if (svalue < min) {
                        paintSymbol(lowRect, m_sp.pos);
                    }
                    else if (svalue > max) {
                        paintSymbol(highRect, m_sp.pos);
                    }
                    else {
                        paintSymbol(normalRect, m_sp.pos);
                    }
                }
            }
            initPaint();
        }
    }
    // 分割线
    auto ly = hEnd + ch + pCount * rh + 0.5;
    painter()->drawLine(line(3, ly, 97, ly));
    // 结论字体
    painter()->setFont(font(gfh, false));
    // 结论
    auto reportConclusionList = m_object.value(ReportDataName::ekey(ReportDataName::reportConclusion))
                                    .toString().split("|");
    QString reportConclusion = reportConclusionList.first() + "\n";
    if (reportConclusionList.size() > 1) {
        reportConclusionList.removeFirst();
        if (m_samepage || single) {
            for (int i = 0; i < reportConclusionList.size(); ++i) {
                reportConclusion += QString("%1.%2\n").arg(i + 1).arg(reportConclusionList.at(i));
            }
        }
        else if (0 == page) {
            reportConclusion += QString("1.%1\n").arg(reportConclusionList.first());
        }
        else {
            reportConclusionList.removeFirst();
            for (int i = 0; i < reportConclusionList.size(); ++i) {
                reportConclusion += QString("%1.%2\n").arg(i + 1).arg(reportConclusionList.at(i));
            }
        }
    }
    auto conclusionRect = rect(sx, ly + 0.5, 100 - 2 * sx, 93 - ly);
    painter()->drawText(conclusionRect, Qt::TextWordWrap, reportConclusion);
    // 提示字体
    painter()->setFont(font(1.0, true, "Simsun"));
    // 提示
    if (m_samepage && !single) {
        painter()->drawText(point(sx, fEnd),
                            QString::fromUtf8("注：第一体位为基础体位（量规显示：%1）；第二体位为回心血量增加（量规显示：%2）。")
                            .arg(symbol(m_fp.pos), symbol(m_sp.pos)));
    }
    else if (0 == page) {
        painter()->drawText(point(sx, fEnd), QString::fromUtf8("注：第一体位为基础体位（量规显示：%1）。").arg(symbol(m_fp.pos)));
    }
    else {
        painter()->drawText(point(sx, fEnd), QString::fromUtf8("注：第二体位为回心血量增加（量规显示：%2）").arg(symbol(m_sp.pos)));
    }
}

void PainterConfig::paintImagePage()
{
    // 初始化页面
    initPage();
    // 页眉
    const double hEnd = paintHeader(QString::fromUtf8("无创心功能监测报告"));
    // 页尾
    const double fEnd = paintFooter();
    // 起始x坐标
    constexpr double sx = 4.0;
    // 波形图字体高度
    constexpr double wfh = 1.2;
    // 波形图高度
    constexpr double wrh = 10;
    // 第二列起始x坐标
    constexpr double ssx = 51;
    // 第二行起始y坐标
    const auto ssy = hEnd + wrh + 1;
    // 列宽
    constexpr double colw = 45;
    // 波形图大小
    const auto wsize = size(colw, wrh);
    painter()->setFont(font(wfh, false));
    paintWaveform(rect(sx, hEnd, wsize), m_fp.waveform, WaveformType::Impedance, true);
    paintWaveform(rect(sx, ssy, wsize), m_fp.waveform, WaveformType::Electrocardiogram, true);
    paintWaveform(rect(ssx, hEnd, wsize), m_sp.waveform, WaveformType::Impedance, false);
    paintWaveform(rect(ssx, ssy, wsize), m_sp.waveform, WaveformType::Electrocardiogram, false);
    if (m_fp.waveform.isEmpty()) {
        paintWaveform(rect(sx, hEnd, wsize), true);
        paintWaveform(rect(ssx, hEnd, wsize), false);
    }
    // 第三行起始y坐标
    const auto tsy = ssy + wrh + 1;
    // 血压靶向分析图
    auto bpRect = rect(sx, tsy, QSizeF(shrink(colw * m_width), shrink(colw * m_width)));
    paintBpChart(bpRect);
    if (m_sp.reportTime.isEmpty()) {    // 单体位
        // 风险图
        paintMaceChart(rect(ssx, tsy, bpRect.size()));
    }
    else {  // 双体位
        // 容量@泵力分析图
        auto isiSvRect = rect(ssx, tsy, colw, 15);
        paintIsiSvChart(isiSvRect);
        // 风险图
        paintMaceChart(rect(ssx, tsy + 16, QSizeF(bpRect.width(),
                            bpRect.height() - isiSvRect.height() - shrink(1 * m_height))));
        // 容量负荷试验
        auto fsy = bpRect.bottomLeft().y() / m_height * 100 + 1;
        paintClt(rect(sx, bpRect.bottomLeft().y() / m_height * 100 + 1, 100 - sx * 2, fEnd - fsy));
    }
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
    initPaint();
}

double PainterConfig::paintHeader(const QString &title)
{
    // 最小值
    constexpr double min = 0;
    // 最大值
    constexpr double max = 100;
    // 开始高度
    constexpr double sh = 3.0;
    // 标题字体高度
    constexpr double th = 1.7;
    // 标题字体框高度
    constexpr double tfh = th + 0.3;
    // 标题框大小
    const auto tsize = size(max, tfh);
    // 标题字体
    painter()->setFont(font(th, true));
    // 医院
    painter()->drawText(rect(min, sh, tsize), Qt::AlignCenter, m_place.primaryPlace);
    // 标题
    painter()->drawText(rect(min, sh + tfh, tsize), Qt::AlignCenter, title);

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
    painter()->drawText(rect(iw, ipy1, isize), QString("姓名: ") + m_info.patientName);
    // 性别
    painter()->drawText(rect(iw + step, ipy1, isize), QString("性别: ") + m_info.sex);
    // 年龄
    painter()->drawText(rect(iw + step * 2, ipy1, isize), QString("年龄: %1 岁").arg(m_info.age));
    // 身高
    painter()->drawText(rect(iw + step * 3, ipy1, isize), QString("身高: %1 cm").arg(m_info.height));
    // 体重
    painter()->drawText(rect(iw + step * 4, ipy1, isize), QString("体重: %1 kg").arg(m_info.weight));
    // 第二行y坐标
    constexpr double ipy2 = ipy1 + ifh;
    // 体表面积
    painter()->drawText(rect(iw, ipy2, isize), QString("体表面积: %1 m²").arg(m_info.bsa));
    // 科室
    painter()->drawText(rect(iw + step, ipy2, isize), QString("科室: ") + m_place.secondaryPlace);
    // 病历号
    painter()->drawText(rect(iw + step * 3, ipy2, isize), QString("病历号: ") + m_info.medicalRecordNumber);
    // 页头结束位置
    constexpr double hEnd = ipy2 + ifh + 0.3;
    // 分割线
    painter()->drawLine(line(3, hEnd, 97, hEnd));

    return hEnd + 0.5;
}

double PainterConfig::paintFooter()
{
    // 开始高度
    constexpr double sh = 94.0;
    // 字体高度
    constexpr double fh = 1.2;
    // 字体框高度
    constexpr double rh = fh + 0.3;
    // 水平起始起始位置
    constexpr double iw = 4.0;
    // 标题字体
    painter()->setFont(font(fh, false));
    // 分割线
    painter()->drawLine(line(3, sh, 97, sh));
    // 时间
    painter()->drawText(rect(iw, sh, 100 - iw, rh), m_sp.reportTime.isEmpty() ? m_fp.reportTime : m_sp.reportTime);
    // 检测人员
    painter()->drawText(rect(80, sh, 20, rh), QString("检测人员: ") + m_place.inspector);
    // 提示
    painter()->drawText(rect(iw, sh + rh, 100 - iw, rh),
                        QString("此报告只说明检测当时状态的血流动力学情况，请结合临床具体情况综合分析。"));
    return sh - 0.5;
}

void PainterConfig::paintValue(const QRectF &rectf, const qreal &value, const qreal &min, const qreal &max)
{
    if (value < min || value > max) {
        painter()->setBrush(QBrush(Qt::gray, Qt::Dense3Pattern));
    }
    painter()->drawRect(rectf);
    initPaint();
    if (DatCa::invalid() != value) {
        painter()->drawText(rectf, Qt::AlignCenter, QString::number(value));
    }
    else {
        painter()->drawText(rectf, Qt::AlignCenter, "-");
    }
}

void PainterConfig::paintArrow(const QRectF &rectf, const Arrow &arrow)
{
    switch (arrow) {
    case Arrow::Down:
        painter()->drawText(rectf, Qt::AlignVCenter | Qt::AlignRight, "↓");
        break;
    case Arrow::Up:
        painter()->drawText(rectf, Qt::AlignVCenter | Qt::AlignRight, "↑");
        break;
    default:
        break;
    }
}

void PainterConfig::paintSymbol(const QRectF &rectf, const int &pos)
{
    painter()->drawText(rectf, Qt::AlignCenter, symbol(pos));
}

void PainterConfig::paintSymbol(const QPointF &pf, const int &pos)
{
    auto psize = painter()->font().pixelSize();
    painter()->drawText(QRectF(pf.x() - psize, pf.y() - psize, psize * 2, psize * 2), Qt::AlignCenter, symbol(pos, false));
}

void PainterConfig::paintSymbols(const QRectF &rectf, const int &pos1, const int &pos2,
                                 const qreal &fvalue, const qreal &svalue)
{
    if (fvalue < svalue) {
        painter()->drawText(rectf, Qt::AlignCenter, symbol(pos1) + symbol(pos2));
    }
    else if (fvalue > svalue) {
        painter()->drawText(rectf, Qt::AlignCenter, symbol(pos2) + symbol(pos1));
    }
    else {
        paintSymbol(rectf, pos1);
        paintSymbol(rectf, pos2);
    }
}

void PainterConfig::paintWaveform(const QRectF &rectf, bool first)
{

    auto array = m_object.value(ReportDataName::ekey(ReportDataName::position)).toArray();
    auto position = array.first().toObject();
    if (!first) {
        position = array.last().toObject();
    }
    auto encoded = position.value("pDz").toString().toLatin1();
    if (encoded.isEmpty()) {
        return;
    }
    auto pRect = QRectF(rectf.topLeft().x() + 10, rectf.topLeft().y() + rectf.height() / 4,
                        rectf.width() - 20, rectf.height() * 0.6);
    painter()->drawRect(rectf);
    QPixmap pixmap;
    pixmap.loadFromData(QByteArray::fromBase64(encoded), "PNG");
    painter()->drawPixmap(pRect, pixmap, QRectF());
    auto text = first ? QString::fromUtf8(" 第一体位 ") : QString::fromUtf8(" 第二体位 ");
    painter()->drawText(rectf, text + waveformCn(WaveformType::Impedance));
}

void PainterConfig::paintWaveform(const QRectF &rectf, const QJsonArray &waveform, const WaveformType &type, bool first)
{
    if (waveform.isEmpty()) {
        return;
    }
    // 原pen
    const auto pen = painter()->pen();
    painter()->setPen(QPen(QColor(Qt::black), 1));
    painter()->drawRect(rectf);
    auto text = first ? QString::fromUtf8(" 第一体位 ") : QString::fromUtf8(" 第二体位 ");
    painter()->drawText(rectf, text + waveformCn(type));
    // 点绘制实际框
    auto pRect = QRectF(rectf.topLeft().x() + 10, rectf.topLeft().y() + rectf.height() / 4,
                        rectf.width() - 20, rectf.height() * 0.6);
    bool ok;
    QVector<int> vector;
    const int vIndex = 2 * type;
    for (int i = 0; i < waveform.count(); ++i) {
        vector<<waveform.at(i).toString().midRef(vIndex, 2).toInt(&ok, 16);
    }
    qreal min = *std::min_element(vector.constBegin(), vector.constEnd());
    qreal max = *std::max_element(vector.constBegin(), vector.constEnd());
    qreal stenLenX = pRect.width() / vector.count();
    qreal stenLenY = pRect.height() / (max - min);
    const auto fp = pRect.bottomLeft();
    QList<QPointF> points;
    for (int i = 0; i < vector.count(); ++i) {
        points.append(QPointF(fp.x() + i * stenLenX, fp.y() - (vector.at(i) - min) * stenLenY));
    }
    paintCurve(points);

    painter()->setPen(pen);
}

void PainterConfig::paintBpChart(const QRectF &rectf)
{
    painter()->drawRect(rectf);
    painter()->drawText(rectf, QString::fromUtf8(" 血压靶向分析图"));
    const auto cRect = QRectF(rectf.topLeft().x() + rectf.width() * 0.12, rectf.topLeft().y() + rectf.height() * 0.12,
                              rectf.width() * 0.76, rectf.height() * 0.76);
    // 刻度
    const qreal minX = 10, maxX = 100;
    const qreal minY = 20, maxY = 180;
    // 步长
    const qreal stepLenX = cRect.width() / (maxX - minX);
    const qreal stepLenY = cRect.height() / (maxY - minY);
    // 坐标系原点
    auto origin = cRect.bottomLeft();
    // 范围值
    auto map = ReportParameters::find(Type::MAP);
    auto si = ReportParameters::find(Type::SI);
    auto lswi = ReportParameters::find(Type::LSWI);
    auto ssvri = ReportParameters::find(Type::SSVRI);
    qreal minMap = map.value(ReportDataName::ekey(ReportDataName::min)).toDouble();
    qreal maxMap = map.value(ReportDataName::ekey(ReportDataName::max)).toDouble();
    qreal minSi = si.value(ReportDataName::ekey(ReportDataName::min)).toDouble();
    qreal maxSi = si.value(ReportDataName::ekey(ReportDataName::max)).toDouble();
    qreal minLswi = lswi.value(ReportDataName::ekey(ReportDataName::min)).toDouble();
    qreal maxLswi = lswi.value(ReportDataName::ekey(ReportDataName::max)).toDouble();
    qreal minSsvri = ssvri.value(ReportDataName::ekey(ReportDataName::min)).toDouble();
    qreal maxSsvri = ssvri.value(ReportDataName::ekey(ReportDataName::max)).toDouble();
    // MAP SI
    qreal minMapY = (minMap - minY) * stepLenY;
    qreal maxMapY = (maxMap - minY) * stepLenY;
    qreal minSiX = (minSi - minX) * stepLenX;
    qreal maxSiX = (maxSi - minX) * stepLenX;
    // SSVRI LSWI
    auto lap = m_fp.valueMap.value(Type::LAP);
    auto cvp = m_fp.valueMap.value(Type::CVP);
    QList<QPointF> minSsvriPoints;
    QList<QPointF> maxSsvriPoints;
    QList<QPointF> minLswiPoints;
    QList<QPointF> maxLswiPoints;
    for (int x = 0; x < maxX * 10; ++x) {
        qreal _x = x / 10.0;
        qreal realX = origin.x() + (_x - minX) * stepLenX;
        qreal minSsvriY = origin.y() - ((minSsvri * _x) / 80 + cvp - minY) * stepLenY;
        qreal maxSsvriY = origin.y() - ((maxSsvri * _x) / 80 + cvp - minY) * stepLenY;
        qreal minLswiY = origin.y() - (minLswi / 0.0144 / _x + lap - minY) * stepLenY;
        qreal maxLswiY = origin.y() - (maxLswi / 0.0144 / _x + lap - minY) * stepLenY;
        if (realX >= origin.x() && realX <= cRect.topRight().x()) {
            if (minSsvriY <= origin.y() && minSsvriY >= cRect.topLeft().y()) {
                minSsvriPoints.append(QPointF(realX, minSsvriY));
            }
            if (maxSsvriY <= origin.y() && maxSsvriY >= cRect.topLeft().y()) {
                maxSsvriPoints.append(QPointF(realX, maxSsvriY));
            }
            if (minLswiY <= origin.y() && minLswiY >= cRect.topLeft().y()) {
                minLswiPoints.append(QPointF(realX, minLswiY));
            }
            if (maxLswiY <= origin.y() && maxLswiY >= cRect.topLeft().y()) {
                maxLswiPoints.append(QPointF(realX, maxLswiY));
            }
        }
    }
    // 绘制正常值区域
    painter()->setPen(Qt::NoPen);
    painter()->setBrush(QBrush(Qt::green));
    paintCurve(QList<QPointF>(minLswiPoints)<<cRect.topRight());
    painter()->setBrush(QBrush(Qt::white));
    paintCurve(QList<QPointF>(maxLswiPoints)<<cRect.topRight());
    paintCurve(QList<QPointF>(minSsvriPoints)<<cRect.bottomRight());
    paintCurve(QList<QPointF>(maxSsvriPoints)<<cRect.topLeft());
    painter()->drawRect(QRectF(cRect.bottomLeft().x(), cRect.bottomLeft().y() - minMapY, cRect.width(), minMapY));
    painter()->drawRect(QRectF(cRect.topLeft().x(), cRect.topLeft().y(), cRect.width(), cRect.bottomLeft().y() - maxMapY - cRect.topLeft().y()));
    painter()->drawRect(QRectF(cRect.topLeft(), QSizeF(minSiX, cRect.height())));
    painter()->drawRect(QRectF(cRect.topLeft().x() + maxSiX, cRect.topLeft().y(), cRect.topRight().x() - cRect.topLeft().x() - maxSiX, cRect.height()));
    // 绘制曲线
    initPaint();
    painter()->drawRect(cRect);
    paintCurve(minSsvriPoints);
    paintCurve(maxSsvriPoints);
    paintCurve(minLswiPoints);
    paintCurve(maxLswiPoints);
    painter()->drawLine(QPointF(origin.x(), origin.y() - minMapY), QPointF(cRect.bottomRight().x(), origin.y() - minMapY));
    painter()->drawLine(QPointF(origin.x(), origin.y() - maxMapY), QPointF(cRect.bottomRight().x(), origin.y() - maxMapY));
    painter()->drawLine(QPointF(origin.x() + minSiX, origin.y()), QPointF(origin.x() + minSiX, cRect.topLeft().y()));
    painter()->drawLine(QPointF(origin.x() + maxSiX, origin.y()), QPointF(origin.x() + maxSiX, cRect.topLeft().y()));
    // 绘制刻度
    for (int x = minX; x <= maxX; x += 10) {
        QPoint temp(origin.x() + (x - minX) * stepLenX, origin.y());
        if (x != minX && x != maxX) {
            painter()->drawPoint(temp);
            painter()->drawPoint(temp.x(), temp.y() - 1);
        }
        painter()->drawText(temp.x(), temp.y() + painter()->font().pixelSize(), QString::number(x));
    }
    for (int y = minY; y <= maxY; y += 20) {
        QPoint temp(origin.x() + 1, origin.y() - (y - minY) * stepLenY);
        if (y != minY && y != maxY) {
            painter()->drawPoint(temp);
            painter()->drawPoint(temp.x() + 1, temp.y());
        }
        painter()->drawText(rectf.x() + painter()->font().pixelSize(), temp.y(), QString("%1").arg(y, 3, 10, QLatin1Char(' ')));
    }
    auto enLswi = lswi.value(ReportDataName::ekey(ReportDataName::en)).toString();
    auto enSsvri = ssvri.value(ReportDataName::ekey(ReportDataName::en)).toString();
    auto enMap = map.value(ReportDataName::ekey(ReportDataName::en)).toString();
    auto enSi = si.value(ReportDataName::ekey(ReportDataName::en)).toString();
    // 标签
    painter()->drawText(QPointF(minLswiPoints.first().x() - 8, minLswiPoints.first().y() - 3), enLswi);
    painter()->drawText(QPointF(maxLswiPoints.first().x() - 8, maxLswiPoints.first().y() - 3), enLswi);
    painter()->drawText(QPointF(minSsvriPoints.last().x() + 3, minSsvriPoints.last().y() - 3), enSsvri);
    painter()->drawText(QPointF(maxSsvriPoints.last().x() - 8, maxSsvriPoints.last().y() - 3), enSsvri);
    painter()->drawText(QPointF(origin.x() + 3, origin.y() - minMapY - 3), enMap);
    painter()->drawText(QPointF(origin.x() + 3, origin.y() - maxMapY - 3), enMap);
    painter()->drawText(QPointF(origin.x() + minSiX + 3, origin.y() - 3), enSi);
    painter()->drawText(QPointF(origin.x() + maxSiX + 3, origin.y() - 3), enSi);
    // 实际值
    const auto cfont = painter()->font();
    painter()->setFont(font(2, true, "Simsun"));
    // 第一体位
    painter()->setPen(QPen(QColor(Qt::blue), 1));
    paintSymbol(QPointF(origin.x() + (limits(m_fp.valueMap.value(Type::SI), minX, maxX) - minX) * stepLenX,
                        origin.y() - (limits(m_fp.valueMap.value(Type::MAP), minY, maxY) - minY) * stepLenY),
                m_fp.pos);
    // 提示信息
    painter()->setFont(font(1, true, "Simsun"));
    auto tip_x = rectf.bottomLeft().x();
    auto tip_y = rectf.bottomLeft().y() - painter()->font().pixelSize() / 2;
    painter()->setPen(QPen(QColor(Qt::black), 1));
    painter()->drawText(tip_x, tip_y, QString::fromUtf8(" 第一体位："));
    painter()->setPen(QPen(QColor(Qt::blue), 1));
    painter()->drawText(tip_x + 6 * painter()->font().pixelSize(), tip_y, symbol(m_fp.pos, false));
    if (!m_sp.reportTime.isEmpty()) {   // 第二体位
        painter()->setFont(font(2, true, "Simsun"));
        painter()->setPen(QPen(QColor(Qt::red), 1));
        paintSymbol(QPointF(origin.x() + (limits(m_sp.valueMap.value(Type::SI), minX, maxX) - minX) * stepLenX,
                            origin.y() - (limits(m_sp.valueMap.value(Type::MAP), minY, maxY) - minY) * stepLenY),
                    m_sp.pos);
        painter()->setFont(font(1, true, "Simsun"));
        painter()->setPen(QPen(QColor(Qt::black), 1));
        painter()->drawText(tip_x + 7 * painter()->font().pixelSize(), tip_y, QString::fromUtf8(" 第二体位："));
        painter()->setPen(QPen(QColor(Qt::red), 1));
        painter()->drawText(tip_x + 13 * painter()->font().pixelSize(), tip_y, symbol(m_sp.pos, false));
    }

    painter()->setFont(cfont);
    initPaint();
}

void PainterConfig::paintIsiSvChart(const QRectF &rectf)
{
    if (m_sp.reportTime.isEmpty()) {
        return;
    }
    painter()->drawRect(rectf);
    painter()->drawText(rectf, QString::fromUtf8(" 容量@泵力分析图"));

    const auto cRect = QRectF(rectf.topLeft().x() + rectf.width() * 0.15, rectf.topLeft().y() + rectf.height() * 0.15,
                              rectf.width() * 0.7, rectf.height() * 0.6);
    // 原点
    auto origin = cRect.bottomLeft();
    // y轴
    paintArrow(origin, cRect.topLeft());
    // x轴
    paintArrow(origin, cRect.bottomRight());
    // y轴标签
    painter()->drawText(QRectF(QPointF(cRect.topLeft().x() - painter()->font().pixelSize() - 3, cRect.topLeft().y()),
                               QSizeF(painter()->font().pixelSize(), cRect.height())),
                        Qt::AlignCenter | Qt::TextSingleLine | Qt::TextWordWrap, "心脏泵力");
    // x轴标签
    painter()->drawText(QRectF(cRect.bottomLeft(), cRect.size()), Qt::AlignHCenter | Qt::AlignTop, "容量");
    // // 曲线
    auto rectArc = QRectF(QPointF(origin.x(), cRect.topLeft().y() + 10),
                          QSizeF(cRect.bottomRight().x() - origin.x(), cRect.bottomRight().x() - origin.x()));
    painter()->drawArc(rectArc, 20 * 16, 140 * 16);
    // 半径
    auto r = rectArc.width() / 2;
    // 圆心
    auto center = QPointF(rectArc.topLeft().x() + r, rectArc.topLeft().y() + r);
    auto fIsi = m_fp.valueMap.value(Type::ISI);
    auto sIsi = m_sp.valueMap.value(Type::ISI);

    QPoint p1, p2;
    if (sIsi / fIsi >= 1.05) {
        p1.setY(origin.y() - 40);
        p1.setX(circleX(p1.y(), r, center.x(), center.y(), true));
        p2.setY(center.y() - r);
        p2.setX(center.x());
        paintArrow(QPointF(p1.x() + 10, p1.y() - 5), QPointF(p2.x() - 10, p2.y() + 5));
    }
    else if (fIsi == sIsi || fIsi < sIsi) {
        p1.setY(origin.y() - 60);
        p1.setX(circleX(p1.y(), r, center.x(), center.y(), true));
        p2.setY(origin.y() - 60);
        p2.setX(circleX(p2.y(), r, center.x(), center.y(), false));
        paintArrow(QPointF(p1.x() + 10, p1.y()), QPointF(p2.x() - 10, p2.y()));
    }
    else {
        p1.setY(center.y() - r);
        p1.setX(center.x());
        p2.setY(origin.y() - 40);
        p2.setX(circleX(p2.y(), r, center.x(), center.y(), false));
        paintArrow(QPointF(p1.x() + 10, p1.y() + 5), QPointF(p2.x() - 10, p2.y() - 5));
    }
    //画点
    const auto cfont = painter()->font();
    painter()->setFont(font(1.5, true, "Simsun"));
    painter()->setPen(QPen(QColor(Qt::blue), 1));
    paintSymbol(p1, m_fp.pos);
    painter()->setPen(QPen(QColor(Qt::red), 1));
    paintSymbol(p2, m_sp.pos);
    painter()->setPen(QPen(QColor(Qt::black), 1));
    painter()->setFont(cfont);
    // 评价
    QString evaluate("回心血量增加后，");
    if (fIsi < sIsi && sIsi / fIsi < 1.05) {
        evaluate += QString("心肌力增加小于5%，容量平台期");
    }
    else {
        evaluate += QString("泵力%1，搏排量%2").arg(compare(fIsi, sIsi),
                        compare(m_fp.valueMap.value(Type::SV), m_sp.valueMap.value(Type::SV)));
    }
    painter()->drawText(rectf, Qt::AlignHCenter | Qt::AlignBottom, evaluate);
    initPaint();
}

void PainterConfig::paintMaceChart(const QRectF &rectf)
{
    painter()->drawRect(rectf);
    painter()->drawText(rectf, QString::fromUtf8(" MACE风险分析图"));

    const auto cRect = QRectF(rectf.topLeft().x() + rectf.width() * 0.12, rectf.topLeft().y() + rectf.height() * 0.12,
                              rectf.width() * 0.76, rectf.height() * 0.76);
    // 原点
    auto origin = cRect.bottomLeft();
    auto topLeftRect = QRectF(cRect.topLeft().x(), cRect.topLeft().y() + cRect.height() * 0.1,
                          cRect.width() * 0.45, cRect.height() * 0.45);
    auto bottomRightRect = QRectF(topLeftRect.bottomRight(), topLeftRect.size());
    auto bottomLeftRect = QRectF(topLeftRect.bottomLeft(), topLeftRect.size());
    auto topRightRect = QRectF(topLeftRect.topRight(), topLeftRect.size());
    painter()->setBrush(QBrush(QColor(255, 165, 0)));
    painter()->drawRect(topLeftRect);
    painter()->setBrush(QBrush(QColor(255, 165, 0)));
    painter()->drawRect(bottomRightRect);
    painter()->setBrush(QBrush(Qt::red));
    painter()->drawRect(bottomLeftRect);
    initPaint();
    painter()->drawRect(topRightRect);
    paintArrow(origin, cRect.topLeft());
    paintArrow(origin, cRect.bottomRight());
    painter()->drawLine(QLineF(topLeftRect.bottomLeft(), bottomRightRect.topRight()));
    painter()->drawLine(QLineF(bottomLeftRect.bottomRight(), topRightRect.topLeft()));
    painter()->drawText(QRectF(origin, QSizeF(topLeftRect.width() * 2, topLeftRect.height())),
                        Qt::AlignHCenter | Qt::AlignTop,
                        ReportParameters::find(Type::ISI).value(ReportDataName::ekey(ReportDataName::en)).toString());
    painter()->drawText(QRectF(topLeftRect.x() - painter()->font().pixelSize() * 1.5, topLeftRect.y(),
                               painter()->font().pixelSize(), topLeftRect.height() * 2),
                        Qt::AlignVCenter | Qt::AlignLeft,
                        ReportParameters::find(Type::CI).value(ReportDataName::ekey(ReportDataName::en)).toString());
    auto fCi = m_fp.valueMap.value(Type::CI);
    auto fIsi = m_fp.valueMap.value(Type::ISI);
    constexpr qreal cCi = 2.5;
    constexpr qreal cIsi = 0.9;
    if (fCi >= cCi && fIsi < cIsi) {
        painter()->drawText(topLeftRect, Qt::AlignCenter, QString::fromUtf8("中风险"));
    }
    else if (fCi < cCi && fIsi >= cIsi) {
        painter()->drawText(bottomRightRect, Qt::AlignCenter, QString::fromUtf8("中风险"));
    }
    else if (fCi < cCi && fIsi < cIsi) {
        painter()->drawText(bottomLeftRect, Qt::AlignCenter, QString::fromUtf8("高风险"));
    }
    else {
        painter()->drawText(topRightRect, Qt::AlignCenter, QString::fromUtf8("低风险"));
    }
}

void PainterConfig::paintClt(const QRectF &rectf)
{
    if (m_sp.reportTime.isEmpty()) {
        return;
    }
    // 标题字体高度
    constexpr qreal fh = 1.7;
    painter()->setFont(font(fh, true));
    painter()->drawText(rectf, Qt::AlignHCenter | Qt::AlignTop, QString::fromUtf8(" 容量负荷试验"));
    const auto asize = QSizeF(rectf.width() / m_plrVect.count(), rectf.height() / 8);
    for (int i = 0; i < m_plrVect.count(); ++i) {
        auto rect1 = QRectF(QPointF(rectf.x() + i * asize.width(), rectf.y() + asize.height()), asize);
        auto rect2 = QRectF(QPointF(rect1.x(), rectf.y() + asize.height() * 2), asize);
        auto rect3 = QRectF(QPointF(rect1.x(), rectf.y() + asize.height() * 3), asize);
        auto rect4 = QRectF(QPointF(rect1.x(), rectf.y() + asize.height() * 4), asize);
        painter()->drawRect(rect1);
        painter()->drawRect(rect2);
        painter()->drawRect(rect3);
        painter()->drawRect(rect4);
        auto type = m_plrVect.at(i);
        painter()->setFont(font(fh, true));
        painter()->drawText(rect1, Qt::AlignCenter, ReportParameters::find(m_plrVect.at(i))
                            .value(ReportDataName::ekey(ReportDataName::en)).toString());
        painter()->setFont(font(fh, false));
        if (i != 0) {
            auto fvalue = m_fp.valueMap.value(type);
            auto svalue = m_sp.valueMap.value(type);
            auto percent = static_cast<int>((svalue - fvalue) / fvalue * 10000) / 100.0;
            painter()->drawText(rect2, Qt::AlignCenter, QString::number(fvalue));
            painter()->drawText(rect3, Qt::AlignCenter, QString::number(svalue));
            painter()->drawText(rect4, Qt::AlignCenter, QString::number(percent));
        }
        else {
            painter()->drawText(rect2, Qt::AlignCenter, QString::fromUtf8("第一体位"));
            painter()->drawText(rect3, Qt::AlignCenter, QString::fromUtf8("第二体位"));
            painter()->drawText(rect4, Qt::AlignCenter, QString::fromUtf8("%"));
        }
    }
    painter()->setFont(font(1.2, false));
    painter()->drawText(QRectF(rectf.x(), rectf.y() + asize.height() * 5, rectf.width(), asize.height() * 3),
                        "容量负荷试验测试报告的建议：\n"
                        "1.阳性的判定：容量负荷试验结束后，SV，ISI大于第一体位10%~15%。视同为容量负荷试验阳性（液体负荷试验阳性）。\n"
                        "2.阴性的判定：容量负荷试验结束后，SV，ISI小于第一体位10%。视同为容量负荷试验阴性（液体负荷试验阴性）。\n"
                        "请结合临床慎重处置液体管理。");
}

void PainterConfig::paintArrow(QPointF start, QPointF end)
{
    qreal arrowSize = 6;
    QLineF line(start, end);
    painter()->drawLine(line);

    double angle = std::atan2(-line.dy(), line.dx());
    QPointF p0 = line.p2() - QPointF(sin(angle + M_PI / 3) * arrowSize, cos(angle + M_PI / 3) * arrowSize);
    QPointF p1 = line.p2() - QPointF(sin(angle + M_PI - M_PI / 3) * arrowSize, cos(angle + M_PI - M_PI / 3) * arrowSize);
    QPolygonF arrowHead;
    arrowHead.clear();
    arrowHead << p0 << p1 << line.p2();

    painter()->setBrush(QColor(Qt::black));
    painter()->drawPolygon(arrowHead);
    initPaint();
}

void PainterConfig::paintCurve(QList<QPointF> points)
{
    if (points.isEmpty()) {
        return;
    }
    QPainterPath path(points.first());
    foreach (auto point, points) {
        path.lineTo(point);
    }
    painter()->drawPath(path);
}

void PainterConfig::initPaint()
{
    painter()->setPen(QPen(QColor(Qt::black), 1));
    painter()->setBrush(Qt::NoBrush);
}

QString PainterConfig::waveformCn(const WaveformType &type) const
{
    switch (type) {
    case WaveformType::Electrocardiogram:
        return QString::fromUtf8("心电信号(ECG)");
        break;
    case WaveformType::Differential:
        return QString::fromUtf8("胸阻抗微分图(dZ/dt)");
        break;
    case WaveformType::Impedance:
        return QString::fromUtf8("胸阻抗血流图(ΔZ)");
        break;
    case WaveformType::Resp:
        return QString::fromUtf8("(ΔZ/Δt)");
        break;
    }
    return QString();
}

QString PainterConfig::compare(qreal v1, qreal v2) const
{
    if (v1 < v2) {
        return QString::fromUtf8("增加");
    }
    else if (v1 == v2) {
        return QString::fromUtf8("不变");
    }
    return QString::fromUtf8("减少");
}

qreal PainterConfig::circleX(qreal y, qreal r, qreal ox, qreal oy, bool negative) const
{
    auto temp = qSqrt(qPow(r, 2) - qPow(y - oy, 2));
    return negative ? ox - temp : ox + temp;
}

qreal PainterConfig::limits(qreal actual, qreal min, qreal max) const
{
    if (actual < min) {
        actual = min;
    }
    else if (actual > max) {
        actual = max;
    }
    return actual;
}

QString PainterConfig::symbol(const int &pos, bool hollow) const
{
    if (hollow) {
        switch (pos) {
        case 1: // square
            return QString::fromUtf8("□");
        case 2: // circle
            return QString::fromUtf8("○");
        case 3: // triangle
            return QString::fromUtf8("△");
        default:
            break;
        }
    }
    else {
        switch (pos) {
        case 1: // square
            return QString::fromUtf8("■");
        case 2: // circle
            return QString::fromUtf8("●");
        case 3: // triangle
            return QString::fromUtf8("▲");
        default:
            break;
        }
    }

    return QString();
}

int PainterConfig::flag(const qreal &source, const qreal &target) const
{
    if (source < target) {
        return Qt::AlignVCenter | Qt::AlignLeft;
    }
    else if (source > target) {
        return Qt::AlignVCenter | Qt::AlignRight;
    }
    return Qt::AlignCenter;
}

PainterConfig::Arrow PainterConfig::arrow(const qreal &fvalue, const qreal &min, const qreal &max, const Type &type) const
{
    if (m_fvect.indexOf(type) != -1) {
        if (fvalue < min) {
            return Arrow::Down;
        }
        else if (fvalue > max) {
            return Arrow::Up;
        }
    }
    return Arrow::Null;
}

PainterConfig::Arrow PainterConfig::arrow(const qreal &fvalue, const qreal &svalue, const Type &type) const
{
    if (m_svect.indexOf(type) != -1) {
        if (fvalue > svalue) {
            return Arrow::Down;
        }
        else if (fvalue < svalue) {
            return Arrow::Up;
        }
    }
    return Arrow::Null;
}

QFont PainterConfig::font(const qreal &ps, const bool &bold, const QString &family)
{
    m_font.setFamily(family);
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
