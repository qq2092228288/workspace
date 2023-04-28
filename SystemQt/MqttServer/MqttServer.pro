QT -= gui
QT += sql

CONFIG += c++17 console
CONFIG -= app_bundle

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        main.cpp \
        mqttclient.cpp \
        singleton.cpp \
        topicanalysis.cpp

HEADERS += \
    databasens.h \
    mqttclient.h \
    singleton.h \
    topicanalysis.h \
    topicns.h

include(../../commonFiles/base.pri)

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32:CONFIG(release, debug|release): LIBS += -LD:/Qt/5.15.2/msvc2019_64/lib/ -lQt5Mqtt
else:win32:CONFIG(debug, debug|release): LIBS += -LD:/Qt/5.15.2/msvc2019_64/lib/ -lQt5Mqttd
else:unix: LIBS += -L/home/ubuntu/Qt5.12.12/5.12.12/gcc_64/lib/ -lQt5Mqtt

win32:{
INCLUDEPATH += D:/Qt/5.15.2/msvc2019_64/include
DEPENDPATH += D:/Qt/5.15.2/msvc2019_64/include
}
else:unix:{
INCLUDEPATH += /home/ubuntu/Qt5.12.12/5.12.12/gcc_64/include
DEPENDPATH += /home/ubuntu/Qt5.12.12/5.12.12/gcc_64/include
}
