# Fitxer generat pel gestor de qmake de kdevelop. 
# ------------------------------------------- 
# Subdirectori relatiu al directori principal del projecte: ./src/extensions/playground/optimalviewpoint
# L'objectiu � una biblioteca:  

FORMS += qmpr3d2dextensionbase.ui

HEADERS += qmpr3d2dextension.h \
           mpr3d2dextensionmediator.h

SOURCES += qmpr3d2dextension.cpp \
           mpr3d2dextensionmediator.cpp

RESOURCES += mpr3d2d.qrc

include(../../basicconfextensions.inc)
