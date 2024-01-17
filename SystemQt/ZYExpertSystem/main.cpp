#include <qtsingleapplication.h>
#ifdef Q_OS_WIN
#include <windows.h>
#endif
#include "logindialog.h"
#include "mainwidget.h"
#include "tcpclientsocket.h"

#include "reportgraphicsitem.h"
#include "printgraphicsview.h"
#include <QGraphicsScene>
#include <QPageSize>
#include <QScreen>

int main(int argc, char *argv[])
{
    QtSingleApplication a(QApplication::applicationName(), argc, argv);
    if (!a.isRunning()) {
#ifdef Q_OS_WIN
        SetThreadExecutionState(ES_CONTINUOUS | ES_SYSTEM_REQUIRED | ES_DISPLAY_REQUIRED);
#endif
        QCoreApplication::setApplicationName(QString::fromUtf8("泽耀专家系统"));
        QCoreApplication::setOrganizationName(QString::fromUtf8("广东泽耀医疗技术有限公司"));
        QCoreApplication::setOrganizationDomain(QString::fromUtf8("www.zeyaotebco.com"));
        QCoreApplication::setApplicationVersion(QString::fromUtf8("2.1.0.1229"));

        QGraphicsScene scene;

        // auto size = QGuiApplication::primaryScreen()->availableSize();
        auto psize = QPageSize::sizePixels(QPageSize::A4, 96);
        // scene.setSceneRect(0, -0.1*size.height(), psize.width(), psize.height() * 3);
        scene.setItemIndexMethod(QGraphicsScene::NoIndex);
        for (int i = 0; i < 1; ++i) {
            auto item = new ReportGraphicsItem(psize);
            item->setPos(0, i * (psize.height() + 20));
            scene.addItem(item);
        }

        PrintGraphicsView view(&scene);

        view.showMaximized();

        // LoginDialog *dialog = new LoginDialog;

        // MainWidget_PTR widget(new MainWidget);
        // QObject::connect(dialog, &LoginDialog::successful, widget.get(), &MainWidget::showMaximized);

        // TcpClientSocket::getInstance()->connectToServer();

        // dialog->exec();
        // delete dialog;
        // dialog = nullptr;

        // if (widget->isHidden())
        //     return 0;
        return a.exec();
    }
    else {
        a.sendMessage(QString("程序正在运行中！"));
    }
    return 0;
}
