#include "reportpainter.h"
#include <QPaintEngine>
#include <QPainterPath>
#include <QDateTime>
#include <QDebug>
#include <algorithm>
#include <numeric>

#include "datacalculation.h"
#include "datamanagement.h"
#include "reportdatajson.h"

ReportPainter::ReportPainter(const ReportStruct &info, QPrinter *printer)
    : QPainter{printer},
      m_info{info},
      m_size{printer->pageRect(QPrinter::DevicePixel).size()},
      m_printer{printer}
{
    if (m_info.type == Printer_Type::Thermal) {
        thermalPage();
    }
    else {
        // 数据页
        generalDataPage();
        // PLR页
        if (m_info.mode != Check_Mode::PhysicalExamination &&
                !m_info.data.value(ekey(ReportDataName::position)).toArray()
                .at(1).toObject().value(ekey(ReportDataName::reportTime)).toString().isEmpty()) {
            m_printer->newPage();
            plrPage();
        }
    }
}

void ReportPainter::thermalPage()
{
    auto data = m_info.data;
    // 场所信息
    auto place = data.value(ekey(ReportDataName::place)).toObject();
    setFontSize(10, true);
    // 医院
    drawText(rectF(0, 10), Qt::AlignHCenter, place.value(ekey(ReportDataName::primaryPlace)).toString());
    // 标题
    drawText(rectF(0, 30), Qt::AlignHCenter, QString("无创心功能监测报告"));
    // 线
    drawLine(QPoint(0, 50), QPoint(m_size.width(), 50));
    // 个人信息
    setFontSize(12, true);
    auto info = data.value(ekey(ReportDataName::patientInfo)).toObject();
    int y = 50;
    setFontSize(8, true);
    drawText(rectF(10, y), "姓名:");
    drawText(rectF(50, y), info.value(ekey(ReportDataName::patientName)).toString());
    drawText(rectF(140, y), "性别:");
    drawText(rectF(200, y), info.value(ekey(ReportDataName::sex)).toString());
    drawText(rectF(10, y + 20), "年龄:");
    drawText(rectF(50, y + 20), info.value(ekey(ReportDataName::age)).toString() + " 岁");
    drawText(rectF(140, y + 20), "身高:");
    auto height = info.value(ekey(ReportDataName::height)).toString().toInt();
    drawText(rectF(200, y + 20), QString::number(height) + " cm");
    drawText(rectF(10, y + 40), "体重:");
    auto weight = info.value(ekey(ReportDataName::weight)).toString().toInt();
    drawText(rectF(50, y + 40), QString::number(weight) + " kg");
    drawText(rectF(140, y + 40), "体表面积:");
    drawText(rectF(200, y + 40), QString::number(DataCalculation::cBsa(height, weight), 'f', 2) + " m²");
    // 线
    drawLine(QPoint(0, 107), QPoint(m_size.width(), 107));
    // 数据
    auto position = m_info.data.value(ekey(ReportDataName::position)).toArray();
    QJsonObject fdata = position.at(0).toObject().value(ekey(ReportDataName::data)).toObject();
    QJsonObject sdata = position.at(1).toObject().value(ekey(ReportDataName::data)).toObject();
    // 第二体位数据存在
    bool secExist = !position.at(1).toObject().value(ekey(ReportDataName::reportTime)).toString().isEmpty();
    auto fMap = valueMap(fdata, QJsonArray());
    auto sMap = valueMap(sdata, QJsonArray());
    auto p = QPoint(10, 120);
    auto parameters = ReportParameters::xprinter();
    if (secExist) {
        auto s1 = QSize(84, 40);
        drawTableCell(QRectF(p, s1), "参数名称");
        auto s2 = QSize(42, 40);
        drawTableCell(QRectF(QPoint(p.x() + s1.width(), p.y()), s2),
                      positionCn(fdata.value(QString::number(Type::Pos)).toInt()));
        drawTableCell(QRectF(QPoint(p.x() + s1.width() + s2.width(), p.y()), s2),
                      positionCn(sdata.value(QString::number(Type::Pos)).toInt()));
        drawTableCell(QRectF(QPoint(p.x() + s1.width() + s2.width() * 2, p.y()), s1), "参考范围");
        setFontSize(7, true);
        for (int i = 0; i < parameters.count(); ++i) {
            auto parameter = parameters.at(i).toObject();
            auto type = Type(parameter.value(ekey(ReportDataName::type)).toInt());
            auto digit = parameter.value(ekey(ReportDataName::digit)).toInt();
            auto y = p.y() + 40 * (i + 1);
            drawTableCell(QRectF(QPoint(p.x(), y), s1), QString("%1\n%2")
                          .arg(parameter.value(ekey(ReportDataName::en)).toString(),
                               parameter.value(ekey(ReportDataName::cn)).toString()));
            drawTableCell(QRectF(QPoint(p.x() + s1.width(), y), s2),
                          QString::number(fMap.value(type), 'f', digit));
            drawTableCell(QRectF(QPoint(p.x() + s1.width() + s2.width(), y), s2),
                          QString::number(sMap.value(type), 'f', digit));
            drawTableCell(QRectF(QPoint(p.x() + s1.width() + s2.width() * 2, y), s1), QString("%1 ~ %2\n%3")
                          .arg(parameter.value(ekey(ReportDataName::min)).toDouble())
                          .arg(parameter.value(ekey(ReportDataName::max)).toDouble())
                          .arg(parameter.value(ekey(ReportDataName::unit)).toString()));
        }
    }
    else {
        auto s1 = QSize(98, 40);
        drawTableCell(QRectF(p, s1), "参数名称");
        auto s2 = QSize(56, 40);
        drawTableCell(QRectF(QPoint(p.x() + s1.width(), p.y()), s2),
                      positionCn(fdata.value(QString::number(Type::Pos)).toInt()));
        drawTableCell(QRectF(QPoint(p.x() + s1.width() + s2.width(), p.y()), s1), "参考范围");
        setFontSize(7, true);
        for (int i = 0; i < parameters.count(); ++i) {
            auto parameter = parameters.at(i).toObject();
            auto type = Type(parameter.value(ekey(ReportDataName::type)).toInt());
            auto digit = parameter.value(ekey(ReportDataName::digit)).toInt();
            auto y = p.y() + 40 * (i + 1);
            drawTableCell(QRectF(QPoint(p.x(), y), s1), QString("%1\n%2")
                          .arg(parameter.value(ekey(ReportDataName::en)).toString(),
                               parameter.value(ekey(ReportDataName::cn)).toString()));
            drawTableCell(QRectF(QPoint(p.x() + s1.width(), y), s2),
                          QString::number(fMap.value(type), 'f', digit));
            drawTableCell(QRectF(QPoint(p.x() + s1.width() + s2.width(), y), s1), QString("%1 ~ %2\n%3")
                          .arg(parameter.value(ekey(ReportDataName::min)).toDouble())
                          .arg(parameter.value(ekey(ReportDataName::max)).toDouble())
                          .arg(parameter.value(ekey(ReportDataName::unit)).toString()));
        }
    }

    auto sy = 170 + parameters.count() * 40;
    drawLine(QPoint(0, sy), QPoint(m_size.width(), sy));
    drawSudoku(QRectF(30, sy + 30, m_size.width() - 60, m_size.width() - 60),
               fMap, secExist ? sMap : QMap<Type, qreal>());
    setFontSize(6);
    drawLine(QPoint(0, sy + m_size.width()), QPoint(m_size.width(), sy + m_size.width()));
    drawText(QRectF(10, sy + m_size.width(), m_size.width() - 20, m_size.height()),
             Qt::AlignLeft | Qt::TextWordWrap,
             data.value(ekey(ReportDataName::reportConclusion)).toString());
    drawLine(QPoint(0, m_size.height() - 40), QPoint(m_size.width(), m_size.height() - 40));
    auto reportTime = position.at(1).toObject().value(ekey(ReportDataName::reportTime)).toString();
    if (reportTime.isEmpty()) {
        reportTime = position.at(0).toObject().value(ekey(ReportDataName::reportTime)).toString();
    }
    setFontSize(7);
    drawText(rectF(10, m_size.height() - 35), "报告时间: " + QDateTime::fromString(reportTime, "yyyyMMddhhmmsszzz")
             .toString("yyyy-MM-dd hh:mm:ss"));
    drawText(rectF(m_size.width() - 80, m_size.height() - 35),
             "检测人员: " + place.value(ekey(ReportDataName::inspector)).toString());
    setFontSize(5);
    drawText(rectF(10, m_size.height() - 15), "此报告只说明检测当时状态的血流动力学情况，请结合临床具体情况综合分析。");
}

