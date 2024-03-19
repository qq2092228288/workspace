#include "plrwidget.h"

PlrWidget::PlrWidget(QWidget *parent)
    : QWidget{parent}
{
    auto layout = new QVBoxLayout(this);
    auto gLayout = new QGridLayout;
    auto stopButton = new QPushButton(QString::fromUtf8("终止\n试验"), this);
    auto comboBox = new QComboBox(this);
    auto stack = new QStackedWidget(this);
    auto startButton = new QPushButton(QString::fromUtf8("启动\n试验"), stack);
    auto pauseButton = new QPushButton(QString::fromUtf8("暂停\n试验"), stack);
    auto progressBar = new QProgressBar(this);

    comboBox->addItems(QStringList{
        QString::fromUtf8("一分钟"),
        QString::fromUtf8("两分钟"),
        QString::fromUtf8("三分钟"),
        QString::fromUtf8("四分钟"),
        QString::fromUtf8("五分钟")
    });

    stack->addWidget(startButton);
    stack->addWidget(pauseButton);

    gLayout->addWidget(stopButton, 0, 0, 2, 1, Qt::AlignLeft | Qt::AlignVCenter);
    gLayout->addWidget(comboBox, 0, 1, 2, 1, Qt::AlignCenter);
    gLayout->addWidget(stack, 0, 2, 2, 1, Qt::AlignRight | Qt::AlignVCenter);
    gLayout->addWidget(progressBar, 2, 0, 1, 3);

    layout->setContentsMargins(40, 0, 40, 0);
    layout->addStretch(82);
    layout->addLayout(gLayout, 14);
    layout->addStretch(4);

    connect(stopButton, &QPushButton::clicked, this, &PlrWidget::stop);
    connect(startButton, &QPushButton::clicked, this, [=](){
        stack->setCurrentIndex(1);
    });
    connect(pauseButton, &QPushButton::clicked, this, [=](){
        stack->setCurrentIndex(0);
    });
    progressBar->setMaximum(100);
    progressBar->setValue(80);
}

void PlrWidget::initWidget()
{

}

void PlrWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setBrush(QColor(64, 158, 255));
    painter.setPen(QPen(QColor(64, 158, 255), 1));
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    auto rect = QRectF(QPointF(0, 0), QSizeF(width() - 1, height() - 1));
    painter.drawRoundedRect(rect, 40, 6, Qt::RelativeSize);
    painter.setBrush(Qt::NoBrush);
    // painter.setBrush(Qt::white);
    // painter.setPen(QPen(QColor(64, 158, 255), 2));
    painter.setPen(QPen(Qt::white, 2));
    auto f = font();
    f.setBold(true);
    f.setPixelSize(height() * 0.08);
    painter.setFont(f);
    painter.drawRoundedRect(QRectF(20, height() * 0.03, width() - 40, height() * 0.78), 20, 20);
    painter.drawLine(21, height() * 0.18, width() - 21, height() * 0.18);
    painter.drawText(QRectF(20, height() * 0.01, width() - 40, height() * 0.18),
                     Qt::AlignCenter, QString::fromUtf8("被动抬腿试验"));
    QStringList list {
        "PLR", QString::fromUtf8("第一体位"), QString::fromUtf8("第二体位"), "%",
        "HR", "60", "66", "10",
        "SV", "40", "42", "5",
        "ISI", "1.0", "1.15", "15"
    };
    f.setPixelSize(height() * 0.03);

    painter.setFont(f);
    painter.drawLine(width() * 0.07 + 1, height() * 0.33, width() * 0.27, height() * 0.2 + 1);
    painter.drawLine(width() * 0.07 + 1, height() * 0.2 + 1, width() * 0.27, height() * 0.33);
    for (int i = 0; i < list.count(); ++i) {
        auto r = QRectF(width() * (0.07 + i / 4 * 0.22), height() * (0.2 + i % 4 * 0.15),
                        width() * 0.2, height() * 0.13);
        painter.setBrush(Qt::NoBrush);
        // painter.setPen(QPen(QColor(64, 158, 255), 2));
        painter.drawRect(r);
        painter.setBrush(QColor(64, 158, 255));
        painter.drawRoundedRect(r, 100, 100, Qt::RelativeSize);
        // painter.setPen(QPen(QColor(64, 158, 255), 1));
        painter.drawText(r, Qt::AlignCenter, list.at(i));
    }
    painter.setBrush(Qt::white);
    painter.drawRoundedRect(QRectF(20, height() * 0.82, width() - 40, height() * 0.15), 20, 20);
}
