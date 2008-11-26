# Fitxer generat pel gestor de qmake de kdevelop. 
# ------------------------------------------- 
# Subdirectori relatiu al directori principal del projecte: ./src/extensions/playground/perfusionmapreconstruction
# L'objectiu és una biblioteca:  

FORMS += qperfusionmapreconstructionextensionbase.ui 
HEADERS += qperfusionmapreconstructionextension.h \
           perfusionmapreconstructionextensionmediator.h 
SOURCES += qperfusionmapreconstructionextension.cpp \
           perfusionmapreconstructionextensionmediator.cpp 

RESOURCES += perfusionmapreconstruction.qrc

include(../../basicconfextensions.inc)

