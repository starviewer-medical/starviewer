TEMPLATE = app

include(../applicationstargetnames.inc)
TARGET = $${TARGET_STARVIEWER_SAP_WRAPPER}

DESTDIR = ../../bin

macx {
    DESTDIR = $${DESTDIR}/$${TARGET_STARVIEWER}.app/Contents/MacOS
}

QT += network

HEADERS = ../core/starviewerapplication.h

SOURCES = starviewersapwrapper.cpp 

INCLUDEPATH += ../core

include(../corelibsconfiguration.inc)
include(../compilationtype.inc)
include(../log4cxx.inc)


