# Fitxer generat pel gestor de qmake de kdevelop. 
# ------------------------------------------- 
# Subdirectori relatiu al directori principal del projecte: ./src
# L'objectiu és un subdirectori del projecte 

SUBDIRS += tools \
           repositories \
           core \
           registration \
           segmentation \
           visualization \
           inputoutput \
           colour \
           filters \
           interface \
           main 
TEMPLATE = subdirs 
CONFIG += debug \
          warn_on 
OBJECTS_DIR = ../tmp/obj 
UI_DIR = ../tmp/ui 
MOC_DIR = ../tmp/moc 
LIBS += inputoutput/libinputoutput.a \
        repositories/librepositories.a \
        core/core.a \
        filters/libfilters.a \
        colour/libcolour.a \
        tools/libtools.a \
        interface/libinterface.a \
        visualization/libvisualization.a \
        registration/libregistration.a \
        segmentation/libsegmentation.a 
TARGETDEPS += ../src/main/../bin/starviewer \
              ../src/filters/libfilters.a \
              ../src/colour/libcolour.a \
              ../src/tools/libtools.a \
              ../src/visualization/libvisualization.a \
              ../src/registration/libregistration.a \
              ../src/segmentation/libsegmentation.a \
              ../src/interface/libinterface.a \
              ../src/inputoutput/libinputoutput.a \
              ../src/core/core.a \
              ../src/repositories/librepositories.a 
