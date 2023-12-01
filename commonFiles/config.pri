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

unix: {
    contains(QT_ARCH, x86_64) {
        DESTDIR = ../bin_x86_64_linux/$${SUB_LIBDIR}
        PROJECT_LIBDIR = ../bin_x86_64_linux/$${SUB_LIBDIR}
    } else {
        contains(QT_ARCH, arm64) {
            DESTDIR = ../bin_arm64_linux/$${SUB_LIBDIR}
            PROJECT_LIBDIR = ../bin_arm64_linux/$${SUB_LIBDIR}
        } else {
            DESTDIR = ../bin_linux/$${SUB_LIBDIR}
            PROJECT_LIBDIR = ../bin_linux/$${SUB_LIBDIR}
        }
    }
}

win32: {
    DESTDIR = ../bin_x86_win32/$${SUB_LIBDIR}
    PROJECT_LIBDIR = ../bin_x86_win32/$${SUB_LIBDIR}
}