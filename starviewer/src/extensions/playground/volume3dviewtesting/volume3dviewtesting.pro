# Fitxer generat pel gestor de qmake de kdevelop. 
# ------------------------------------------- 
# Subdirectori relatiu al directori principal del projecte: ./src/extensions/playground/volume3dviewtesting
# L'objectiu és una aplicaci??:  

FORMS += qvolume3dviewtestingextensionbase.ui 
HEADERS += volume3dviewtestingextensionmediator.h \
           qvolume3dviewtestingextension.h \
           volume3dviewtestingsettings.h \
           renderingstyle.h 
SOURCES += volume3dviewtestingextensionmediator.cpp \
           qvolume3dviewtestingextension.cpp \
           volume3dviewtestingsettings.cpp \
           renderingstyle.cpp 
RESOURCES += volume3dviewtesting.qrc

EXTENSION_DIR = $$PWD
include(../../basicconfextensions.inc)
