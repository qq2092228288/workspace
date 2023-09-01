#include "reportpainter.h"
#include <QPaintEngine>
#include <QPainterPath>
#include <QDateTime>
#include <QDebug>

#include "datacalculation.h"
#include "datamanagement.h"

ReportPainter::ReportPainter(const ReportInfo &info, QPrinter *printer)
    : QPainter{printer},
      m_info{info},
      m_size{printer->pageRect(QPrinter::DevicePixel).size()},
      m_printer{printer}
{
    if (m_info.type == Printer_Type::Thermal) {

    }
    else {
        generalDataPage();
    }
}

void ReportPainter::generalHeader()
{
    auto data = m_info.data;
    // 场所信息
    auto place = data.value(ekey(ReportDataName::place)).toObject();
    setFontSize(16, true);
    // 医院LOGO
    QPixmap pixmap(DataManagement::getInstance().getPaths().hospitalLogo());
    if (pixmap.width() > pixmap.height()) {
        drawPixmap(QRect(30, 15, 50, pixmap.height() * 50 / pixmap.width()), pixmap);
    }
    else {
        drawPixmap(QRect(30, 15, pixmap.width() * 50 / pixmap.height(), 50), pixmap);
    }
    // 医院
    drawText(rectF(0, 10), Qt::AlignHCenter, place.value(ekey(ReportDataName::primaryPlace)).toString());
    // 标题
    drawText(rectF(0, 40), Qt::AlignHCenter, QString("无创心功能监测报告"));
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
    int hvalue2 = 110;
    drawText(rectF( 50, hvalue2), "体表面积:");
    drawText(rectF(125, hvalue2), QString::number(DataCalculation::cBsa(height, weight), 'f', 2) + " m²");
    drawText(rectF(250, hvalue2), "科 室:");
    drawText(rectF(300, hvalue2), place.value(ekey(ReportDataName::secondaryPlace)).toString());
    drawText(rectF(500, hvalue2), "病历号:");
    drawText(rectF(555, hvalue2), info.value(ekey(ReportDataName::medicalRecordNumber)).toString());
    drawLine(QPointF(30, 140), QPointF(m_size.width() - 30, 140));
}

