# Fitxer generat pel gestor de qmake de kdevelop. 
# ------------------------------------------- 
# Subdirectori relatiu al directori principal del projecte: ./src/extensions/playground/volume3dviewtesting
# L'objectiu és una aplicaci??:  

FORMS += q3dviewerextensionbase.ui 
HEADERS += q3dviewerextensionmediator.h \
           q3dviewerextension.h \
           q3dviewerextensionsettings.h \
           renderingstyle.h 
SOURCES += q3dviewerextensionmediator.cpp \
           q3dviewerextension.cpp \
           q3dviewerextensionsettings.cpp \
           renderingstyle.cpp 
RESOURCES += q3dviewer.qrc

EXTENSION_DIR = $$PWD
include(../../basicconfextensions.inc)
