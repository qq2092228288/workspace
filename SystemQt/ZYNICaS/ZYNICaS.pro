QT       += core gui widgets serialport charts axcontainer concurrent
QT       += printsupport sql network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17
TARGET = YZNICaS
#版本信息、图标
RC_FILE = resource.rc
#管理员权限运行
#CONFIG(release, debug|release){
#    QMAKE_LFLAGS += /MANIFESTUAC:\"level=\'requireAdministrator\' uiAccess=\'false\'\"
#}


# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ../MqttServer/databasens.cpp \
    ../MqttServer/singleton.cpp \
    anothersetdialog.cpp \
    auxargdialog.cpp \
    baseargwidget.cpp \
    baseargwidgetsinst.cpp \
    bpeditdialog.cpp \
    countdowngizmo.cpp \
    createreportthread.cpp \
    customctrl.cpp \
    datacalculation.cpp \
    datamanagement.cpp \
    devicedatabase.cpp \
    deviceparameters.cpp \
    drawsudoku.cpp \
    drawwaveforms.cpp \
    entersystemwidget.cpp \
    getiddialog.cpp \
    httppost.cpp \
    infoeditdialog.cpp \
    isicurvewidget.cpp \
    logindialog.cpp \
    main.cpp \
    mainwidget.cpp \
    mqttclient.cpp \
    personalinfodialog.cpp \
    plrttablewidget.cpp \
    reportdatabase.cpp \
    reportword.cpp \
    selectitemdialog.cpp \
    showreportdialog.cpp \
    singleapplication.cpp \
    sudokuwidget.cpp \
    surplusbaseargsdialog.cpp \
    systemconfigdialog.cpp \
    threadserivce.cpp \
    trendchart.cpp \
    trendchartswidget.cpp \
    updateappdialog.cpp \
    waitingdialog.cpp

HEADERS += \
    ../MqttServer/databasens.h \
    ../MqttServer/singleton.h \
    ../MqttServer/topicns.h \
    MyStruct.h \
    anothersetdialog.h \
    auxargdialog.h \
    baseargwidget.h \
    baseargwidgetsinst.h \
    bpeditdialog.h \
    countdowngizmo.h \
    createreportthread.h \
    customctrl.h \
    datacalculation.h \
    datamanagement.h \
    devicedatabase.h \
    deviceparameters.h \
    drawsudoku.h \
    drawwaveforms.h \
    entersystemwidget.h \
    getiddialog.h \
    httppost.h \
    infoeditdialog.h \
    isicurvewidget.h \
    logindialog.h \
    mainwidget.h \
    mqttclient.h \
    personalinfodialog.h \
    plrttablewidget.h \
    reportdatabase.h \
    reportword.h \
    selectitemdialog.h \
    showreportdialog.h \
    singleapplication.h \
    sudokuwidget.h \
    surplusbaseargsdialog.h \
    systemconfigdialog.h \
    threadserivce.h \
    trendchart.h \
    trendchartswidget.h \
    updateappdialog.h \
    waitingdialog.h


include(../../commonFiles/base.pri)
#include(../../commonFiles/qextserialport/src/qextserialport.pri)

INCLUDEPATH += ../ZyTebco \
    ../IdCheck \
#    ../../commonFiles/QRencode

LIBS += -L$$PROJECT_LIBDIR -lZyTebco \
    -L$$PROJECT_LIBDIR -lIdCheck \
#    -L$$PROJECT_LIBDIR -lqrencoded

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32:{
    COMPILATION_CHAIN_DIR = D:/Qt/5.15.2/msvc2019_64
}
unix:{
    COMPILATION_CHAIN_DIR = /home/ubuntu/Qt5.12.12/5.12.12/gcc_64
}

win32:CONFIG(release, debug|release): LIBS += -L$$COMPILATION_CHAIN_DIR/lib/ -lQt5Mqtt
else:win32:CONFIG(debug, debug|release): LIBS += -L$$COMPILATION_CHAIN_DIR/lib/ -lQt5Mqttd
else:unix: LIBS += -L$$COMPILATION_CHAIN_DIR/lib/ -lQt5Mqtt
INCLUDEPATH += ../MqttServer

DISTFILES +=

RESOURCES += \
    images.qrc \
    version.qrc
