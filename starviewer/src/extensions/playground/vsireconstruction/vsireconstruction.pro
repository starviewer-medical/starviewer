# Fitxer generat pel gestor de qmake de kdevelop. 
# ------------------------------------------- 
# Subdirectori relatiu al directori principal del projecte: ./src/extensions/strokesegmentation
# L'objectiu � una biblioteca:  

FORMS += qvsireconstructionextensionbase.ui 
HEADERS += vsireconstructionsettings.h \
           qvsireconstructionextension.h \
           vsireconstructionextensionmediator.h 
SOURCES += vsireconstructionsettings.cpp \
           qvsireconstructionextension.cpp \
           vsireconstructionextensionmediator.cpp 

RESOURCES += vsireconstruction.qrc

EXTENSION_DIR = $$PWD
include(../../basicconfextensions.inc)
