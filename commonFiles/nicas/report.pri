INCLUDEPATH	+= $$PWD
DEPENDPATH      += $$PWD
HEADERS		+= \
    $$PWD/datacalculation.h \
    $$PWD/datatype.h \
    $$PWD/painterconfig.h \
    $$PWD/reportdatajson.h \
    $$PWD/reportdataname.h \
    $$PWD/reportgraphicsitem.h \
    $$PWD/reportpainter.h \
    $$PWD/reportset.h \
    $$PWD/reporttablemodel.h
SOURCES		+= \
    $$PWD/datacalculation.cpp \
    $$PWD/datatype.cpp \
    $$PWD/painterconfig.cpp \
    $$PWD/reportdatajson.cpp \
    $$PWD/reportdataname.cpp \
    $$PWD/reportgraphicsitem.cpp \
    $$PWD/reportpainter.cpp \
    $$PWD/reporttablemodel.cpp

RESOURCES += \
    $$PWD/img.qrc \
    $$PWD/json.qrc

