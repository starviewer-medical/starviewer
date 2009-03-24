TEMPLATE = app

include(../applicationstargetnames.inc)
TARGET = $${TARGET_STARVIEWER_SAP_WRAPPER}

DESTDIR = ../../bin

macx {
    DESTDIR = $${DESTDIR}/$${TARGET_STARVIEWER}.app/Contents/MacOS
}

macx {
    DEFINES += STARVIEWER_EXE=\\\"../../../$${TARGET_STARVIEWER}\\\"
}
linux* {
    DEFINES += STARVIEWER_EXE=\\\"$${TARGET_STARVIEWER}\\\"
}
win32 {
    DEFINES += STARVIEWER_EXE=\\\"$${TARGET_STARVIEWER}.exe\\\"
}

HEADERS = ../core/starviewerapplication.h \
 	  starviewersapwrapper.h \
          ../inputoutput/starviewersettings.h

SOURCES = main.cpp  \
 	  starviewersapwrapper.cpp \
         ../inputoutput/starviewersettings.cpp

INCLUDEPATH += ../main/src_breakpad ../core

include(../corelibsconfiguration.inc)
include(../compilationtype.inc)
include(../log4cxx.inc)

QT += network
