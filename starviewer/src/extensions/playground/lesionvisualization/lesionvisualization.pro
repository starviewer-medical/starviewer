# Fitxer generat pel gestor de qmake de kdevelop. 
# ------------------------------------------- 
# Subdirectori relatiu al directori principal del projecte: ./src/extensions/lesionvisualization
# L'objectiu � una biblioteca:  

FORMS += qlesionvisualizationextensionbase.ui 
HEADERS += qlesionvisualizationextension.h \
           lesionvisualizationextensionmediator.h 

SOURCES += qlesionvisualizationextension.cpp \
           lesionvisualizationextensionmediator.cpp 

RESOURCES += lesionvisualization.qrc

include(../../basicconfextensions.inc)
