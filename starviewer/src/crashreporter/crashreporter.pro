TEMPLATE = app

include(../applicationstargetnames.inc)
TARGET = $${TARGET_STARVIEWER_CRASH_REPORTER}

DESTDIR = ../../bin

mac* {
  DESTDIR = $${DESTDIR}/$${TARGET_STARVIEWER}.app/Contents/MacOS
}

mac* {
    DEFINES += STARVIEWER_EXE=\\\"../../../$${TARGET_STARVIEWER}\\\"
}
linux* {
    DEFINES += STARVIEWER_EXE=\\\"$${TARGET_STARVIEWER}\\\"
}
win32 {
    DEFINES += STARVIEWER_EXE=\\\"$${TARGET_STARVIEWER}.exe\\\"
}

FORMS = qcrashreporterbase.ui
HEADERS = qcrashreporter.h
SOURCES = crashreporter.cpp \
          qcrashreporter.cpp

RESOURCES = ../main/main.qrc

QT += network

include(../corelibsconfiguration.inc)
include(../compilationtype.inc)
