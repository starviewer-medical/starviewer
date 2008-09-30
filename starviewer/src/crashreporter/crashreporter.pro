TEMPLATE = app

include(../applicationtargetnames.inc)
TARGET = $${TARGET_STARVIEWER_CRASH_REPORTER}

DESTDIR = ../../bin

FORMS = qcrashreporterbase.ui
HEADERS = qcrashreporter.h
SOURCES = crashreporter.cpp \
          qcrashreporter.cpp

QT += network
CONFIG -= app_bundle

include(../corelibsconfiguration.inc)
include(../compilationtype.inc)
