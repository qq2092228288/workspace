#include "plrttablewidget.h"
#include <QApplication>
#include <QScreen>
#include <QtMath>

PlrtTableWidget::PlrtTableWidget(QWidget *parent)
    : QDialog{parent},
      m_rows{4},
      m_columns{9}
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);

    mainLayout = new QGridLayout(this);
    // 列名
    m_headerText<<""<<"HR"<<"SI"<<"CI"<<"SV"<<"CO"<<"DO2"<<"TFC"<<"ISI";
    setWindowTitle(tr("被动抬腿试验"));
    double zoom = QApplication::primaryScreen()->availableSize().width()/1920.0;
    setMinimumSize(1000*zoom, 400*zoom);
    setStyleSheet(QString("PlrtTableWidget{background-color:#EEFFFF;border-radius:10px;}"));

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

    m_pEditBpBtn = new QPushButton(tr("抬腿后血压"), this);
//    m_pStartBtn = new QPushButton(tr("开始"), this);
    m_pStopBtn = new QPushButton(tr("终止"), this);
    m_pPauseBtn = new QPushButton(tr("开始"), this);
    m_pTimer = new QTimer(this);
    m_pProgressBar = new QProgressBar(this);

    mainLayout->addWidget(m_pEditBpBtn, rowCount() + 1, 0, Qt::AlignLeft);
//    mainLayout->addWidget(m_pStartBtn, rowCount() + 1, count - 3, Qt::AlignRight);
    mainLayout->addWidget(m_pStopBtn, rowCount() + 1, 3, Qt::AlignRight);
    mainLayout->addWidget(m_pPauseBtn, rowCount() + 1, 4, Qt::AlignRight);
    mainLayout->addWidget(m_pProgressBar, rowCount() + 2, 0, 1, m_headerText.count());

    m_pProgressBar->setAlignment(Qt::AlignCenter);
    m_pProgressBar->setStyleSheet(QString("QProgressBar{"
                                          "font:9pt;"
                                          "border-radius:5px;"
                                          "text-align:center;"
                                          "border:1px solid #E8EDF2;"
                                          "background-color: rgb(255, 255, 255);"
                                          "border-color: rgb(180, 180, 180);"
                                      "}"
                                      "QProgressBar:chunk{"
                                          "border-radius:5px;"
                                          "background-color:#1ABC9C;"
                                      "}"));
    // PLRT单位
    labels().at(getLabelNumber(3, 0))->setText("%");

    connect(m_pEditBpBtn, &QPushButton::clicked, this, &PlrtTableWidget::editBp);
//    connect(m_pStartBtn, &QPushButton::clicked, this, &PlrtTableWidget::startSlot);
    connect(m_pStopBtn, &QPushButton::clicked, this, &PlrtTableWidget::stopSlot);
    connect(m_pPauseBtn, &QPushButton::clicked, this, &PlrtTableWidget::pauseSlot);
    connect(m_pTimer, &QTimer::timeout, this, &PlrtTableWidget::timeroutSlot);

    setProcessBarTxt(0);
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

void PlrtTableWidget::setCountDown(const int &sec)
{
    if (sec < 2 || sec > 3600) {
        QMessageBox::warning(this, tr("警告！"), tr("设置的时间要在1~3600秒之间！"));
        return;
    }
    m_sec = sec;
    m_pProgressBar->setRange(0, m_sec);
    m_pProgressBar->setValue(0);
    m_pTimer->setInterval(1000);
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
    m_pProgressBar->setValue(0);
}

void PlrtTableWidget::setPic(const PosType &fpos, const PosType &spos)
{
    auto fLabel = labels().at(getLabelNumber(1, 0));
    auto sLabel = labels().at(getLabelNumber(2, 0));
    auto fPixmap = QPixmap(getPicFileName(fpos));
    auto sPixmap = QPixmap(getPicFileName(spos));
    double fZoom = 1.0*fLabel->width()/QPixmap(getPicFileName(fpos)).width();
    double sZoom = 1.0*sLabel->width()/QPixmap(getPicFileName(spos)).width();

    fLabel->setPixmap(fPixmap.scaled(fPixmap.width()*fZoom - 5, fPixmap.height()*fZoom,
                                     Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    sLabel->setPixmap(sPixmap.scaled(sPixmap.width()*sZoom - 5, sPixmap.height()*sZoom,
                                     Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
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

void PlrtTableWidget::closeEvent(QCloseEvent *event)
{
    event->accept();
    m_pProgressBar->setValue(0);
}

void PlrtTableWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (m_mousePress) {
        move(event->globalPos() - m_mousePoint);
    }
    event->accept();
}

void PlrtTableWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_mousePress = true;
        m_mousePoint = event->pos();
    }
    event->accept();
}

void PlrtTableWidget::mouseReleaseEvent(QMouseEvent *event)
{
    m_mousePress = false;
    event->accept();
}

//void PlrtTableWidget::startSlot()
//{
//    m_pTimer->start();
//    emit status(TestS::Started);
//}

void PlrtTableWidget::timeroutSlot()
{
    int value = m_pProgressBar->value();
    m_pProgressBar->setValue(++value);
    setProcessBarTxt(--m_sec);
    if (value == m_pProgressBar->maximum()) {
        m_pTimer->stop();
        close();
        emit status(TestS::Completed);
    }
}

void PlrtTableWidget::stopSlot()
{
    close();
    if (m_pTimer->isActive()) {
        pauseSlot();
    }
    m_pProgressBar->setValue(0);
    setProcessBarTxt(0);
    emit status(TestS::Stopped);
}

void PlrtTableWidget::pauseSlot()
{
    if (m_pTimer->isActive()) {
        m_pTimer->stop();
        m_pPauseBtn->setText("开始");
        emit status(TestS::Paused);
    }
    else {
        m_pTimer->start();
        m_pPauseBtn->setText("暂停");
        emit status(TestS::Started);
    }
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

void PlrtTableWidget::setProcessBarTxt(const int &value)
{
    m_pProgressBar->setFormat(QString("试验剩余时间 %1:%2").arg(value/60, 2, 10, QLatin1Char('0'))
                              .arg(value%60, 2, 10, QLatin1Char('0')));
}
