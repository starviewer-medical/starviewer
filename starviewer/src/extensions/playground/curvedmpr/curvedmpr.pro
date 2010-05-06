# Fitxer generat pel gestor de qmake de kdevelop. 
# ------------------------------------------- 
# Subdirectori relatiu al directori principal del projecte: ./src/extensions/playground/curvedmpr
# L'objectiu és una biblioteca:

FORMS += curvedmprextensionbase.ui

HEADERS += curvedmprextension.h \
           curvedmprsettings.h \
           curvedmprextensionmediator.h

SOURCES += curvedmprextension.cpp \
           curvedmprsettings.cpp \
           curvedmprextensionmediator.cpp

RESOURCES += curvedmpr.qrc

EXTENSION_DIR = $$PWD
include(../../basicconfextensions.inc)
