TEMPLATE = app

include(../applicationstargetnames.pri)
TARGET = $${TARGET_STARVIEWER_CRASH_REPORTER}

DESTDIR = ../../bin

macx {
    CONFIG += no_keywords
    DESTDIR = $${DESTDIR}/$${TARGET_STARVIEWER}.app/Contents/MacOS
}

FORMS = qcrashreporterbase.ui
HEADERS = qcrashreporter.h \
          crashreportersender.h \
          ../core/settingsregistry.h \
          ../core/settings.h \
          ../core/settingsparser.h \
          ../core/defaultsettings.h \
          ../core/coresettings.h \
          ../core/settingsaccesslevelfilereader.h \
          ../main/applicationtranslationsloader.h \
          ../core/starviewerapplication.h
          
SOURCES = crashreporter.cpp \
          qcrashreporter.cpp \
          ../core/settingsregistry.cpp \
          ../core/settings.cpp \
          ../core/settingsparser.cpp \
          ../core/defaultsettings.cpp \
          ../core/coresettings.cpp \
          ../core/settingsaccesslevelfilereader.cpp \
          ../main/applicationtranslationsloader.cpp \
          ../core/logging.cpp

TRANSLATIONS += crashreporter_ca_ES.ts \
                crashreporter_es_ES.ts \
                crashreporter_en_GB.ts 
                
INCLUDEPATH += ../core

macx {
    HEADERS +=
    OBJECTIVE_SOURCES += crashreportersender_mac.mm
    ICON = ../main/images/starviewer.icns
}
linux* {
    HEADERS +=
    SOURCES += crashreportersender_linux.cpp
    LIBS += -ldl
}
win32 {
    HEADERS +=
    SOURCES += crashreportersender_windows.cpp
               
    LIBS += -lWinInet
}

RESOURCES = crashreporter.qrc ../main/main.qrc

include(../corelibsconfiguration.pri)
include(../breakpad.pri)
include(../compilationtype.pri)

QT += network widgets
