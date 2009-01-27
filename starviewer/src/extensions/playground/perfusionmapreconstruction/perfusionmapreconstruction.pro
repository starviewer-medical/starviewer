# Fitxer generat pel gestor de qmake de kdevelop. 
# ------------------------------------------- 
# Subdirectori relatiu al directori principal del projecte: ./src/extensions/playground/perfusionmapreconstruction
# L'objectiu Ã©s una biblioteca:  

FORMS += qperfusionmapreconstructionextensionbase.ui \
         qgraphicplotwidgetbase.ui
HEADERS += qperfusionmapreconstructionextension.h \
           perfusionmapreconstructionextensionmediator.h  \
           perfusionmapcalculatorthread.h \
           perfusionmapcalculatormainthread.h \
           qgraphicplotwidget.h
SOURCES += qperfusionmapreconstructionextension.cpp \
           perfusionmapreconstructionextensionmediator.cpp  \
           perfusionmapcalculatorthread.cpp \
           perfusionmapcalculatormainthread.cpp \
           qgraphicplotwidget.cpp
RESOURCES += perfusionmapreconstruction.qrc

include(../../basicconfextensions.inc)

