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
    anothersetdialog.cpp \
    auxargdialog.cpp \
    bpeditdialog.cpp \
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
    main.cpp \
    mainwidget.cpp \
    personalinfodialog.cpp \
    reportdatabase.cpp \
    reportword.cpp \
    selectitemdialog.cpp \
    showreportdialog.cpp \
    singleapplication.cpp \
    systemconfigdialog.cpp \
    threadserivce.cpp \
    trendchart.cpp \
    trendchartswidget.cpp \
    waitingdialog.cpp

HEADERS += \
    MyStruct.h \
    anothersetdialog.h \
    auxargdialog.h \
    bpeditdialog.h \
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
    mainwidget.h \
    personalinfodialog.h \
    reportdatabase.h \
    reportword.h \
    selectitemdialog.h \
    showreportdialog.h \
    singleapplication.h \
    systemconfigdialog.h \
    threadserivce.h \
    trendchart.h \
    trendchartswidget.h \
    waitingdialog.h


include(../../commonFiles/base.pri)

INCLUDEPATH += ../ZyTebco \
    ../IdCheck \
    ../../commonFiles/QRencode

LIBS += -L$$PROJECT_LIBDIR -lZyTebco \
    -L$$PROJECT_LIBDIR -lIdCheck \
    -L$$PROJECT_LIBDIR -lqrencoded

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES +=

RESOURCES += \
    images.qrc \
    version.qrc
