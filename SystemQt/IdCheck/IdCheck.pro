QT -= gui

TEMPLATE = lib
DEFINES += IDCHECK_LIBRARY

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    idcheck.cpp

HEADERS += \
    idcheck.h


include(../../commonFiles/base.pri)

INCLUDEPATH += ../../commonFiles/quazip-0.7.3/quazip \
    D:\Qt\5.15.2\Src\qtbase\src\3rdparty\zlib\src
LIBS += -L$$PROJECT_LIBDIR -lquazip

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target
