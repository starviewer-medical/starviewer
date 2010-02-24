# Fitxer generat pel gestor de qmake de kdevelop. 
# ------------------------------------------- 
# Subdirectori relatiu al directori principal del projecte: ./src/extensions/angiosubstraction
# L'objectiu � una biblioteca:  

FORMS += qangiosubstractionextensionbase.ui 
HEADERS += angiosubstractionsettings.h \
           qangiosubstractionextension.h \
           angiosubstractionextensionmediator.h 
SOURCES += angiosubstractionsettings.cpp \
           qangiosubstractionextension.cpp \
           angiosubstractionextensionmediator.cpp 

RESOURCES += angiosubstraction.qrc

EXTENSION_DIR = $$PWD
include(../../basicconfextensions.inc)
