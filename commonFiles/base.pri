#QMAKE_CXXFLAGS +=  -Wno-unused
#CONFIG += debug_and_release

#QMAKE_CXXFLAGS += /wd"4819"

include($$PWD/threadservice/threadservice.pri)

win32-msvc* {
    QMAKE_CXXFLAGS += /source-charset:utf-8 /execution-charset:utf-8
}

CONFIG(debug, debug|release) {
    OBJECTS_DIR += ./debug/obj
    MOC_DIR += ./debug/moc
    RCC_DIR += ./debug/rcc
    UI_DIR += ./debug/forms
    SUB_LIBDIR = debug
}

CONFIG(release, debug|release) {
    OBJECTS_DIR += ./release/obj
    MOC_DIR += ./release/moc
    RCC_DIR += ./release/rcc
    UI_DIR += ./release/forms
    SUB_LIBDIR = release
}

unix:{
    target.path = /usr/lib
    INSTALLS += target

    contains(DEFINES,ANDROID){
        DEFINES += ANDROID_PLATFORM
        contains(CONFIG,Release){
            DESTDIR = ../bin_Android/release
            PROJECT_LIBDIR = ../bin_Android/release
        } else {
            DESTDIR = ../bin_Android/debug
            PROJECT_LIBDIR = ../bin_Android/debug
        }
    } else {
        DEFINES += LINUX_PLATFORM

        if (contains(DEFINES, linux_arm)) {
            contains(DEFINES, linux_arm_571x) {
                DESTDIR = ../../BIXTestSystem/bin_arm_linux
                PROJECT_LIBDIR = ../../BIXTestSystem/bin_arm_linux
            } else {
                DESTDIR = ../bin_arm_linux
                PROJECT_LIBDIR = ../bin_arm_linux
            }
        } else {

            contains(DEFINES, linux_arm_5728) {

                DEFINES += AM57XX_PLATFORM

                DESTDIR = ../../Am5728/bin_arm_linux
                PROJECT_LIBDIR = ../../Am5728/bin_arm_linux
           } else {

            contains(DEFINES, linux_arm_571x) {

                DEFINES += AM57XX_PLATFORM

                DESTDIR = ../../Am5728/bin_arm_linux_571x
                PROJECT_LIBDIR = ../../Am5728/bin_arm_linux_571x
           } else {
                     DESTDIR = ../bin_x86_linux64
                     PROJECT_LIBDIR = ../bin_x86_linux64
                  }

            }
        }
    }
}

win32:{
    QT += dbus
    DEFINES += WIN32_PLATFORM
    DESTDIR = ../bin_x86_win32/$${SUB_LIBDIR}
    PROJECT_LIBDIR = ../bin_x86_win32/$${SUB_LIBDIR}
}

#message("Project library path: $${PROJECT_LIBDIR}")
#message("Project bin path:     $${DESTDIR}")