void ReportPainter::generalHeader()
{
    auto &instance = DataManagement::getInstance();
    auto data = m_info.data;
    // 场所信息
    auto place = data.value(ekey(ReportDataName::place)).toObject();
    setFontSize(14, true);
    // 医院LOGO
    QPixmap pixmap(DataManagement::getInstance().getPaths().hospitalLogo());
    if (!pixmap.isNull()) {
        if (pixmap.width() > pixmap.height()) {
            drawPixmap(QRect(30, 15, 50, pixmap.height() * 50 / pixmap.width()), pixmap);
        }
        else {
            drawPixmap(QRect(30, 15, pixmap.width() * 50 / pixmap.height(), 50), pixmap);
        }
    }
    // 医院
    drawText(rectF(0, 10), Qt::AlignHCenter, place.value(ekey(ReportDataName::primaryPlace)).toString());
    // 标题
    drawText(rectF(0, 33), Qt::AlignHCenter, QString("无创心功能监测报告"));
    auto consultationPlace = place.value(ekey(ReportDataName::consultationPlace)).toString();
    drawText(rectF(0, 55), Qt::AlignHCenter, QString("会诊医院: ") + consultationPlace);
    // 个人信息
    setFontSize(12, true);
    auto info = data.value(ekey(ReportDataName::patientInfo)).toObject();
    int hvalue1 = 80;
    drawText(rectF( 50, hvalue1), "姓 名:");
    drawText(rectF(125, hvalue1), info.value(ekey(ReportDataName::patientName)).toString());
    drawText(rectF(250, hvalue1), "性 别:");
    drawText(rectF(300, hvalue1), info.value(ekey(ReportDataName::sex)).toString());
    drawText(rectF(375, hvalue1), "年 龄:");
    drawText(rectF(425, hvalue1), info.value(ekey(ReportDataName::age)).toString() + " 岁");
    drawText(rectF(500, hvalue1), "身 高:");
    auto height = info.value(ekey(ReportDataName::height)).toString().toInt();
    drawText(rectF(555, hvalue1), QString::number(height) + " cm");
    drawText(rectF(625, hvalue1), "体 重:");
    auto weight = info.value(ekey(ReportDataName::weight)).toString().toInt();
    drawText(rectF(675, hvalue1), QString::number(weight) + " kg");
    int hvalue2 = 100;
    drawText(rectF( 50, hvalue2), "体表面积:");
    drawText(rectF(125, hvalue2), QString::number(DataCalculation::cBsa(height, weight), 'f', 2) + " m²");
    QString departmentName = instance.departmentName();
    if (departmentName.size() == 2) {
        departmentName.insert(1, ' ');
    }
    drawText(rectF(250, hvalue2), departmentName + ":");
    drawText(rectF(325, hvalue2), place.value(ekey(ReportDataName::secondaryPlace)).toString());
    drawText(rectF(500, hvalue2), instance.idName() + ":");
    drawText(rectF(560, hvalue2), info.value(ekey(ReportDataName::medicalRecordNumber)).toString());

    if (m_info.mode == Check_Mode::Hypertension) {
        int hvalue3 = 120;
        auto inquiry = data.value(ekey(ReportDataName::inquiry)).toObject();
        drawText(rectF( 50, hvalue3), "高血压家族史: " +
                 getConsultation(inquiry.value(ekey(ReportDataName::fhh)).toInt()));
        drawText(rectF(250, hvalue3), "过量饮酒史: " +
                 getConsultation(inquiry.value(ekey(ReportDataName::edh)).toInt()));
        drawText(rectF(500, hvalue3), "长期吸烟史: " +
                 getConsultation(inquiry.value(ekey(ReportDataName::ltsh)).toInt()));
        int hvalue4 = 140;
        drawText(rectF( 50, hvalue4), "长期精神紧张史: " +
                 getConsultation(inquiry.value(ekey(ReportDataName::lthms)).toInt()));
        drawText(rectF(250, hvalue4), "坚持服药: " +
                 getConsultation(inquiry.value(ekey(ReportDataName::ptm)).toInt()));
        drawText(rectF(500, hvalue4), "活动受限: " +
                 getConsultation(inquiry.value(ekey(ReportDataName::al)).toInt()));
    }

    drawLine(QPointF(30, 162), QPointF(m_size.width() - 30, 162));
}

