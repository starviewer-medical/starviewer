# Fitxer generat pel gestor de qmake de kdevelop. 
# ------------------------------------------- 
# Subdirectori relatiu al directori principal del projecte: ./src/extensions/playground/multiq2dviewer
# L'objectiu �s una biblioteca:  

HEADERS += multiq2dviewerextensionmediator.h \
           patientitemmodel.h \
           patientitem.h \
           interactivewidget.h
SOURCES += multiq2dviewerextensionmediator.cpp \
           patientitemmodel.cpp \
           patientitem.cpp \
           interactivewidget.cpp

RESOURCES += multiq2dviewer.qrc

include(../../basicconfextensions.inc)
