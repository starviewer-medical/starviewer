# Incloem les extensions

EXTENSIONS = diffusionperfusionsegmentation \
             edemasegmentation \
             landmarkregistration \
             strokesegmentation \
             optimalviewpoint

for(dir, EXTENSIONS) {
    exists(../extensions/$$dir) {
	TARGETDEPS += ../extensions/$$dir/lib$${dir}.a
	LIBS += ../extensions/$$dir/lib$${dir}.a
	INCLUDEPATH += ../extensions/$$dir
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

MOC_DIR = ../../tmp/moc 
UI_DIR = ../../tmp/ui 
OBJECTS_DIR = ../../tmp/obj 
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