void ReportPainter::generalDataPage(int page)
{
    setPen(QPen(QColor(Qt::black), 1));
    setBrush(Qt::NoBrush);
    // 页眉
    generalHeader();
    // 页脚
    generalFooter();
    // 数据
    auto position = m_info.data.value(ekey(ReportDataName::position)).toArray();
    QJsonObject fdata = position.at(0).toObject().value(ekey(ReportDataName::data)).toObject();
    QJsonObject sdata = position.at(1).toObject().value(ekey(ReportDataName::data)).toObject();
    // 第二体位数据存在
    bool secExist = !position.at(1).toObject().value(ekey(ReportDataName::reportTime)).toString().isEmpty();
    ReportTableSet set(m_info.paging || !secExist);
    // 行高
    qreal row_h = set.rowHeight(m_info.mode);
    setFontSize(8, true);
    auto rectArg = drawTableCell(QRectF(set.start(), QSizeF(set.colWidth().at(0), row_h)),
                                 "参数名称");
    auto rectFirst = drawTableCell(QRectF(rectArg.topRight(), QSizeF(set.colWidth().at(1), row_h)),
                                   positionCn(QJsonObject(0 == page ? fdata : sdata)
                                              .value(QString::number(Type::Pos)).toInt()) + "体位");
    auto rectSecond = rectFirst;
    // 两个体位数据合一页显示
    bool single = secExist && !m_info.paging;
    if (single) {
        rectSecond = drawTableCell(QRectF(rectFirst.topRight(), QSizeF(set.colWidth().at(2), row_h)),
                                   positionCn(sdata.value(QString::number(Type::Pos)).toInt()) + "体位");
    }
    auto rectRange = drawTableCell(QRectF(rectSecond.topRight(), QSizeF(set.colWidth().at(2 + single), row_h)),
                                   "参考范围");
    auto rectUnit = drawTableCell(QRectF(rectRange.topRight(), QSizeF(set.colWidth().at(3 + single), row_h)),
                                  "单位");
    auto fMap = valueMap(fdata, position.at(0).toObject().value(ekey(ReportDataName::allData)).toArray());
    auto sMap = valueMap(sdata, position.at(1).toObject().value(ekey(ReportDataName::allData)).toArray());
    auto array = ReportParameters::array(m_info.mode);
    for (int i = 0; i < array.count(); ++i) {
        auto group = array.at(i).toObject();
        auto parameters = group.value(ekey(ReportDataName::parameters)).toArray();
        setFontSize(8);
        for (int j = 0; j < parameters.count(); ++j) {
            auto parameter = parameters.at(j).toObject();
            qreal _y = rectArg.bottomLeft().y() + row_h * j;
            auto min = parameter.value(ekey(ReportDataName::min)).toDouble();
            auto max = parameter.value(ekey(ReportDataName::max)).toDouble();
            // 参数名称
            drawTableCell(QRectF(rectArg.bottomLeft().x() + set.groupWidth(), _y,
                                 set.colWidth().at(0) - set.groupWidth(), row_h),
                          parameter.value(ekey(ReportDataName::cn)).toString() + "/" +
                          parameter.value(ekey(ReportDataName::en)).toString(),
                          Qt::AlignLeft | Qt::AlignVCenter, 10);
            // 值
            drawValue(parameter, (0 == page ? fMap : sMap), rectFirst, _y);
            if (single) {
                drawValue(parameter, sMap, rectSecond, _y);
            }
            // 正常范围
            drawTableCell(QRectF(QPointF(rectRange.topLeft().x(), _y), rectRange.size()),
                          min != max ? QString("%1 ~ %2").arg(min).arg(max) : "-",
                          Qt::AlignLeft | Qt::AlignVCenter, 10);
            // 单位
            drawTableCell(QRectF(QPointF(rectUnit.topLeft().x(), _y), rectUnit.size()),
                          parameter.value(ekey(ReportDataName::unit)).toString(),
                          Qt::AlignLeft | Qt::AlignVCenter, 10);
        }
        setFontSize(8, true);
        rectArg = drawTableCell(QRectF(rectArg.bottomLeft(), QSizeF(set.groupWidth(), row_h * parameters.count())),
                      group.value(ekey(ReportDataName::module)).toString(),
                      Qt::AlignCenter | Qt::TextSingleLine | Qt::TextWordWrap);
    }
    // 心阻抗图
    drawWaveform(QRectF(rectUnit.topRight().x() + 15, rectUnit.topRight().y(), 748 - rectUnit.topRight().x(), 100),
                 QJsonObject(0 == page ? fdata : sdata).value(QString::number(Type::Pos)).toInt(),
                 position.at(page).toObject().value(ekey(ReportDataName::waveform)).toArray(), page);
    if (secExist && !m_info.paging) {
        drawWaveform(QRectF(rectUnit.topRight().x() + 15, rectUnit.topRight().y() + 110,
                            748 - rectUnit.topRight().x(), 100),
                     sdata.value(QString::number(Type::Pos)).toInt(),
                     position.at(1).toObject().value(ekey(ReportDataName::waveform)).toArray(), 1);
    }
    // 血压靶向分析图
    drawSudoku(QRectF(rectUnit.topRight().x() + 23, 400, 720 - rectUnit.topRight().x(), 720 - rectUnit.topRight().x()),
               0 == page ? fMap : sMap, secExist && !m_info.paging ? sMap : QMap<Type, qreal>());
    // 分隔线
    drawLine(QPointF(rectArg.bottomLeft().x(), rectArg.bottomLeft().y() + 10),
             QPointF(m_size.width() - set.start().x(), rectArg.bottomLeft().y() + 10));
    // 第二页
    if (secExist && m_info.paging && 0 == page) { // 双体位 分页 当前为第一页
        m_printer->newPage();
        generalDataPage(1);
    }
    if ((!m_info.paging || 1 == page) && secExist) {
        drawIsiAndSv(QRectF(rectUnit.topRight().x() + 8, 660, 720 - rectUnit.topRight().x(), 100),
                     fMap.value(Type::ISI), fMap.value(Type::SV), sMap.value(Type::ISI), sMap.value(Type::SV));
    }
    if (!m_info.paging || 1 == page || !secExist) {  // 不分页或者第二页
        // 结论
        setFontSize(8);
        drawText(QRectF(set.start().x(), rectArg.bottomLeft().y() + 10,
                        m_size.width() - set.start().x() * 2, m_size.height() - rectArg.bottomLeft().y() - 10),
                 Qt::AlignLeft | Qt::TextWordWrap,
                 m_info.data.value(ekey(ReportDataName::reportConclusion)).toString());
    }
}

