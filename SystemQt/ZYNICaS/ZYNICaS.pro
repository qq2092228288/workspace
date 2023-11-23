QT       += core gui widgets serialport concurrent charts
QT       += printsupport sql network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17
TARGET = YZNICaS

#版本信息、图标
win32: {
# Windows RC
RC_FILE = resource.rc
}

#管理员权限运行
#CONFIG(release, debug|release){
#    QMAKE_LFLAGS += /MANIFESTUAC:\"level=\'requireAdministrator\' uiAccess=\'false\'\"
#}


# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    anothersetdialog.cpp \
    auxargdialog.cpp \
    bpeditdialog.cpp \
    countdowngizmo.cpp \
    customctrl.cpp \
    datamanagement.cpp \
    deviceparameters.cpp \
    drawsudoku.cpp \
    drawwaveforms.cpp \
    entersystemwidget.cpp \
    getiddialog.cpp \
    infoeditdialog.cpp \
    isicurvewidget.cpp \
    logindialog.cpp \
    main.cpp \
    mainwidget.cpp \
    mqttclient.cpp \
    personalinfodialog.cpp \
    plrttablewidget.cpp \
    reportdialog.cpp \
    reportpreviewdialog.cpp \
    selectitemdialog.cpp \
    sudokuwidget.cpp \
    systemconfigdialog.cpp \
    trendchart.cpp \
    trendchartswidget.cpp \
    viewreportdialog.cpp \
    waitingdialog.cpp

HEADERS += \
    anothersetdialog.h \
    auxargdialog.h \
    bpeditdialog.h \
    countdowngizmo.h \
    customctrl.h \
    datamanagement.h \
    drawsudoku.h \
    drawwaveforms.h \
    entersystemwidget.h \
    getiddialog.h \
    infoeditdialog.h \
    isicurvewidget.h \
    logindialog.h \
    mainwidget.h \
    mqttclient.h \
    personalinfodialog.h \
    plrttablewidget.h \
    reportdialog.h \
    reportpreviewdialog.h \
    selectitemdialog.h \
    sudokuwidget.h \
    systemconfigdialog.h \
    trendchart.h \
    trendchartswidget.h \
    version.h \
    viewreportdialog.h \
    waitingdialog.h

include(../../commonFiles/base.pri)
include(../../commonFiles/qt-solutions/qtsingleapplication/src/qtsingleapplication.pri)
include(../../commonFiles/MqttServerNs/mqttserverns.pri)
include(../../commonFiles/nicas/report.pri)
include(../../commonFiles/zywidgets/zywidgets.pri)

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

INCLUDEPATH += ../ZyTebco \
#    ../../commonFiles/QRencode

LIBS += -L$$PWD/$$PROJECT_LIBDIR -lZyTebco \
#    -L$$PROJECT_LIBDIR -lqrencoded

win32:CONFIG(release, debug|release): LIBS += -L$$[QT_INSTALL_LIBS] -lQt5Mqtt
else:win32:CONFIG(debug, debug|release): LIBS += -L$$[QT_INSTALL_LIBS] -lQt5Mqttd
else:unix: LIBS += -L$$[QT_INSTALL_LIBS] -lQt5Mqtt

RESOURCES += \
    images.qrc \
    version.qrc
