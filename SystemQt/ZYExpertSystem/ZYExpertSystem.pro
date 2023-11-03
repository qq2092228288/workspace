QT       += core gui widgets printsupport sql network

CONFIG += c++17
# app icon
RC_ICONS = $$PWD/images/app.ico

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        logindialog.cpp \
        main.cpp \
        mainwidget.cpp \
        mqttclient.cpp \
        printpreviewdialog.cpp \
        reportconfigdialog.cpp \
        reportdialog.cpp

HEADERS += \
    logindialog.h \
    mainwidget.h \
    mainwidgetui.h \
    mqttclient.h \
    printpreviewdialog.h \
    reportconfigdialog.h \
    reportdialog.h \
    ui/logindialogui.h \
    ui/mainwidgetui.h \
    ui/reportconfigdialogui.h \
    ui/reportdialogui.h

include(../../commonFiles/base.pri)
include(../../commonFiles/qt-solutions/qtsingleapplication/src/qtsingleapplication.pri)
include(../../commonFiles/MqttServerNs/mqttserverns.pri)
include(../../commonFiles/nicas/report.pri)
include(../../commonFiles/zywidgets/zywidgets.pri)

win32:CONFIG(release, debug|release): LIBS += -L$$[QT_INSTALL_LIBS] -lQt5Mqtt
else:win32:CONFIG(debug, debug|release): LIBS += -L$$[QT_INSTALL_LIBS] -lQt5Mqttd
else:unix: LIBS += -L$$[QT_INSTALL_LIBS] -lQt5Mqtt

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    ui.qrc
