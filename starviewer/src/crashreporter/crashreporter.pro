TEMPLATE = app

include(../applicationstargetnames.pri)
TARGET = $${TARGET_STARVIEWER_CRASH_REPORTER}

DESTDIR = ../../bin

macx {
    DESTDIR = $${DESTDIR}/$${TARGET_STARVIEWER}.app/Contents/MacOS
}

FORMS = qcrashreporterbase.ui
HEADERS = qcrashreporter.h \
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
          ../core/logging.cpp \
          ../core/starviewerapplication.cpp

TRANSLATIONS += crashreporter_ca_ES.ts \
                crashreporter_es_ES.ts \
                crashreporter_en_GB.ts 
                
INCLUDEPATH += ../core

macx {
    ICON = ../main/images/logo/logo.icns
}

RESOURCES = crashreporter.qrc ../main/main.qrc

include(../corelibsconfiguration.pri)
include(../compilationtype.pri)

QT += network widgets
