TEMPLATE = app

include(../applicationstargetnames.inc)
TARGET = $${TARGET_STARVIEWER_CRASH_REPORTER}

DESTDIR = ../../bin

CONFIG += no_keywords

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

TRANSLATIONS += crashreporter_ca_ES.ts \
                crashreporter_es_ES.ts \
                crashreporter_en_GB.ts 
                
INCLUDEPATH += ../thirdparty/breakpad ../core

macx {
    HEADERS += ../thidparty/breakpad/common/mac/HTTPMultipartUpload.h
    OBJECTIVE_SOURCES += crashreportersender_mac.mm \
                         ../thirdparty/breakpad/common/mac/HTTPMultipartUpload.m
    ICON = ../main/images/starviewer.icns
}
linux* {
    HEADERS += ../thirdparty/breakpad/common/linux/http_upload.h
    SOURCES += crashreportersender_linux.cpp \
               ../thirdparty/breakpad/common/linux/http_upload.cc
}
win32 {
    HEADERS += ../thirdparty/breakpad/common/windows/http_upload.h
    SOURCES += crashreportersender_windows.cpp \
               ../thirdparty/breakpad/common/windows/http_upload.cc
               
    LIBS += -lWinInet
}

RESOURCES = crashreporter.qrc ../main/main.qrc

include(../corelibsconfiguration.inc)
include(../compilationtype.inc)
include(../log4cxx.inc)

QT += network
