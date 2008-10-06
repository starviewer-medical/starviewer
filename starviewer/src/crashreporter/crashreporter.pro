TEMPLATE = app

include(../applicationstargetnames.inc)
TARGET = $${TARGET_STARVIEWER_CRASH_REPORTER}

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

FORMS = qcrashreporterbase.ui
HEADERS = qcrashreporter.h \
          crashreportersender.h 
SOURCES = crashreporter.cpp \
          qcrashreporter.cpp

INCLUDEPATH += ../main/src_breakpad 

macx {
    HEADERS += ../main/src_breakpad/common/mac/HTTPMultipartUpload.h
    SOURCES += crashreportersender_mac.mm \
               ../main/src_breakpad/common/mac/HTTPMultipartUpload.m
    ICON = ../main/images/starviewer.icns
}
linux* {
    HEADERS += ../main/src_breakpad/common/linux/http_upload.h
    SOURCES += crashreportersender_linux.cpp \
               ../main/src_breakpad/common/linux/http_upload.cc
}
win32 {
    HEADERS += ../main/src_breakpad/common/windows/http_upload.h
    SOURCES += crashreportersender_windows.cpp \
               ../main/src_breakpad/common/windows/http_upload.cc
}

RESOURCES = ../main/main.qrc

include(../corelibsconfiguration.inc)
include(../compilationtype.inc)
