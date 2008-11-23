# Fitxer generat pel gestor de qmake de kdevelop. 
# ------------------------------------------- 
# Subdirectori relatiu al directori principal del projecte: ./src/extensions/rectumsegmentation
# L'objectiu � una biblioteca:  

FORMS += qrectumsegmentationextensionbase.ui 
HEADERS += qrectumsegmentationextension.h \
           rectumsegmentationextensionmediator.h \
           rectumSegmentationMethod.h 
SOURCES += qrectumsegmentationextension.cpp \
           rectumsegmentationextensionmediator.cpp \
           rectumSegmentationMethod.cpp 

RESOURCES += rectumsegmentation.qrc

include(../../basicconfextensions.inc)
