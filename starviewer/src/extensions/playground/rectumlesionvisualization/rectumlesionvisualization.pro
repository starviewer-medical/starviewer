# Fitxer generat pel gestor de qmake de kdevelop. 
# ------------------------------------------- 
# Subdirectori relatiu al directori principal del projecte: ./src/extensions/rectumlesionvisualization
# L'objectiu � una biblioteca:  

FORMS += qrectumlesionvisualizationextensionbase.ui 
HEADERS += qrectumlesionvisualizationextension.h \
           rectumlesionvisualizationextensionmediator.h 

SOURCES += qrectumlesionvisualizationextension.cpp \
           rectumlesionvisualizationextensionmediator.cpp 

RESOURCES += rectumlesionvisualization.qrc

include(../../basicconfextensions.inc)
