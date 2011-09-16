# Afegim dependències de les extensions

include(../applicationstargetnames.inc)

TARGET = $${TARGET_STARVIEWER}
DESTDIR = ../../bin
TEMPLATE = app

# CrashHandler
SOURCES += crashhandler.cpp
HEADERS += crashhandler.h

macx {
    DEFINES += STARVIEWER_CRASH_REPORTER_EXE=\\\"$${TARGET_STARVIEWER_CRASH_REPORTER}.app/Contents/MacOS/$${TARGET_STARVIEWER_CRASH_REPORTER}\\\"
}
linux* {
    DEFINES += STARVIEWER_CRASH_REPORTER_EXE=\\\"$${TARGET_STARVIEWER_CRASH_REPORTER}\\\"
}
win32 {
    DEFINES += STARVIEWER_CRASH_REPORTER_EXE=\\\"$${TARGET_STARVIEWER_CRASH_REPORTER}.exe\\\"
}

# End CrashHandler

SOURCES += main.cpp \
           applicationtranslationsloader.cpp
HEADERS += applicationtranslationsloader.h \
           diagnosistests.h
RESOURCES = main.qrc

win32{
RC_FILE = starviewer.rc
}
macx {
    ICON = images/starviewer.icns
}

# Definim que per sistemes de compilació windows basats en visual studio 
# s'activi el flag /LARGEADDRESSAWARE, que permet que es puguin fer servir
# més de 2Gb de memòria per procés. Això serà efectiu en sistemes de 64 bits
win32-msvc2008:QMAKE_LFLAGS += /LARGEADDRESSAWARE

include(../../sourcelibsdependencies.pri)

# Thirdparty libraries
DUMMY = $$addLibraryDependency(../thirdparty, breakpad)

include(../corelibsconfiguration.inc)
include(../thirdparty/qtsingleapplication/src/qtsingleapplication.pri)
include(../breakpad.inc)

include(installextensions.inc)

QT += xml opengl network webkit script xmlpatterns

#TODO: Qt 4.5.3 no afegeix la informacio de UI_DIR com a include a l'hora de compilar el main.cpp
INCLUDEPATH += ../../tmp/ui
