TEMPLATE = app

include(../applicationstargetnames.inc)
TARGET = $${TARGET_STARVIEWER_SAP_WRAPPER}

DESTDIR = ../../bin

macx {
    DESTDIR = $${DESTDIR}/$${TARGET_STARVIEWER}.app/Contents/MacOS
}

QT += network

HEADERS = ../core/starviewerapplication.h \
          ../core/logging.h \
          ../core/settingsregistry.h \
          ../core/settings.h \
          ../core/settingsparser.h \
          ../core/defaultsettings.h \
          ../core/settingsaccesslevelfilereader.h \
          ../inputoutput/inputoutputsettings.h \
          qstarviewersapwrapper.h

SOURCES = ../core/settingsregistry.cpp \
          ../core/settings.cpp \
          ../core/settingsparser.cpp \
          ../core/defaultsettings.cpp \
          ../inputoutput/inputoutputsettings.cpp \
          ../core/settingsaccesslevelfilereader.cpp \
          starviewersapwrapper.cpp \
          qstarviewersapwrapper.cpp

INCLUDEPATH += ../core

include(../corelibsconfiguration.inc)
include(../compilationtype.inc)
include(../log4cxx.inc)


