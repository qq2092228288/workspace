#include "waveform.h"

Waveform::Waveform(QWidget *parent)
    : QWidget{parent}
{}

void Waveform::clear()
{

}

void Waveform::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setBrush(QColor(64, 158, 255));
    painter.setPen(QPen(QColor(64, 158, 255), 1));
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    auto size = QSizeF(width() - 1, (height() - 21) / 3.0);
    auto ecgRect = QRectF(QPointF(0, 0), size);
    auto diffRect = QRectF(QPointF(0, ecgRect.bottomLeft().y() + 10), size);
    auto admitRect = QRectF(QPointF(0, diffRect.bottomLeft().y() + 10), size);
    painter.drawRoundedRect(ecgRect, 40, 27, Qt::RelativeSize);
    painter.drawRoundedRect(diffRect, 40, 27, Qt::RelativeSize);
    painter.drawRoundedRect(admitRect, 40, 27, Qt::RelativeSize);
    ecgRect.setTopLeft(QPointF(0, ecgRect.y() + 5));
    diffRect.setTopLeft(QPointF(0, diffRect.y() + 5));
    admitRect.setTopLeft(QPointF(0, admitRect.y() + 5));
    painter.setPen(QPen(Qt::white, 1));
    painter.drawText(ecgRect, Qt::AlignHCenter | Qt::AlignTop, QString::fromUtf8("阻抗心电图"));
    painter.drawText(diffRect, Qt::AlignHCenter | Qt::AlignTop, QString::fromUtf8("心血流图"));
    painter.drawText(admitRect, Qt::AlignHCenter | Qt::AlignTop, QString::fromUtf8("心阻抗图"));
}
