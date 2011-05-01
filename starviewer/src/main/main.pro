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
HEADERS += applicationtranslationsloader.h
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

include(../extensions.inc)

# Funció per afegir una llibreria estàtica com a dependència
defineReplace(addLibraryDependency) {
    directoryName = $$1
    libraryName = $$2
    exists($$directoryName/$$libraryName) {
        unix:PRE_TARGETDEPS += $$directoryName/$$libraryName/lib$${libraryName}.a
        win32:PRE_TARGETDEPS += $$directoryName/$$libraryName/$${libraryName}.lib
        LIBS += -L$$directoryName/$$libraryName -l$${libraryName}
        INCLUDEPATH += $$directoryName/$$libraryName
        DEPENDPATH += $$directoryName/$$libraryName
    }
    # Propaguem els canvis a fora de la funció
    export(PRE_TARGETDEPS)
    export(LIBS)
    export(INCLUDEPATH)
    export(DEPENDPATH)

    return(0)
}

for(dir, PLAYGROUND_EXTENSIONS) {
    DUMMY = $$addLibraryDependency(../extensions/playground, $$dir)
}

for(dir, CONTRIB_EXTENSIONS) {
    DUMMY = $$addLibraryDependency(../extensions/contrib, $$dir)
}

for(dir, MAIN_EXTENSIONS) {
    DUMMY = $$addLibraryDependency(../extensions/main, $$dir)
}

# Dependències de llibreries core
DUMMY = $$addLibraryDependency(.., interface)
DUMMY = $$addLibraryDependency(.., inputoutput)
DUMMY = $$addLibraryDependency(.., core)

# Thirdparty libraries
DUMMY = $$addLibraryDependency(../thirdparty, breakpad)

win32{
  LIBS += -ladvapi32 \
          -lRpcrt4
}

include(../corelibsconfiguration.inc)
include(../dcmtk.inc)
include(../vtk.inc)
include(../itk.inc)
include(../gdcm.inc)
include(../log4cxx.inc)
include(../cuda.inc)
include(../compilationtype.inc)
include(../threadweaver.inc)
include(../thirdparty/qtsingleapplication/src/qtsingleapplication.pri)
include(../breakpad.inc)

CONFIG -= staticlib

include(installextensions.inc)

QT += xml opengl network webkit script xmlpatterns

#TODO: Qt 4.5.3 no afegeix la informacio de UI_DIR com a include a l'hora de compilar el main.cpp
INCLUDEPATH += ../../tmp/ui
