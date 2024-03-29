QT -= gui
QT += core serialport

TEMPLATE = lib
DEFINES += ZYTEBCO_LIBRARY

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    zytebco.cpp

HEADERS += \
    DemoModeData.h \
    zytebco.h

include(../../commonFiles/config.pri)

#include(../../commonFiles/qextserialport/src/qextserialport.pri)

#LIBS += -L$$PROJECT_LIBDIR -lQt5ExtSerialPort1

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target
