QT += core gui widgets charts

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

HEADERS += \
        mainwidget.h \
        vqueue.h \
        waveform.h

SOURCES += \
        main.cpp \
        mainwidget.cpp \
        waveform.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

include(../../commonFiles/base.pri)

INCLUDEPATH += ../AdsModule

LIBS += -L$$PROJECT_LIBDIR -lAdsModule

DISTFILES += \
    leadwire.json