void ReportPainter::plrPage()
{
    // 页眉
    generalHeader();
    // 页脚
    generalFooter();
    // 被动抬腿试验
    setFontSize(16, true);
    drawText(rectF(0, 170), Qt::AlignHCenter, "被动抬腿试验");
    // 数据
    auto position = m_info.data.value(ekey(ReportDataName::position)).toArray();
    auto fMap = valueMap(position.at(0).toObject().value(ekey(ReportDataName::data)).toObject(),
                         position.at(0).toObject().value(ekey(ReportDataName::allData)).toArray());
    auto sMap = valueMap(position.at(1).toObject().value(ekey(ReportDataName::data)).toObject(),
                         position.at(1).toObject().value(ekey(ReportDataName::allData)).toArray());
    auto size = QSizeF(91.625, 50);
    QList<Type> types;
    types<<Type::Pos<<Type::HR<<Type::SI<<Type::CI<<Type::SV<<Type::CO<<Type::TFC<<Type::ISI;
    for (int i = 0; i < types.count(); ++i) {
        auto type = types.at(i);
        auto parameter = ReportParameters::find(type);
        qreal x = 30 + size.width() * i;
        qreal y = 200;

        if (0 == i) {
            setFontSize(11);
            drawTableCell(QRectF(QPointF(x, y), size), "");
            drawTableCell(QRectF(QPointF(x, y + size.height()), size), "");
            drawPixmap(QRect(x, y + size.height(), size.width(), size.height()),
                       QPixmap(getPicFileName(fMap.value(type))));
            drawTableCell(QRectF(QPointF(x, y + size.height() * 2), size), "");
            drawPixmap(QRect(x, y + size.height() * 2, size.width(), size.height()),
                       QPixmap(getPicFileName(sMap.value(type))));
            drawTableCell(QRectF(QPointF(x, y + size.height() * 3), size), "%");
        }
        else {
            auto digit = parameter.value(ekey(ReportDataName::digit)).toInt();
            auto fval = actualValue(fMap.value(type), digit);
            auto sval = actualValue(sMap.value(type), digit);
            setFontSize(11, true);
            drawTableCell(QRectF(QPointF(x, y), size), parameter.value(ekey(ReportDataName::en)).toString());
            setFontSize(11);
            drawTableCell(QRectF(QPointF(x, y + size.height()), size), QString::number(fval, 'f', digit));
            drawTableCell(QRectF(QPointF(x, y + size.height() * 2), size), QString::number(sval, 'f', digit));
            drawTableCell(QRectF(QPointF(x, y + size.height() * 3), size),
                          QString::number(actualValue((sval - fval) / fval * 100, 2)));
        }

    }
    setFontSize(8);
    drawText(rectF(30, 410),
             "被动抬腿试验测试报告的建议：\n"
             "1.阳性的判定：被动抬腿试验结束后，SV，SI，CO，CI大于第一体位10%~15%。视同为被动抬腿试验阳性（液体负荷试验阳性）。\n"
             "2.阴性的判定：被动抬腿试验结束后，SV，SI，CO，CI小于第一体位10%。视同为被动抬腿试验阴性（液体负荷试验阴性）。\n"
             "请结合临床慎重处置液体管理问题。");
    auto allData = position.at(0).toObject().value(ekey(ReportDataName::allData)).toArray();
    auto sAllData = position.at(1).toObject().value(ekey(ReportDataName::allData)).toArray();
    if (allData.isEmpty()) {
        return;
    }
    // 心率变异性分析
    setFontSize(16, true);
    drawText(rectF(0, 600), Qt::AlignHCenter, "心率变异性分析");
    setFontSize(11);
    foreach (auto data, sAllData) {
        allData<<data;
    }
    auto allHr = ReportDataJson::allValue(Type::HR, allData);
    QStringList hrAnalysis;
    hrAnalysis<<QString("采集数量（个）")
        <<QString::number(allHr.count())
        <<QString("最高HR（次/分）")
        <<QString::number(*std::max_element(allHr.constBegin(), allHr.constEnd()))
        <<QString("平均HR（次/分）")
        <<QString::number(std::accumulate(allHr.constBegin(), allHr.constEnd(), 0) / allHr.count())
        <<QString("最低HR（次/分）")
        <<QString::number(*std::min_element(allHr.constBegin(), allHr.constEnd()));
    auto pHr = QPointF(30, 630);
    auto sHr = QSizeF(183.25, 30);
    for (int i = 0; i < hrAnalysis.count(); ++i) {
        drawTableCell(QRectF(QPointF(pHr.x() + sHr.width() * (i%4), pHr.y() + sHr.height() * static_cast<int>(i / 4)),
                             sHr), hrAnalysis.at(i));
    }
    QStringList hrvAnalysis;
    hrvAnalysis<<QString("检测参数")<<QString("测量值")<<QString("范围")
        <<QString("NNVGR(ms)")<<QString::number(static_cast<int>(DatCa::cNnvgr(allHr)))<<QString("-")
        <<QString("SDNN(ms)")<<QString::number(static_cast<int>(DatCa::cSdnn(allHr)))<<QString("102~180")
        <<QString("PNN50(%)")<<QString::number(static_cast<int>(DatCa::cPnn50(allHr) * 10) / 10.0)<<QString("4.4~29")
        <<QString("RMSSD(ms)")<<QString::number(static_cast<int>(DatCa::cRmssd(allHr)))<<QString("24~54");
    auto pHrv = QPointF(30, 700);
    auto sHrv = QSizeF(244.33, 30);
    for (int i = 0; i < hrvAnalysis.count(); ++i) {
        if (i < 3) {
            setFontSize(11, true);
        }
        else {
            setFontSize(11);
        }
        drawTableCell(QRectF(QPointF(pHrv.x() + sHrv.width() * (i%3), pHrv.y() + sHrv.height() * static_cast<int>(i / 3)),
                             sHrv), hrvAnalysis.at(i));
    }
    setFontSize(8);
    drawText(rectF(30, 850),
             "说明：\n"
             "1.心血管疾病患者（房颤、早搏、高血压、糖尿病等）心率变异性数据通常会偏高，高于正常值范围；\n"
             "2.同年龄段，女性心率变异性数据普遍略高于男性；\n"
             "3.运动人群心率变异值高于普通人群；\n"
             "4.心率变异性会随着年龄的增长而逐渐下降；\n"
             "5.测量姿势不正确，心电图存在干扰，都会影响心率变异性的准确性。\n");
}

