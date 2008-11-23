# Fitxer generat pel gestor de qmake de kdevelop. 
# ------------------------------------------- 
# Subdirectori relatiu al directori principal del projecte: ./src/extensions/strokesegmentation
# L'objectiu � una biblioteca:  

FORMS += qvsireconstructionextensionbase.ui 
HEADERS += qvsireconstructionextension.h \
           vsireconstructionextensionmediator.h 
SOURCES += qvsireconstructionextension.cpp \
           vsireconstructionextensionmediator.cpp 

RESOURCES += vsireconstruction.qrc

include(../../basicconfextensions.inc)
