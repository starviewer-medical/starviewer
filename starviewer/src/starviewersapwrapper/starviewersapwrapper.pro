TEMPLATE = app

include(../applicationstargetnames.pri)
TARGET = $${TARGET_STARVIEWER_SAP_WRAPPER}

DESTDIR = ../../bin

macx {
    DESTDIR = $${DESTDIR}/$${TARGET_STARVIEWER}.app/Contents/MacOS
}

HEADERS = ../core/starviewerapplication.h

SOURCES = starviewersapwrapper.cpp 

INCLUDEPATH += ../core

include(../corelibsconfiguration.pri)
include(../compilationtype.pri)
include(../log4cxx.pri)

QT += widgets
