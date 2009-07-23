# Fitxer generat pel gestor de qmake de kdevelop. 
# ------------------------------------------- 
# Subdirectori relatiu al directori principal del projecte: ./src/extensions/playground/optimalviewpoint
# L'objectiu � una biblioteca:  

FORMS += qmprextensionbase.ui

HEADERS += qmprextension.h \
           mprsettings.h \
           mprextensionmediator.h

SOURCES += qmprextension.cpp \
           mprsettings.cpp \
           mprextensionmediator.cpp

RESOURCES += mpr.qrc

EXTENSION_DIR = $$PWD
include(../../basicconfextensions.inc)
