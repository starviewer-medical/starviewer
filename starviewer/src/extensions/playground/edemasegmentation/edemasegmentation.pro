# Fitxer generat pel gestor de qmake de kdevelop. 
# ------------------------------------------- 
# Subdirectori relatiu al directori principal del projecte: ./src/extensions/edemasegmentation
# L'objectiu � una biblioteca:  

FORMS += qedemasegmentationextensionbase.ui 
HEADERS += qedemasegmentationextension.h \
           edemasegmentationsettings.h \
           edemasegmentationextensionmediator.h 
SOURCES += qedemasegmentationextension.cpp \
           edemasegmentationsettings.cpp \
           edemasegmentationextensionmediator.cpp 

RESOURCES += edemasegmentation.qrc

EXTENSION_DIR = $$PWD
include(../../basicconfextensions.inc)
