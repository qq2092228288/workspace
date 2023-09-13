QT -= gui
QT += core sql websockets webchannel

CONFIG += c++17 console
CONFIG -= app_bundle

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
#    htmlclient.cpp \
#    htmlserver.cpp \
    main.cpp \
    mqttclient.cpp \
    topicanalysis.cpp \

HEADERS += \
#    htmlclient.h \
#    htmlserver.h \
    mqttclient.h \
    topicanalysis.h \

COMMONFILES_DIR = ../../commonFiles
include($$COMMONFILES_DIR/base.pri)
include($$COMMONFILES_DIR/qt-solutions/qtsingleapplication/src/qtsinglecoreapplication.pri)
include($$COMMONFILES_DIR/MqttServerNs/mqttserverns.pri)
include($$COMMONFILES_DIR/webchannel/shared.pri)

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


win32:CONFIG(release, debug|release): LIBS += -L$$[QT_INSTALL_LIBS] -lQt5Mqtt
else:win32:CONFIG(debug, debug|release): LIBS += -L$$[QT_INSTALL_LIBS] -lQt5Mqttd
else:unix: LIBS += -L$$[QT_INSTALL_LIBS] -lQt5Mqtt


RESOURCES += \
    xml.qrc
