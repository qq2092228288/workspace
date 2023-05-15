QT -= gui
QT += sql websockets webchannel

CONFIG += c++17 console
CONFIG -= app_bundle

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        $$[QT_INSTALL_EXAMPLES]/webchannel/shared/websocketclientwrapper.cpp \
        $$[QT_INSTALL_EXAMPLES]/webchannel/shared/websockettransport.cpp \
        htmlserver.cpp \
        main.cpp \
        mqttclient.cpp \
        singleton.cpp \
        topicanalysis.cpp \

HEADERS += \
    $$[QT_INSTALL_EXAMPLES]/webchannel/shared/websocketclientwrapper.h \
    $$[QT_INSTALL_EXAMPLES]/webchannel/shared/websockettransport.h \
    databasens.h \
    htmlserver.h \
    mqttclient.h \
    singleton.h \
    topicanalysis.h \
    topicns.h \

include(../../commonFiles/base.pri)
include(../../commonFiles/qt-solutions/qtsingleapplication/src/qtsinglecoreapplication.pri)
INCLUDEPATH += $$[QT_INSTALL_EXAMPLES]/webchannel/shared

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

win32:{
    INCLUDEPATH += $$COMPILATION_CHAIN_DIR/include
    DEPENDPATH += $$COMPILATION_CHAIN_DIR/include
# postgresql
    INCLUDEPATH += $$quote(D:/Program Files/PostgreSQL/15/include)
    LIBS += -L$$quote(D:/Program Files/PostgreSQL/15/lib/) -llibpq
}
else:unix:{
    INCLUDEPATH += $$COMPILATION_CHAIN_DIR/include
    DEPENDPATH += $$COMPILATION_CHAIN_DIR/include
}
