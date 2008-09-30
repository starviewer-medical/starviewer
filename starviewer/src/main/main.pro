# Afegim dependències de les extensions

include(../applicationstargetnames.inc)

TARGET = $${TARGET_STARVIEWER}
DESTDIR = ../../bin
TEMPLATE = app

SOURCES += main.cpp
RESOURCES = main.qrc

include(crashhandler.inc)

win32{
RC_FILE = starviewer.rc
}

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

win32{
  LIBS += -ladvapi32 
}

include(../corelibsconfiguration.inc)
include(../dcmtk.inc)
include(../vtk.inc)
include(../itk.inc)
include(../log4cxx.inc)
include(../compilationtype.inc)

CONFIG -= staticlib

include(installextensions.inc)

QT += xml