void ReportPainter::generalFooter()
{
    drawLine(QPointF(30, 1050), QPointF(m_size.width() - 30, 1050));
    auto position = m_info.data.value(ekey(ReportDataName::position)).toArray();
    auto reportTime = position.at(1).toObject().value(ekey(ReportDataName::reportTime)).toString();
    if (reportTime.isEmpty()) {
        reportTime = position.at(0).toObject().value(ekey(ReportDataName::reportTime)).toString();
    }
    setFontSize(8);
    auto time = QDateTime::fromString(reportTime, "yyyyMMddhhmmsszzz");
    if (!time.isValid()) {
        time = QDateTime::fromString(reportTime, "yyyyMMddhhmmss");
    }
    drawText(rectF(35, 1055), "报告时间: " + time.toString("yyyy-MM-dd hh:mm:ss"));
    auto place = m_info.data.value(ekey(ReportDataName::place)).toObject();
    drawText(rectF(640, 1055), "检测人员:  " + place.value(ekey(ReportDataName::inspector)).toString());
    drawText(rectF(35, 1075), "此报告只说明检测当时状态的血流动力学情况，请结合临床具体情况综合分析。");
    // 签名
    auto const encoded = m_info.data.value(ekey(ReportDataName::signature)).toString().toLatin1();
    if (!encoded.isEmpty()) {
        QPixmap pixmap;
        pixmap.loadFromData(QByteArray::fromBase64(encoded), "PNG");
        drawText(rectF(640, 1085), "会诊医生:");
        drawPixmap(QRect(690, 1080, 50, 25), pixmap);
    }
}

QString ReportPainter::getConsultation(int value)
{
    switch (value) {
    case 1:
        return QString("是");
    case 2:
        return QString("否");
    case 3:
        return QString("不详");
    default:
        break;
    }
    return QString();
}

void ReportPainter::setFontSize(int pointSize, bool bold)
{
    auto font = QFont("Microsoft YaHei", pointSize);
    font.setBold(bold);
    setFont(font);
}

QRectF ReportPainter::drawTableCell(QRectF rect, const QString &txt, int align, int offset)
{
    drawRect(rect);
    drawText(QRectF(rect.topLeft().x() + offset, rect.topLeft().y(),
                    rect.width() - offset, rect.height()), align, txt);
    return rect;
}

QMap<Type, qreal> ReportPainter::valueMap(const QJsonObject &data, const QJsonArray &alldata)
{
    return ReportDataJson::valueMap(m_info.data.value(ekey(ReportDataName::patientInfo)).toObject(), data, alldata);
}

