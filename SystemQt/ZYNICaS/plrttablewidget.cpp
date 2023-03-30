#include "plrttablewidget.h"
#include <QApplication>
#include <QScreen>
#include <QtMath>

PlrtTableWidget::PlrtTableWidget(QWidget *parent)
    : QDialog{parent},
      m_rows{4},
      m_columns{9}
{
    mainLayout = new QGridLayout(this);
    // 列名
    m_headerText<<""<<"HR"<<"SI"<<"CI"<<"SV"<<"CO"<<"DO2"<<"TFC"<<"ISI";
    setWindowTitle(tr("被动抬腿试验"));
    double zoom = QApplication::primaryScreen()->availableSize().width()/1920.0;
    setMinimumSize(1000*zoom, 400*zoom);
    setStyleSheet(QString("PlrtTableWidget{background-color:#FFFFFF;}"));

    QFont headerFont;
    headerFont.setPixelSize(25*zoom);
    headerFont.setBold(true);
    QFont dataFont;
    dataFont.setPixelSize(20*zoom);
    for (int row = 0; row < rowCount(); ++row) {
        for (int col = 0; col < header().size(); ++col) {
            QLabel *label = new QLabel(this);
            label->setAlignment(Qt::AlignCenter);
            label->setStyleSheet(labelQss(LabelStatus::None));
            mainLayout->addWidget(label, row, col);
            m_pLabels.append(label);
            if (0 == row) {
                label->setText(header().at(col));
                label->setFont(headerFont);
            }
            else if (0 == col) {
                label->setFont(headerFont);
                label->setGeometry(0, 0, 100*zoom, 50*zoom);
            }
            else {
                label->setFont(dataFont);
            }
        }
    }
    // PLRT单位
    labels().at(getLabelNumber(3, 0))->setText("%");
}

int PlrtTableWidget::rowCount() const
{
    return m_rows;
}

int PlrtTableWidget::columnCount() const
{
    return m_columns;
}

QStringList PlrtTableWidget::header() const
{
    return m_headerText;
}

void PlrtTableWidget::clear()
{
    for (int row = 1; row < rowCount(); ++row) {
        for (int col = 0; col < columnCount(); ++col) {
            if (!(3 == row && 0 == col)) {
                auto label = labels().at(getLabelNumber(row, col));
                label->clear();
                label->setStyleSheet(labelQss(LabelStatus::None));
            }
        }
    }
}

void PlrtTableWidget::setPic(const PosType &fpos, const PosType &spos)
{
    auto fLabel = labels().at(getLabelNumber(1, 0));
    auto sLabel = labels().at(getLabelNumber(2, 0));
    auto fPixmap = QPixmap(getPicFileName(fpos));
    auto sPixmap = QPixmap(getPicFileName(spos));
    double fZoom = 1.0*fLabel->width()/QPixmap(getPicFileName(fpos)).width();
    double sZoom = 1.0*sLabel->width()/QPixmap(getPicFileName(spos)).width();

    fLabel->setPixmap(fPixmap.scaled(fPixmap.width()*fZoom, fPixmap.height()*fZoom, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    sLabel->setPixmap(sPixmap.scaled(sPixmap.width()*sZoom, sPixmap.height()*sZoom, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
}

void PlrtTableWidget::setPos(const QString &fpos, const QString &spos)
{
    labels().at(getLabelNumber(1, 0))->setText(fpos);
    labels().at(getLabelNumber(2, 0))->setText(spos);
}

void PlrtTableWidget::setPlrtData(const PaType &type, const double &fdata, const double &sdata)
{
    double offset = dataDigit((sdata - fdata)/fdata, 3);
    labels().at(getLabelNumber(1, type))->setText(QString::number(fdata));
    labels().at(getLabelNumber(2, type))->setText(QString::number(sdata));
    labels().at(getLabelNumber(3, type))->setText(QString::number(offset*100));
    dataWarning(type, offset >= 0.1 ? false : true);
}

void PlrtTableWidget::setData(const QString &name, const double &fdata, const double &sdata)
{
    PaType type;
    if (name == "HR") {
        type = PaType::Hr;
    }
    else if (name == "SI") {
        type = PaType::Si;
    }
    else if (name == "CI") {
        type = PaType::Ci;
    }
    else if (name == "SV") {
        type = PaType::Sv;
    }
    else if (name == "CO") {
        type = PaType::Co;
    }
    else if (name == "DO2") {
        type = PaType::Do2;
    }
    else if (name == "TFC") {
        type = PaType::Tfc;
    }
    else if (name == "ISI") {
        type = PaType::Isi;
    }
    else {
        return;
    }
    setPlrtData(type, fdata, sdata);
}

QString PlrtTableWidget::labelQss(LabelStatus status)
{
    QString backgroundColor = "#FFFFFF";
    QString fontColor = "black";
    switch (status) {
    case LabelStatus::None:
        backgroundColor = "#FFFFFF";
        fontColor = "black";
        break;
    case LabelStatus::Normal:
        backgroundColor = "#00FF00";
        fontColor = "white";
        break;
    case LabelStatus::Warning:
        backgroundColor = "#FF6347";
        fontColor = "yellow";
        break;
    }
    return QString("QLabel{background-color:%1;"
                   "font-family:Microsoft YaHei;"
                   "color:%2;"
                   "border-style:solid;"
                   "border-width:2px;"
                   "border-color:green;"
                   "border-radius:10px;}").arg(backgroundColor, fontColor);
}

double PlrtTableWidget::dataDigit(const double &value, const int &decimals)
{
    return (static_cast<int>((value + 0.5/qPow(10, decimals))*qPow(10, decimals)))/qPow(10, decimals);
}

QList<QLabel *> PlrtTableWidget::labels() const
{
    return m_pLabels;
}

QString PlrtTableWidget::getPicFileName(const PosType &type)
{
    switch (type) {
    case PosType::HalfSleeper:
        return ":/images/halfsleeper.png";
        break;
    case PosType::LieFlat:
        return ":/images/lieflat.png";
        break;
    case PosType::LegLift:
        return ":/images/leglift.png";
        break;
    default:
        break;
    }
    return QString();
}

int PlrtTableWidget::getLabelNumber(const int &row, const int &col)
{
    return (row*header().size() + col);
}

void PlrtTableWidget::dataWarning(const PaType &type, bool w)
{
    labels().at(getLabelNumber(3, type))->setStyleSheet(labelQss(w ? LabelStatus::Warning : LabelStatus::Normal));
}
