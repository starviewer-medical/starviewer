# Fitxer generat pel gestor de qmake de kdevelop. 
# ------------------------------------------- 
# Subdirectori relatiu al directori principal del projecte: ./src/main
# L'objectiu és una aplicaci??:  ../../bin/starviewer

TARGETDEPS += ../interface/libinterface.a \
              ../repositories/librepositories.a \
              ../core/libcore.a \
              ../inputoutput/libinputoutput.a \
              ../filters/libfilters.a \
              ../colour/libcolour.a \
              ../tools/libtools.a \
              ../visualization/libvisualization.a \
              ../registration/libregistration.a \
              ../segmentation/libsegmentation.a \
              ../../src/inputoutput/libinputoutput.a 
LIBS += ../../src/inputoutput/libinputoutput.a \
        -llog4cpp \
        ../interface/libinterface.a \
        ../repositories/librepositories.a \
        ../core/libcore.a \
        ../inputoutput/libinputoutput.a \
        ../filters/libfilters.a \
        ../colour/libcolour.a \
        ../tools/libtools.a \
        ../visualization/libvisualization.a \
        ../registration/libregistration.a \
        ../segmentation/libsegmentation.a 
INCLUDEPATH += ../main \
               ../filters \
               ../interface \
               ../colour \
               ../tools \
               ../inputoutput \
               ../visualization \
               ../registration \
               ../segmentation \
               ../core \
               ../repositories 
MOC_DIR = ../../tmp/moc 
UI_DIR = ../../tmp/ui 
OBJECTS_DIR = ../../tmp/obj 
TARGET = ../../bin/starviewer 
CONFIG += debug \
          warn_on 
TEMPLATE = app 
SOURCES += main.cpp 
RESOURCES = main.qrc
 
include(../vtk.inc)
include(../itk.inc)
include(../dcmtk.inc)

