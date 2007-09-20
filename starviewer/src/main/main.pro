# Afegim dependències de les extensions

include(../extensions.inc)

for(dir, PLAYGROUND_EXTENSIONS) {
    exists(../extensions/playground/$$dir) {
        TARGETDEPS += ../extensions/playground/$$dir/lib$${dir}.a
        LIBS += ../extensions/playground/$$dir/lib$${dir}.a
        INCLUDEPATH += ../extensions/playground/$$dir
        DEPENDPATH += ../extensions/playground/$$dir
    }
}

for(dir, CONTRIB_EXTENSIONS) {
    exists(../extensions/contrib/$$dir) {
        TARGETDEPS += ../extensions/contrib/$$dir/lib$${dir}.a
        LIBS += ../extensions/contrib/$$dir/lib$${dir}.a
        INCLUDEPATH += ../extensions/contrib/$$dir
        DEPENDPATH += ../extensions/contrib/$$dir
    }
}

for(dir, MAIN_EXTENSIONS) {
    exists(../extensions/main/$$dir) {
        TARGETDEPS += ../extensions/main/$$dir/lib$${dir}.a
        LIBS += ../extensions/main/$$dir/lib$${dir}.a
        INCLUDEPATH += ../extensions/main/$$dir
        DEPENDPATH += ../extensions/main/$$dir
    }
}

RESOURCES = main.qrc
TARGETDEPS += ../interface/libinterface.a \
              ../core/libcore.a \
              ../inputoutput/libinputoutput.a
LIBS += -llog4cxx \
        ../interface/libinterface.a \
        ../inputoutput/libinputoutput.a \
        ../core/libcore.a
INCLUDEPATH += ../main \
               ../interface \
               ../core \
               ../inputoutput
DEPENDPATH += images

MOC_DIR = ../../tmp/moc
UI_DIR = ../../tmp/ui
OBJECTS_DIR = ../../tmp/obj
RCC_DIR = ../../tmp/rcc
TARGET = ../../bin/starviewer
CONFIG += warn_on
TEMPLATE = app
SOURCES += main.cpp
include(../vtk.inc)
include(../itk.inc)
include(../dcmtk.inc)
include(../compilationtype.inc)

# Instalem les extensions

unix {
    !system(sh installextensions.sh):error(ERROR FATAL! Falta el fitxer installextension.sh. No es pot compilar!!!)
}