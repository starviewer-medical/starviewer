# Fitxer generat pel gestor de qmake de kdevelop. 
# ------------------------------------------- 
# Subdirectori relatiu al directori principal del projecte: ./src/extensions/segmentationframework
# L'objectiu és una biblioteca:  

FORMS += qsegmentationframeworkextensionbase.ui 
HEADERS += qsegmentationframeworkextension.h \
           segmentationframeworkextensionmediator.h  \
           connectedthreshold.h \
           confidenceconnected.h \
           isolatedconnected.h \
           neighborhoodconnected.h \
           volumcalculator.h \
           contourntool.h \
           determinatecontour.h \
           areaspline.h \
           llescacontorn.h \
           llenca.h
SOURCES += qsegmentationframeworkextension.cpp \
           segmentationframeworkextensionmediator.cpp  \
           connectedthreshold.cpp \
           confidenceconnected.cpp \
           isolatedconnected.cpp \
           neighborhoodconnected.cpp \
           volumcalculator.cpp \
           contourntool.cpp \
           determinatecontour.cpp \
           areaspline.cpp \
           llescacontorn.cpp

RESOURCES += segmentationframework.qrc

include(../../basicconfextensions.inc)
