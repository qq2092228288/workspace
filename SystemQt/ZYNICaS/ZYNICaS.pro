QT       += core gui widgets serialport concurrent charts
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
    anothersetdialog.cpp \
    auxargdialog.cpp \
    bpeditdialog.cpp \
    countdowngizmo.cpp \
#    createreportthread.cpp \
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
    reportdatajson.cpp \
    reportdialog.cpp \
    reportpainter.cpp \
    reportpreviewdialog.cpp \
    reporttablemodel.cpp \
    scopecalendardialog.cpp \
    selectitemdialog.cpp \
    sudokuwidget.cpp \
    systemconfigdialog.cpp \
    trendchart.cpp \
    trendchartswidget.cpp \
    updateappdialog.cpp \
    viewreportdialog.cpp \

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
    reportdatajson.h \
    reportdialog.h \
    reportpainter.h \
    reportpreviewdialog.h \
    reportset.h \
    reporttablemodel.h \
    scopecalendardialog.h \
    selectitemdialog.h \
    sudokuwidget.h \
    systemconfigdialog.h \
    trendchart.h \
    trendchartswidget.h \
    updateappdialog.h \
    viewreportdialog.h \

include(../../commonFiles/base.pri)
#include(../../commonFiles/qextserialport/src/qextserialport.pri)
include(../../commonFiles/qt-solutions/qtsingleapplication/src/qtsingleapplication.pri)
include(../../commonFiles/MqttServerNs/mqttserverns.pri)
nicas.path = ../../commonFiles/nicas
include(../../commonFiles/nicas.pri)

INCLUDEPATH += ../ZyTebco \
#    ../IdCheck \
#    ../../commonFiles/QRencode

LIBS += -L$$PROJECT_LIBDIR -lZyTebco \
#    -L$$PROJECT_LIBDIR -lIdCheck \
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

RESOURCES += \
    common.qrc \
    images.qrc \
    version.qrc