void ReportPainter::drawValue(const QJsonObject &parameter, QMap<Type, qreal> map, QRectF rect, qreal _y)
{
    auto value = actualValue(map.value(Type(parameter.value(ekey(ReportDataName::type)).toInt())),
                             parameter.value(ekey(ReportDataName::digit)).toInt());
    drawTableCell(QRectF(QPointF(rect.topLeft().x(), _y), rect.size()),
                  DatCa::invalid() != value ? QString::number(value) : "-",
                  Qt::AlignLeft | Qt::AlignVCenter, 10);
    auto min = parameter.value(ekey(ReportDataName::min)).toDouble();
    auto max = parameter.value(ekey(ReportDataName::max)).toDouble();
    if (DatCa::invalid() != value) {
        if (min != max) {
            if (value < min) {
                drawText(QRectF(QPointF(rect.topLeft().x() - 3, _y), rect.size()),
                         Qt::AlignVCenter | Qt::AlignRight, " ↓");
            }
            else if (value > max) {
                drawText(QRectF(QPointF(rect.topLeft().x() - 3, _y), rect.size()),
                         Qt::AlignVCenter | Qt::AlignRight, " ↑");
            }
        }
    }
}

QString ReportPainter::positionCn(int pos)
{
    if (1 == pos)      return QString("半卧");
    else if (2 == pos) return QString("平躺");
    else if (3 == pos) return QString("抬腿");
    return QString();
}

void ReportPainter::drawWaveform(QRectF rect, int pos, const QJsonArray &waveform, bool index)
{
    if (waveform.isEmpty()) {
        auto const encoded = m_info.data.value(ekey(ReportDataName::position)).toArray()
                .at(index).toObject().value("pDz").toString().toLatin1();
        QPixmap pixmap;
        pixmap.loadFromData(QByteArray::fromBase64(encoded), "PNG");
        drawPixmap(QRect(rect.x(), rect.y() + 5, rect.width(), rect.height() - 5), pixmap);
    }
    setFontSize(6);
    drawRect(rect);
    drawText(rect.topLeft().x() + 2, rect.topLeft().y() + 10, positionCn(pos) + QString(" 心阻抗图(dZ)"));
    if (waveform.isEmpty()) {
        return;
    }
    bool ok;
    QVector<int> vector;
    for (int i = 0; i < waveform.count(); ++i) {
        vector<<waveform.at(i).toString().midRef(4, 2).toInt(&ok, 16);
    }
    qreal min = *std::min_element(vector.constBegin(), vector.constEnd());
    qreal max = *std::max_element(vector.constBegin(), vector.constEnd());
    qreal stenLenX = (rect.width() - 10) / vector.count();
    qreal stenLenY = rect.height() / (max - min) / 2;
    auto fp = QPointF(rect.bottomLeft().x() + 5, rect.bottomLeft().y() - rect.height() / 4);
    QList<QPointF> points;
    for (int i = 0; i < vector.count(); ++i) {
        points.append(QPointF(fp.x() + i * stenLenX, fp.y() - (vector.at(i) - min) * stenLenY));
    }
    setPen(QPen(QColor(Qt::blue), 1));
    drawCurve(points);
    setPen(QPen(QColor(Qt::black), 1));
}

