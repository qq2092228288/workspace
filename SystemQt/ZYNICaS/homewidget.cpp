#include "homewidget.h"
#include "mainpagewidget.h"
#include "parameterwidget.h"
#include "datatype.h"
#include "reportset.h"

#include <QScreen>
#include <QDebug>

HomeWidget::HomeWidget(QWidget *parent)
    : QWidget{parent}
{
    auto pal = palette();
    pal.setColor(QPalette::Window, Qt::white);
    setPalette(pal);

    auto mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    auto leftButton = new QPushButton("<", this);
    auto rightButton = new QPushButton(">", this);
    stack = new QStackedWidget(this);

    mainLayout->addWidget(leftButton);
    mainLayout->addWidget(stack);
    mainLayout->addWidget(rightButton);

    // auto w = style()->pixelMetric(QStyle::PM_TitleBarHeight);
    leftButton->setFixedWidth(20);
    rightButton->setFixedWidth(20);
    leftButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
    rightButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);

    auto mainPageWidget = new MainPageWidget(stack);
    auto paramWidget = new QWidget(stack);
    auto paramLayout = new QGridLayout;
    QFile qssFile(":/homepagestyle/mainpageui.qss");
    if (qssFile.open(QIODevice::ReadOnly)) {
        auto factor = 1013.0 / (screen()->availableSize().height() -
                                style()->pixelMetric(QStyle::PM_TitleBarHeight));
        QRegExp exp("(\\d+)(?:px)");
        QStringList list;
        while (!qssFile.atEnd()) {
            QString line = qssFile.readLine();
            if (line.indexOf(exp) >= 0) {
                int value = exp.cap(1).toDouble() / factor;
                if (value == 0) value = 1;
                line.replace(exp.cap(0), QString("%1px").arg(value));
            }
            list.append(line);
        }
        mainPageWidget->setStyleSheet(list.join(""));
        paramWidget->setStyleSheet(list.join(""));
    }
    paramWidget->setLayout(paramLayout);
    QList<Type> paramList {
        Type::EDI, Type::Vol, Type::SVR, Type::SSVR, Type::SSVRI, Type::SVRI, Type::Vas,
        Type::PEP, Type::LVET, Type::LSW, Type::LSWI, Type::LCW, Type::LCWI, Type::STR,
        Type::EPCI, Type::ISI, Type::Ino, Type::HRV, Type::MAP, Type::DO2, Type::SVV
    };
    for (int i = 0; i < paramList.count(); ++i) {
        auto widget = new ParameterWidget(ParamJson(ReportParameters::find(paramList.at(i))), paramWidget);
        paramLayout->addWidget(widget, i / 7, i%7);
    }

    stack->addWidget(mainPageWidget);
    stack->addWidget(paramWidget);

    connect(leftButton, &QPushButton::clicked, this, [=](){
        stack->setCurrentIndex(stack->currentIndex() - 1);
    });
    connect(rightButton, &QPushButton::clicked, this, [=](){
        stack->setCurrentIndex(stack->currentIndex() + 1);
    });
    connect(stack, &QStackedWidget::currentChanged, this, [=](int index){
        leftButton->setHidden(index == 0);
        rightButton->setHidden(index == stack->count() - 1);
    });
}
