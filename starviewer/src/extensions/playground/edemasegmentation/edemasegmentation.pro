# Fitxer generat pel gestor de qmake de kdevelop. 
# ------------------------------------------- 
# Subdirectori relatiu al directori principal del projecte: ./src/extensions/edemasegmentation
# L'objectiu � una biblioteca:  

FORMS += qedemasegmentationextensionbase.ui 
HEADERS += qedemasegmentationextension.h \
           edemasegmentationextensionmediator.h 
SOURCES += qedemasegmentationextension.cpp \
           edemasegmentationextensionmediator.cpp 

RESOURCES += edemasegmentation.qrc

include(../../basicconfextensions.inc)