void ReportPainter::drawSudoku(QRectF rect, QMap<Type, qreal> fMap, QMap<Type, qreal> sMap)
{
    // 字体
    setFontSize(6);
    // 标题
    drawText(rect.topLeft().x() - 15, rect.topLeft().y() - 15, "血压靶向分析图");
    // 正常范围颜色
    setBrush(QBrush(Qt::green));
    // 边框
    drawRect(rect);
    // 刻度
    const qreal minX = 10, maxX = 100;
    const qreal minY = 20, maxY = 180;
    // 步长
    const qreal stepLenX = rect.width() / (maxX - minX);
    const qreal stepLenY = rect.height() / (maxY - minY);
    // O点
    auto origin = rect.bottomLeft();
    // 范围值
    auto map = ReportParameters::find(Type::MAP);
    auto si = ReportParameters::find(Type::SI);
    auto lswi = ReportParameters::find(Type::LSWI);
    auto ssvri = ReportParameters::find(Type::SSVRI);
    qreal minMap = map.value(ekey(ReportDataName::min)).toDouble();
    qreal maxMap = map.value(ekey(ReportDataName::max)).toDouble();
    qreal minSi = si.value(ekey(ReportDataName::min)).toDouble();
    qreal maxSi = si.value(ekey(ReportDataName::max)).toDouble();
    qreal minLswi = lswi.value(ekey(ReportDataName::min)).toDouble();
    qreal maxLswi = lswi.value(ekey(ReportDataName::max)).toDouble();
    qreal minSsvri = ssvri.value(ekey(ReportDataName::min)).toDouble();
    qreal maxSsvri = ssvri.value(ekey(ReportDataName::max)).toDouble();
    // MAP SI
    qreal minMapY = (minMap - minY) * stepLenY;
    qreal maxMapY = (maxMap - minY) * stepLenY;
    qreal minSiX = (minSi - minX) * stepLenX;
    qreal maxSiX = (maxSi - minX) * stepLenX;
    // SSVRI LSWI
    auto lap = fMap.value(Type::LAP);
    auto cvp = fMap.value(Type::CVP);
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
        if (realX >= origin.x() && realX <= rect.topRight().x() && minSsvriY <= origin.y() && minSsvriY >= rect.topLeft().y()) {
            minSsvriPoints.append(QPointF(realX, minSsvriY));
        }
        if (realX >= origin.x() && realX <= rect.topRight().x() && maxSsvriY <= origin.y() && maxSsvriY >= rect.topLeft().y()) {
            maxSsvriPoints.append(QPointF(realX, maxSsvriY));
        }
        if (realX >= origin.x() && realX <= rect.topRight().x() && minLswiY <= origin.y() && minLswiY >= rect.topLeft().y()) {
            minLswiPoints.append(QPointF(realX, minLswiY));
        }
        if (realX >= origin.x() && realX <= rect.topRight().x() && maxLswiY <= origin.y() && maxLswiY >= rect.topLeft().y()) {
            maxLswiPoints.append(QPointF(realX, maxLswiY));
        }
    }
    // 喷漆
    setBrush(QBrush(Qt::white));
    drawCurve(QList<QPointF>(minLswiPoints)<<rect.bottomLeft());
    drawCurve(QList<QPointF>(maxLswiPoints)<<rect.topRight());
    drawCurve(QList<QPointF>(minSsvriPoints)<<rect.bottomRight());
    drawCurve(QList<QPointF>(maxSsvriPoints)<<rect.topLeft());
    drawRect(QRectF(rect.bottomLeft().x(), rect.bottomLeft().y() - minMapY, rect.width(), minMapY));
    drawRect(QRectF(rect.topLeft().x(), rect.topLeft().y(), rect.width(), rect.bottomLeft().y() - maxMapY - rect.topLeft().y()));
    drawRect(QRectF(rect.topLeft(), QSizeF(minSiX, rect.height())));
    drawRect(QRectF(rect.topLeft().x() + maxSiX, rect.topLeft().y(), rect.topRight().x() - rect.topLeft().x() - maxSiX, rect.height()));
    // 删除喷漆
    setBrush(Qt::NoBrush);
    // 绘制刻度
    for (int x = minX; x <= maxX; x += 10) {
        QPoint temp(origin.x() + (x - minX) * stepLenX, origin.y() + 9);
        drawPoint(QPointF(temp.x(), origin.y() - 1));
        drawText(temp, QString::number(x));
    }
    for (int y = minY; y <= maxY; y += 20) {
        QPoint temp(origin.x() - 17, origin.y() - (y - minY) * stepLenY);
        drawPoint(QPointF(origin.x() + 1, temp.y()));
        drawText(temp, QString("%1").arg(y, 3, 10, QLatin1Char(' ')));
    }
    // 标签
    drawText(QPointF(minLswiPoints.first().x() - 8, minLswiPoints.first().y() - 3), "LSWI");
    drawText(QPointF(maxLswiPoints.first().x() - 8, maxLswiPoints.first().y() - 3), "LSWI");
    drawText(QPointF(minSsvriPoints.last().x() + 3, minSsvriPoints.last().y() - 3), "SSVRI");
    drawText(QPointF(maxSsvriPoints.last().x() - 8, maxSsvriPoints.last().y() - 3), "SSVRI");
    drawText(QPointF(origin.x() + 3, origin.y() - minMapY - 3), "MAP");
    drawText(QPointF(origin.x() + 3, origin.y() - maxMapY - 3), "MAP");
    drawText(QPointF(origin.x() + minSiX + 3, origin.y() - 3), "SI");
    drawText(QPointF(origin.x() + maxSiX + 3, origin.y() - 3), "SI");
    // 曲线
    drawCurve(minLswiPoints);
    drawCurve(maxLswiPoints);
    drawCurve(minSsvriPoints);
    drawCurve(maxSsvriPoints);
    drawLine(QPointF(origin.x(), origin.y() - minMapY), QPointF(rect.bottomRight().x(), origin.y() - minMapY));
    drawLine(QPointF(origin.x(), origin.y() - maxMapY), QPointF(rect.bottomRight().x(), origin.y() - maxMapY));
    drawLine(QPointF(origin.x() + minSiX, origin.y()), QPointF(origin.x() + minSiX, rect.topLeft().y()));
    drawLine(QPointF(origin.x() + maxSiX, origin.y()), QPointF(origin.x() + maxSiX, rect.topLeft().y()));
    // 实际值
    auto fasi = limits(fMap.value(Type::SI), minX, maxX);
    auto famap = limits(fMap.value(Type::MAP), minY, maxY);
    positionSymbol(fMap.value(Type::Pos),
                   QPoint(origin.x() + (fasi - minX) * stepLenX, origin.y() - (famap - minY) * stepLenY),
                   Qt::blue);
    positionText(fMap.value(Type::Pos), QPoint(origin.x(), origin.y() + 20), Qt::blue);
    if (!sMap.isEmpty()) {
        auto sasi = limits(sMap.value(Type::SI), minX, maxX);
        auto samap = limits(sMap.value(Type::MAP), minY, maxY);
        positionSymbol(sMap.value(Type::Pos),
                       QPoint(origin.x() + (sasi - minX) * stepLenX, origin.y() - (samap - minY) * stepLenY),
                       Qt::red);
        positionText(sMap.value(Type::Pos), QPoint(origin.x() + 40, origin.y() + 20), Qt::red);
    }
}

