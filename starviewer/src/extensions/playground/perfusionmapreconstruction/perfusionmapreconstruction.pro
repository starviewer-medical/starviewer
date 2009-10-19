# Fitxer generat pel gestor de qmake de kdevelop. 
# ------------------------------------------- 
# Subdirectori relatiu al directori principal del projecte: ./src/extensions/playground/perfusionmapreconstruction
# L'objectiu Ã©s una biblioteca:  

FORMS += qperfusionmapreconstructionextensionbase.ui \
         qgraphicplotwidgetbase.ui
HEADERS += qperfusionmapreconstructionextension.h \
           perfusionmapreconstructionextensionmediator.h  \
           perfusionmapreconstructionsettings.h \
           perfusionmapcalculatorthread.h \
           perfusionmapcalculatormainthread.h \
           qgraphicplotwidget.h
SOURCES += qperfusionmapreconstructionextension.cpp \
           perfusionmapreconstructionextensionmediator.cpp  \
           perfusionmapreconstructionsettings.cpp \
           perfusionmapcalculatorthread.cpp \
           perfusionmapcalculatormainthread.cpp \
           qgraphicplotwidget.cpp
RESOURCES += perfusionmapreconstruction.qrc

EXTENSION_DIR = $$PWD
include(../../basicconfextensions.inc)

