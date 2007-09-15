# Fitxer generat pel gestor de qmake de kdevelop. 
# ------------------------------------------- 
# Subdirectori relatiu al directori principal del projecte: ./src/extensions/strokesegmentation
# L'objectiu � una biblioteca:  

FORMS += qstrokesegmentationextensionbase.ui 
HEADERS += qstrokesegmentationextension.h \
           strokesegmentationextensionmediator.h 
SOURCES += qstrokesegmentationextension.cpp \
           strokesegmentationextensionmediator.cpp 

RESOURCES += strokesegmentation.qrc

include(../../basicconfextensions.inc)
