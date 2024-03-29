QT       += core gui widgets printsupport sql network xlsx

CONFIG += c++17
# app icon
RC_ICONS = $$PWD/images/app.ico

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        datanfilterwidgetns.cpp \
        datatablemodel.cpp \
        datawidget.cpp \
        logindialog.cpp \
        main.cpp \
        mainwidget.cpp \
        # mqttclient.cpp \
        nicasdatafilterwidget.cpp \
        printpreviewdialog.cpp \
        reportdialog.cpp \
        systemconfigdialog.cpp \
        tcpclientsocket.cpp \
        waitingdialog.cpp

HEADERS += \
    datanfilterwidgetns.h \
    datatablemodel.h \
    datawidget.h \
    logindialog.h \
    mainwidget.h \
    mainwidgetui.h \
    # mqttclient.h \
    nicasdatafilterwidget.h \
    printpreviewdialog.h \
    reportdialog.h \
    systemconfigdialog.h \
    tcpclientsocket.h \
    ui/datawidgetui.h \
    ui/logindialogui.h \
    ui/mainwidgetui.h \
    ui/reportdialogui.h \
    ui/systemconfigdialogui.h \
    ui/waitingdialogui.h \
    waitingdialog.h

include(../../commonFiles/config.pri)
include(../../commonFiles/qt-solutions/qtsingleapplication/src/qtsingleapplication.pri)
include(../../commonFiles/MqttServerNs/mqttserverns.pri)
include(../../commonFiles/nicas/report.pri)
include(../../commonFiles/zywidgets/zywidgets.pri)
include(../../commonFiles/telegramprofile/telegramprofile.pri)

win32:CONFIG(release, debug|release): LIBS += -L$$[QT_INSTALL_LIBS] -lQt5Xlsx
else:win32:CONFIG(debug, debug|release): LIBS += -L$$[QT_INSTALL_LIBS] -lQt5Xlsxd
else:unix: LIBS += -L$$[QT_INSTALL_LIBS] -lQt5Xlsx

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    ui.qrc
