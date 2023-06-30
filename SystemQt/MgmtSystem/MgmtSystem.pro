QT -= gui
QT += sql websockets webchannel

CONFIG += c++17 console
CONFIG -= app_bundle

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        htmlclient.cpp \
        htmlserver.cpp \
        main.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

COMMONFILES_DIR = ../../commonFiles
include($$COMMONFILES_DIR/base.pri)
include($$COMMONFILES_DIR/qt-solutions/qtsingleapplication/src/qtsinglecoreapplication.pri)
include($$COMMONFILES_DIR/MqttServerNs/mqttserverns.pri)
include($$COMMONFILES_DIR/webchannel/shared.pri)

HEADERS += \
    htmlclient.h \
    htmlserver.h