void ReportPainter::generalDataPage(int page)
{
    // 页眉
    generalHeader();
    // 页脚
    generalFooter();
    // 数据
    auto position = m_info.data.value(ekey(ReportDataName::position)).toArray();
    QJsonObject data = position.at(page).toObject().value(ekey(ReportDataName::data)).toObject();
    QJsonObject sdata = position.at(1).toObject().value(ekey(ReportDataName::data)).toObject();
    // 第二体位数据存在
    bool secExist = !position.at(1).toObject().value(ekey(ReportDataName::reportTime)).toString().isEmpty();
    ReportTableSet set(m_info.paging);
    // 行高
    qreal row_h = set.rowHeight(m_info.mode);
    setFontSize(8, true);
    auto rectArg = drawTableCell(QRectF(set.start(), QSizeF(set.colWidth().at(0), row_h)),
                                 "参数名称");
    auto rectFirst = drawTableCell(QRectF(rectArg.topRight(), QSizeF(set.colWidth().at(1), row_h)),
                                   positionCn(data.value(QString::number(Type::Pos)).toInt()) + "体位");
    auto rectSecond = rectFirst;
    // 单页
    bool single = secExist && !m_info.paging;
    if (single) {
        rectSecond = drawTableCell(QRectF(rectFirst.topRight(), QSizeF(set.colWidth().at(2), row_h)),
                                   positionCn(sdata.value(QString::number(Type::Pos)).toInt()));
    }
    auto rectRange = drawTableCell(QRectF(rectSecond.topRight(), QSizeF(set.colWidth().at(2 + single), row_h)),
                                   "参考范围");
    auto rectUnit = drawTableCell(QRectF(rectRange.topRight(), QSizeF(set.colWidth().at(3 + single), row_h)),
                                  "单位");
    auto fMap = valueMap(data);
    auto sMap = valueMap(sdata);
    auto array = ReportParameters::array(m_info.mode);
    for (int i = 0; i < array.count(); ++i) {
        auto group = array.at(i).toObject();
        auto parameters = group.value("parameters").toArray();
        setFontSize(8);
        for (int j = 0; j < parameters.count(); ++j) {
            auto parameter = parameters.at(j).toObject();
            qreal _y = rectArg.bottomLeft().y() + row_h * j;
            auto min = parameter.value("min").toDouble();
            auto max = parameter.value("max").toDouble();
            // 参数名称
            drawTableCell(QRectF(rectArg.bottomLeft().x() + set.groupWidth(), _y,
                                 set.colWidth().at(0) - set.groupWidth(), row_h),
                          parameter.value("cn").toString() + "/" + parameter.value("en").toString(),
                          Qt::AlignLeft | Qt::AlignVCenter, 10);
            // 值
            drawValue(parameter, fMap, rectFirst, min, max, _y);
            if (single) {
                drawValue(parameter, sMap, rectSecond, min, max, _y);
            }
            // 正常范围
            drawTableCell(QRectF(QPointF(rectRange.topLeft().x(), _y), rectRange.size()),
                          min != max ? QString("%1 ~ %2").arg(min).arg(max) : "-",
                          Qt::AlignLeft | Qt::AlignVCenter, 10);
            // 单位
            drawTableCell(QRectF(QPointF(rectUnit.topLeft().x(), _y), rectUnit.size()),
                          parameter.value("unit").toString(),
                          Qt::AlignLeft | Qt::AlignVCenter, 10);
        }
        setFontSize(8, true);
        rectArg = drawTableCell(QRectF(rectArg.bottomLeft(), QSizeF(set.groupWidth(), row_h * parameters.count())),
                      group.value("module").toString(),
                      Qt::AlignCenter | Qt::TextSingleLine | Qt::TextWordWrap);
    }
    // 血压靶向分析图
    drawSudoku(QRectF(rectUnit.topRight().x() + 23, 400, 720 - rectUnit.topRight().x(), 720 - rectUnit.topRight().x()), data);
    // 分隔线
    drawLine(QPointF(rectArg.bottomLeft().x(), rectArg.bottomLeft().y() + 10),
             QPointF(m_size.width() - set.start().x(), rectArg.bottomLeft().y() + 10));
    // 第二页
    if (secExist && m_info.paging && 0 == page) { // 双体位 分页 当前为第一页
        m_printer->newPage();
        generalDataPage(1);
    }
    setFontSize(8);
    // 结论
    drawText(QRectF(set.start().x(), rectArg.bottomLeft().y() + 10,
                    m_size.width() - set.start().x() * 2, m_size.height() - rectArg.bottomLeft().y() - 10),
             Qt::AlignLeft | Qt::TextWordWrap,
             m_info.data.value(ekey(ReportDataName::reportConclusion)).toString());
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
    drawText(rectF(35, 1055), "报告时间: " + QDateTime::fromString(reportTime, "yyyyMMddhhmmss")
             .toString("yyyy-MM-dd hh:mm:ss"));
    auto place = m_info.data.value(ekey(ReportDataName::place)).toObject();
    drawText(rectF(640, 1055), "检测人员:  " + place.value(ekey(ReportDataName::inspector)).toString());
    drawText(rectF(35, 1075), "此报告只说明检测当时状态的血流动力学情况，请结合临床具体情况综合分析。");
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

QMap<Type, qreal> ReportPainter::valueMap(const QJsonObject &data)
{
    auto info = m_info.data.value(ekey(ReportDataName::patientInfo)).toObject();
    // 基本数据
    auto age = info.value(ekey(ReportDataName::age)).toString().toInt();
    auto height = info.value(ekey(ReportDataName::height)).toString().toInt();
    auto weight = info.value(ekey(ReportDataName::weight)).toString().toInt();
    auto sex = info.value(ekey(ReportDataName::sex)).toString() == "男" ? 0 : 1;
    auto hb = info.value(ekey(ReportDataName::hb)).toString().toInt();
    auto sbp = data.value(QString::number(Type::SBP)).toInt();
    auto dbp = data.value(QString::number(Type::DBP)).toInt();
    auto lap = data.value(QString::number(Type::LAP)).toInt();
    auto cvp = data.value(QString::number(Type::CVP)).toInt();
    // 计算值
    auto bsa = DatCa::cBsa(height, weight);
    auto vept = DatCa::cVept(height, weight, sex);
    auto map = DatCa::cMap(sbp, dbp);

    QMap<Type, qreal> vMap;
    vMap.insert(Type::SBP, sbp);
    vMap.insert(Type::DBP, dbp);
    vMap.insert(Type::LAP, lap);
    vMap.insert(Type::CVP, cvp);
    vMap.insert(Type::MAP, map);

    auto lvet = DatCa::cVet(data.value(QString::number(Type::VET)).toInt());
    auto pep = DatCa::cPep(data.value(QString::number(Type::PEP)).toInt());
    auto isi = DatCa::cIsi(data.value(QString::number(Type::ISI)).toInt());
    auto ef = DatCa::cEf(data.value(QString::number(Type::EF)).toInt());
    auto ino = DatCa::cIno(isi, sex, age);
    auto si = DatCa::cSi(data.value(QString::number(Type::SI)).toInt(), bsa, vept);
    auto sv = DatCa::cSv(si, bsa);
    auto lswi = DatCa::cLswi(si, map, lap);
    auto ssvri = DatCa::cSsvri(si, map, cvp);
    auto ci = DatCa::cCi(data.value(QString::number(Type::CI)).toInt(), bsa, vept);
    auto co = DatCa::cCo(ci, bsa);

    vMap.insert(Type::HR, DatCa::cHr(data.value(QString::number(Type::HR)).toInt()));
    vMap.insert(Type::LVET, lvet);
    vMap.insert(Type::PEP, pep);
    vMap.insert(Type::STR, DatCa::cStr(pep, lvet));
    vMap.insert(Type::TFC, DatCa::cTfc(data.value(QString::number(Type::TFC)).toInt()));
    vMap.insert(Type::EPCI, DatCa::cEpci(data.value(QString::number(Type::EPCI)).toInt()));
    vMap.insert(Type::ISI, isi);
    vMap.insert(Type::Ino, ino);
    vMap.insert(Type::SI, si);
    vMap.insert(Type::SV, sv);
    vMap.insert(Type::EDI, DatCa::cEdi(si, ef));
    vMap.insert(Type::LSW, DatCa::cLsw(sv, map, lap));
    vMap.insert(Type::LSWI, lswi);
    vMap.insert(Type::Vol, DatCa::cVol(lswi, ino));
    vMap.insert(Type::SSVR, DatCa::cSsvr(sv, map, cvp));
    vMap.insert(Type::SSVRI, DatCa::cSsvri(si, map, cvp));
    vMap.insert(Type::Vas, DatCa::cVas(ssvri));
    vMap.insert(Type::SVV, DatCa::cSvv(sv, QList<qreal>()));
    vMap.insert(Type::CI, ci);
    vMap.insert(Type::CO, co);
    vMap.insert(Type::HRV, DatCa::cHrv(ci));
    vMap.insert(Type::SVR, DatCa::cSvr(co, map, cvp));
    vMap.insert(Type::SVRI, DatCa::cSvri(ci, map, cvp));
    vMap.insert(Type::LCW, DatCa::cLcw(co, map, lap));
    vMap.insert(Type::LCWI, DatCa::cLcwi(ci, map, lap));
    vMap.insert(Type::DO2, DatCa::cDo2(co, hb));

    return vMap;
}

void ReportPainter::drawValue(const QJsonObject &parameter, QMap<Type, qreal> map,
                               QRectF rect, qreal min, qreal max, qreal _y)
{
    auto value = map.value(Type(parameter.value("type").toInt()));
    if (DatCa::invalid() != value) {
        auto digit = parameter.value("digit").toInt();
        value = int(value * qPow(10, digit)) / qPow(10, digit);
    }
    drawTableCell(QRectF(QPointF(rect.topLeft().x(), _y), rect.size()),
                  DatCa::invalid() != value ? QString::number(value) : "-",
                  Qt::AlignLeft | Qt::AlignVCenter, 10);
    if (DatCa::invalid() != value) {
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

QString ReportPainter::positionCn(int pos)
{
    if (1 == pos)      return QString("半卧");
    else if (2 == pos) return QString("平躺");
    else if (3 == pos) return QString("抬腿");
    return QString();
}

void ReportPainter::drawSudoku(QRectF rect, const QJsonObject &data)
{
    setFontSize(6);
    drawRect(rect);
    // 刻度
    const qreal minX = 10, maxX = 100;
    const qreal minY = 20, maxY = 180;
    // 步长
    const qreal stepLenX = rect.width()/(maxX - minX);
    const qreal stepLenY = rect.height()/(maxY - minY);
    auto origin = rect.bottomLeft();
    for (int x = minX; x <= maxX; x += 10) {
        QPoint temp(origin.x() + (x - minX) * stepLenX, origin.y() + 9);
        drawPoint(temp.x(), origin.y() - 1);
        drawText(temp, QString::number(x));
    }
    for (int y = minY; y <= maxY; y += 20) {
        QPoint temp(origin.x() - 17, origin.y() - (y - minY) * stepLenY);
        drawPoint(origin.x() + 1, temp.y());
        drawText(temp, QString("%1").arg(y, 3, 10, QLatin1Char(' ')));
    }
    // 范围值线 MAP SI
    auto map = ReportParameters::find(Type::MAP);
    auto si = ReportParameters::find(Type::SI);
    qreal minMap = map.value("min").toDouble();
    qreal maxMap = map.value("max").toDouble();
    qreal minSi = map.value("min").toDouble();
    qreal maxSi = map.value("max").toDouble();
    qreal minMapY = (minMap - minY) * stepLenY;
    qreal maxMapY = (maxMap - minY) * stepLenY;
    qreal minSiX = (minSi - minX) * stepLenX;
    qreal maxSiX = (maxSi - minX) * stepLenX;
    drawLine(origin.x(), origin.y() - minMapY, rect.bottomRight().x(), origin.y() - minMapY);
    drawLine(origin.x(), origin.y() - maxMapY, rect.bottomRight().x(), origin.y() - maxMapY);
    drawLine(origin.x() + minSiX, origin.y(), origin.x() + minSiX, rect.topLeft().y());
    drawLine(origin.x() + maxSiX, origin.y(), origin.x() + maxSiX, rect.topLeft().y());
    auto lswi = ReportParameters::find(Type::LSWI);
    auto ssvri = ReportParameters::find(Type::SSVRI);
    qreal minLswi = lswi.value("min").toDouble();
    qreal maxLswi = lswi.value("max").toDouble();
    qreal minSsvri = ssvri.value("min").toDouble();
    qreal maxSsvri = ssvri.value("max").toDouble();
    auto lap = data.value(QString::number(Type::LAP)).toInt();
    auto cvp = data.value(QString::number(Type::CVP)).toInt();
    QList<QPointF> minSsvriPoints;
    QList<QPointF> maxSsvriPoints;
    QList<QPointF> minLswiPoints;
    QList<QPointF> maxLswiPoints;
    for (int x = 0; x < maxX*10; ++x) {
        qreal _x = x/10.0;
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
    drawCurve(minSsvriPoints);
    drawCurve(maxSsvriPoints);
    drawCurve(minLswiPoints);
    drawCurve(maxLswiPoints);

    setPen(QPen(QColor(Qt::blue), 1));
    auto p1 = intersection(minSsvri, minLswi, cvp, lap);
    auto p2 = intersection(minSsvri, maxLswi, cvp, lap);
    auto p3 = intersection(maxSsvri, minLswi, cvp, lap);
    auto p4 = intersection(maxSsvri, maxLswi, cvp, lap);

//    drawPoint(origin.x() + (p1.x() - minX) * stepLenX, origin.y() - (p1.y() - minY) * stepLenY);
//    drawPoint(origin.x() + (p2.x() - minX) * stepLenX, origin.y() - (p2.y() - minY) * stepLenY);
//    drawPoint(origin.x() + (p3.x() - minX) * stepLenX, origin.y() - (p3.y() - minY) * stepLenY);
//    drawPoint(origin.x() + (p4.x() - minX) * stepLenX, origin.y() - (p4.y() - minY) * stepLenY);
    positionSymbol(data.value(QString::number(Type::Pos)).toInt(), QPointF(origin.x(), origin.y() + 20));
}

void ReportPainter::drawCurve(QList<QPointF> points)
{
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

void ReportPainter::positionSymbol(int pos, QPointF point)
{
    drawText(point, positionCn(pos) + ":");
    QRect rect(point.x() + 22, point.y() - 6, 6, 6);
    if (1 == pos) {
        setPen(QPen(Qt::blue, 1));
        setBrush(QColor(Qt::blue));
        drawEllipse(rect);
    }
    else if (2 == pos) {
        setPen(QPen(Qt::blue, 1));
        setBrush(QColor(Qt::blue));
        drawRect(rect);
    }
    else if (3 == pos) {
        setPen(QPen(Qt::blue, 1));
        setBrush(QColor(Qt::blue));
        QPolygon triangle;
        int x = point.x() + 22;
        int y = point.y();
        triangle.setPoints(3, x, y, x + 6, y, x + 3, y - 6);
        drawPolygon(triangle);
    }
    setPen(Qt::NoBrush);
    setBrush(Qt::NoBrush);
}

QRectF ReportPainter::rectF(qreal x, qreal y)
{
    return QRectF(QPointF(x, y), m_size);
}

template<class T>
QString ReportPainter::ekey(const T &t)
{
    return ReportDataName::ekey(t);
}
