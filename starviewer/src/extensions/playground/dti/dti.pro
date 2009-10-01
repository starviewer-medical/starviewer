# Fitxer generat pel gestor de qmake de kdevelop. 
# ------------------------------------------- 
# Subdirectori relatiu al directori principal del projecte: ./src/extensions/playground/dti
# L'objectiu és una biblioteca:  

FORMS += dtiextensionbase.ui 
HEADERS += dtiextension.h \
           dtiextensionmediator.h 
SOURCES += dtiextension.cpp \
           dtiextensionmediator.cpp 

RESOURCES += dti.qrc

EXTENSION_DIR = $$PWD
include(../../basicconfextensions.inc)
