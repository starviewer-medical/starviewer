TEMPLATE = app

include(../applicationstargetnames.pri)
TARGET = $${TARGET_STARVIEWER_SAP_WRAPPER}

DESTDIR = ../../bin

macx {
    DESTDIR = $${DESTDIR}/$${TARGET_STARVIEWER}.app/Contents/MacOS
}

HEADERS = ../core/starviewerapplication.h

SOURCES = starviewersapwrapper.cpp \
    ../core/logging.cpp \
    ../core/starviewerapplication.cpp

INCLUDEPATH += ../core

official_release {
    win32:RESOURCES += ../main/qtconf/win/qtconf.qrc
    #macx:RESOURCES += ../main/qtconf/mac/qtconf.qrc    # For future use
    #linux:RESOURCES += ../main/qtconf/linux/qtconf.qrc # For future use
}

include(../corelibsconfiguration.pri)
include(../compilationtype.pri)

QT += widgets
