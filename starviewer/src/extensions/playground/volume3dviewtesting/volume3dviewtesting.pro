# Fitxer generat pel gestor de qmake de kdevelop. 
# ------------------------------------------- 
# Subdirectori relatiu al directori principal del projecte: ./src/extensions/playground/volume3dviewtesting
# L'objectiu és una aplicaci??:  

FORMS += qtransferfunctioneditorbyvaluesbase.ui \
         qtransferfunctionintervaleditorbase.ui \
         qvolume3dviewtestingextensionbase.ui \
         qcluteditordialogbase.ui 
HEADERS += qcolorspinbox.h \
           volume3dviewtestingextensionmediator.h \
           qvolume3dviewtestingextension.h \
           shadewidget.h \
           hoverpoints.h \
           qcluteditordialog.h 
SOURCES += qcolorspinbox.cpp \
           volume3dviewtestingextensionmediator.cpp \
           qvolume3dviewtestingextension.cpp \
           shadewidget.cpp \
           hoverpoints.cpp \
           qcluteditordialog.cpp 
RESOURCES += volume3dviewtesting.qrc
include(../../basicconfextensions.inc)
