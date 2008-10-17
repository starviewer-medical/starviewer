# Fitxer generat pel gestor de qmake de kdevelop. 
# ------------------------------------------- 
# Subdirectori relatiu al directori principal del projecte: ./src/extensions/playground/experimental3d
# L'objectiu és una aplicaci??:  

FORMS += qexperimental3dextensionbase.ui 
HEADERS += experimental3dextensionmediator.h \
           qexperimental3dextension.h \
           qexperimental3dviewer.h \
           experimental3dvolume.h 
SOURCES += experimental3dextensionmediator.cpp \
           qexperimental3dextension.cpp \
           qexperimental3dviewer.cpp \
           experimental3dvolume.cpp 
RESOURCES += experimental3d.qrc
include(../../basicconfextensions.inc)
