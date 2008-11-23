# Fitxer generat pel gestor de qmake de kdevelop. 
# ------------------------------------------- 
# Subdirectori relatiu al directori principal del projecte: ./src/extensions/playground/glialsegmentation
# L'objectiu � una biblioteca:  

FORMS += qglialestimationextensionbase.ui 
HEADERS += qglialestimationextension.h \
           glialestimationextensionmediator.h 
SOURCES += qglialestimationextension.cpp \
           glialestimationextensionmediator.cpp 

RESOURCES += glialestimation.qrc

include(../../basicconfextensions.inc)
