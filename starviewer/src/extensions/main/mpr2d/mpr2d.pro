# Fitxer generat pel gestor de qmake de kdevelop. 
# ------------------------------------------- 
# Subdirectori relatiu al directori principal del projecte: ./src/extensions/playground/optimalviewpoint
# L'objectiu � una biblioteca:  

FORMS += qmprextensionbase.ui

HEADERS += qmprextension.h \
           mprsettings.h \
           mpr2dextensionmediator.h

SOURCES += qmprextension.cpp \
           mprsettings.cpp \
           mpr2dextensionmediator.cpp

RESOURCES += mpr2d.qrc

EXTENSION_DIR = $$PWD
include(../../basicconfextensions.pri)
