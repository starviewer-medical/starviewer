TEMPLATE = app

include(../applicationstargetnames.inc)
TARGET = $${TARGET_STARVIEWER_CRASH_REPORTER}

DESTDIR = ../../bin

mac* {
  DESTDIR = $${DESTDIR}/$${TARGET_STARVIEWER}.app/Contents/MacOS
}

FORMS = qcrashreporterbase.ui
HEADERS = qcrashreporter.h
SOURCES = crashreporter.cpp \
          qcrashreporter.cpp

QT += network

include(../corelibsconfiguration.inc)
include(../compilationtype.inc)
