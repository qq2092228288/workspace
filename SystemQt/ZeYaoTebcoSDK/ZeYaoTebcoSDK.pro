QT -= gui
QT += core serialport

TEMPLATE = lib
DEFINES += ZEYAOTEBCOSDK_LIBRARY

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    zeyaotebcosdk.cpp \
    zeyaotebcosdkprivate.cpp

HEADERS += \
    zeyaotebcosdk.h \
    zeyaotebcosdkprivate.h


include(../../commonFiles/base.pri)
include(../../commonFiles/nicas/report.pri)
include(../../commonFiles/MqttServerNs/basic.pri)

win32:CONFIG(release, debug|release): LIBS += -L$$[QT_INSTALL_LIBS] -lQt5Mqtt
else:win32:CONFIG(debug, debug|release): LIBS += -L$$[QT_INSTALL_LIBS] -lQt5Mqttd
else:unix: LIBS += -L$$[QT_INSTALL_LIBS] -lQt5Mqtt

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target
