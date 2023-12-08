QT -= gui
QT += network sql

CONFIG += c++17 console
CONFIG -= app_bundle

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

HEADERS += \
    clientworker.h \
    config.h \
    subtcpserver.h \
    tcpclientsocket.h \
    tcpserver.h

SOURCES += \
        clientworker.cpp \
        main.cpp \
        subtcpserver.cpp \
        tcpclientsocket.cpp \
        tcpserver.cpp

include(../../commonFiles/base.pri)
include(../../commonFiles/telegramprofile/telegramprofile.pri)
include(../../commonFiles/qt-solutions/qtsingleapplication/src/qtsinglecoreapplication.pri)
include(../../commonFiles/MqttServerNs/mqttserverns.pri)

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

