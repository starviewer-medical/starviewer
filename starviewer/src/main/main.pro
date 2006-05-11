# Fitxer generat pel gestor de qmake de kdevelop. 
# ------------------------------------------- 
# Subdirectori relatiu al directori principal del projecte: ./src/main
# L'objectiu és una aplicaci??:  ../../bin/starviewer

RESOURCES = main.qrc 
TARGETDEPS += ../interface/libinterface.a \
              ../repositories/librepositories.a \
              ../inputoutput/libinputoutput.a \
              ../filters/libfilters.a \
              ../colour/libcolour.a \
              ../tools/libtools.a \
              ../visualization/libvisualization.a \
              ../registration/libregistration.a \
              ../segmentation/libsegmentation.a \
              ../../src/inputoutput/libinputoutput.a 
LIBS += ../../src/inputoutput/libinputoutput.a \
        -llog4cxx \
        ../interface/libinterface.a \
        ../repositories/librepositories.a \
        ../inputoutput/libinputoutput.a \
        ../filters/libfilters.a \
        ../colour/libcolour.a \
        ../tools/libtools.a \
        ../visualization/libvisualization.a \
        ../registration/libregistration.a \
        ../segmentation/libsegmentation.a 
INCLUDEPATH += ../../src/inputoutput \
               ../main \
               ../filters \
               ../interface \
               ../colour \
               ../tools \
               ../inputoutput \
               ../visualization \
               ../registration \
               ../segmentation \
               ../repositories 
MOC_DIR = ../../tmp/moc 
UI_DIR = ../../tmp/ui 
OBJECTS_DIR = ../../tmp/obj 
TARGET = ../../bin/starviewer 
CONFIG += debug \
          warn_on 
TEMPLATE = app 
SOURCES += main.cpp 
include(../vtk.inc)
include(../itk.inc)
include(../dcmtk.inc)
