# Fitxer generat pel gestor de qmake de kdevelop. 
# ------------------------------------------- 
# Subdirectori relatiu al directori principal del projecte: ./src/extensions/playground/gputesting
# L'objectiu és una aplicaci??:  

RESOURCES = gputesting.qrc 
QT = $$QT opengl 
CONFIG += release \
          warn_on 
TEMPLATE = app 
FORMS += qgputestingextensionbase.ui 
HEADERS += gputestingextensionmediator.h \
           qgputestingextension.h \
           qgputestingviewer.h 
SOURCES += gputestingextensionmediator.cpp \
           qgputestingextension.cpp \
           qgputestingviewer.cpp 
include(../../basicconfextensions.inc)