void ReportPainter::drawIsiAndSv(QRectF rect, qreal fIsi, qreal fSv, qreal sIsi, qreal sSv)
{
    // 字体
    setFontSize(6);
    // 标签
    drawText(rect.topLeft(), "容量@泵力分析图");
    // 原点
    auto origin = QPointF(rect.bottomLeft().x() + 10, rect.bottomLeft().y() - 10);
    // y轴
    drawArrow(origin, QPointF(origin.x(), rect.topLeft().y() + 10));
    // x轴
    drawArrow(origin, QPointF(rect.bottomRight().x(), origin.y()));
    // y轴标签
    drawText(QRectF(rect.topLeft(), QSizeF(10, rect.height())),
             Qt::AlignCenter | Qt::TextSingleLine | Qt::TextWordWrap,
             "心脏泵力");
    // x轴标签
    drawText(QRectF(origin, QSizeF(rect.width(), 10)), Qt::AlignCenter, "容量");
    // 评价
    drawText(QRectF(QPointF(origin.x(), origin.y() + 10), QSizeF(rect.width(), 10)), Qt::AlignCenter,
             QString("回心血量增加后，泵力%1，搏排量%2").arg(compare(fIsi, sIsi), compare(fSv, sSv)));
    // 曲线
    auto rectArc = QRectF(QPointF(origin.x(), rect.topLeft().y() + 10),
                       QSizeF(rect.bottomRight().x() - origin.x(), rect.bottomRight().x() - origin.x()));
    drawArc(rectArc, 20 * 16, 140 * 16);
    // 半径
    auto r = rectArc.width() / 2;
    // 圆心
    auto center = QPointF(rectArc.topLeft().x() + r, rectArc.topLeft().y() + r);
    QPointF p1, p2;
    if (fIsi < sIsi) {
        p1.setY(origin.y() - 40);
        p1.setX(circleX(p1.y(), r, center.x(), center.y(), true));
        p2.setY(center.y() - r);
        p2.setX(center.x());
        drawArrow(QPointF(p1.x() + 5, p1.y() - 2), QPointF(p2.x() - 5, p2.y() + 2));
    }
    else if (fIsi == sIsi) {
        p1.setY(origin.y() - 60);
        p1.setX(circleX(p1.y(), r, center.x(), center.y(), true));
        p2.setY(origin.y() - 60);
        p2.setX(circleX(p2.y(), r, center.x(), center.y(), false));
        drawArrow(QPointF(p1.x() + 5, p1.y()), QPointF(p2.x() - 5, p2.y()));
    }
    else {
        p1.setY(center.y() - r);
        p1.setX(center.x());
        p2.setY(origin.y() - 40);
        p2.setX(circleX(p2.y(), r, center.x(), center.y(), false));
        drawArrow(QPointF(p1.x() + 5, p1.y() + 2), QPointF(p2.x() - 5, p2.y() - 2));
    }
    drawPoint(QPointF(p1.x(), origin.y() - 1));
    drawPoint(QPointF(p2.x(), origin.y() - 1));
    setPen(QPen(QColor(Qt::blue), 1));
    setBrush(QBrush(Qt::blue));
    drawEllipse(p1, 2, 2);
    drawEllipse(p2, 2, 2);
    setPen(QPen(QColor(Qt::black), 1));
    setBrush(Qt::NoBrush);
}

void ReportPainter::drawArrow(QPointF start, QPointF end)
{
    qreal arrowSize = 4;
    QLineF line(start, end);
    drawLine(line);

    double angle = std::atan2(-line.dy(), line.dx());
    QPointF p0 = line.p2() - QPointF(sin(angle + M_PI / 3) * arrowSize, cos(angle + M_PI / 3) * arrowSize);
    QPointF p1 = line.p2() - QPointF(sin(angle + M_PI - M_PI / 3) * arrowSize, cos(angle + M_PI - M_PI / 3) * arrowSize);
    QPolygonF arrowHead;
    arrowHead.clear();
    arrowHead << p0 << p1 << line.p2();

    setBrush(QColor(Qt::black));
    drawPolygon(arrowHead);
    setBrush(Qt::NoBrush);
}

void ReportPainter::drawCurve(QList<QPointF> points)
{
    if (points.isEmpty()) {
        return;
    }
    QPainterPath path;
    path.moveTo(points.first());
    foreach (auto point, points) {
        path.lineTo(point);
    }
    drawPath(path);
}

QPointF ReportPainter::intersection(qreal ssvri, qreal lswi, qreal cvp, qreal lap)
{
    qreal x = qSqrt(80 * lswi / (0.0144 * ssvri) + qPow(40 * (cvp - lap) / ssvri, 2)) - 40 * (cvp - lap) / ssvri;
    qreal y = qSqrt(lswi * ssvri / (80 * 0.0144) - cvp * lap + qPow((cvp + lap) / 2, 2)) + (cvp + lap) / 2;
    return QPointF(x, y);
}

void ReportPainter::positionSymbol(int pos, QPoint point, QColor color)
{
    setPen(QPen(color, 1));
    setBrush(color);
    QRect rect(point.x() - 3, point.y() - 3, 6, 6);
    if (1 == pos) {
        drawRect(rect);
    }
    else if (1 == pos) {
        drawEllipse(rect);
    }
    else if (2 == pos) {
        QPolygon triangle;
        triangle.setPoints(3, point.x(), point.y() - 3,
                           point.x() + 3, point.y() + 3,
                           point.x() - 3, point.y() + 3);
        drawPolygon(triangle);
    }
    setPen(QPen(Qt::black, 1));
    setBrush(Qt::NoBrush);
}

void ReportPainter::positionText(int pos, QPoint point, QColor color)
{
    if (1 == pos) {
        drawText(point, "半卧:");
    }
    else if (2 == pos) {
        drawText(point, "平躺:");
    }
    else if (3 == pos) {
        drawText(point, "抬腿:");
    }
    positionSymbol(pos, QPoint(point.x() + 25, point.y() - 3), color);
}

qreal ReportPainter::actualValue(qreal value, int digit)
{
    if (DatCa::invalid() != value) {
        return static_cast<int>(value * qPow(10, digit)) / qPow(10, digit);
    }
    return value;
}

qreal ReportPainter::limits(qreal actual, qreal min, qreal max)
{
    if (actual < min) {
        actual = min;
    }
    else if (actual > max) {
        actual = max;
    }
    return actual;
}

QString ReportPainter::compare(qreal v1, qreal v2)
{
    if (v1 < v2) {
        return "增加";
    }
    else if (v1 == v2) {
        return "不变";
    }
    return "减少";
}

qreal ReportPainter::circleX(qreal y, qreal r, qreal ox, qreal oy, bool negative)
{
    auto temp = qSqrt(qPow(r, 2) - qPow(y - oy, 2));
    return negative ? ox - temp : ox + temp;
}

QRectF ReportPainter::rectF(qreal x, qreal y)
{
    return QRectF(QPointF(x, y), m_size);
}

QString ReportPainter::getPicFileName(int pos)
{
    if (1 == pos) {
        return ":/images/halfsleeper.png";
    }
    else if (2 == pos) {
        return ":/images/lieflat.png";
    }
    else if (3 == pos) {
        return ":/images/leglift.png";
    }
    return QString();
}

template<class T>
QString ReportPainter::ekey(const T &t)
{
    return ReportDataName::ekey(t);
}
