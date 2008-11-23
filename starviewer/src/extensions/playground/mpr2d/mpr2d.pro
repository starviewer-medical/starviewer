# Fitxer generat pel gestor de qmake de kdevelop. 
# ------------------------------------------- 
# Subdirectori relatiu al directori principal del projecte: ./src/extensions/playground/mpr2d
# L'objectiu és una biblioteca:

FORMS += qmpr2dextensionbase.ui

HEADERS += qmpr2dextension.h \
           mpr2dextensionmediator.h

SOURCES += qmpr2dextension.cpp \
           mpr2dextensionmediator.cpp

RESOURCES += mpr2d.qrc

include(../../basicconfextensions.inc)
