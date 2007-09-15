# Fitxer generat pel gestor de qmake de kdevelop. 
# ------------------------------------------- 
# Subdirectori relatiu al directori principal del projecte: ./src/extensions/playground/optimalviewpoint
# L'objectiu � una biblioteca:  

FORMS += qmprextensionbase.ui

HEADERS += qmprextension.h \
           mprextensionmediator.h

SOURCES += qmprextension.cpp \
           mprextensionmediator.cpp

RESOURCES += mpr.qrc

include(../../basicconfextensions.inc)
