# Fitxer generat pel gestor de qmake de kdevelop. 
# ------------------------------------------- 
# Subdirectori relatiu al directori principal del projecte: ./src
# L'objectiu és un subdirectori del projecte 

SUBDIRS += core \
           repositories \
           extensions \
           inputoutput \
           interface \
           main 
TEMPLATE = subdirs 
CONFIG += debug \
          warn_on 
OBJECTS_DIR = ../tmp/obj 
UI_DIR = ../tmp/ui 
MOC_DIR = ../tmp/moc 
LIBS += core/core.a \
        inputoutput/libinputoutput.a \
        repositories/librepositories.a \
        interface/libinterface.a
TARGETDEPS += ../src/main/../bin/starviewer \
              ../src/interface/libinterface.a \
              ../src/inputoutput/libinputoutput.a \
              ../src/repositories/librepositories.a \
              ../src/core/core.a
