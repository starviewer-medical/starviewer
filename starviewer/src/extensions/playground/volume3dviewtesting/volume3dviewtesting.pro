# Fitxer generat pel gestor de qmake de kdevelop. 
# ------------------------------------------- 
# Subdirectori relatiu al directori principal del projecte: ./src/extensions/playground/volume3dviewtesting
# L'objectiu ÃÂ©s una biblioteca:  

FORMS += qtransferfunctioneditorbyvaluesbase.ui \
         qtransferfunctionintervaleditorbase.ui \
         qvolume3dviewtestingextensionbase.ui 
HEADERS += qcolorspinbox.h \
           qtransferfunctioneditorbyvalues.h \
           qtransferfunctionintervaleditor.h \
           volume3dviewtestingextensionmediator.h \
           transferfunction.h \
           transferfunctionio.h \
           qtransferfunctioneditor.h \
           qtransferfunctioneditorbygradient.h \
           qvolume3dviewtestingextension.h \
           shadewidget.h \
           hoverpoints.h
SOURCES += qcolorspinbox.cpp \
           qtransferfunctioneditorbyvalues.cpp \
           qtransferfunctionintervaleditor.cpp \
           volume3dviewtestingextensionmediator.cpp \
           transferfunction.cpp \
           transferfunctionio.cpp \
           qtransferfunctioneditor.cpp \
           qtransferfunctioneditorbygradient.cpp \
           qvolume3dviewtestingextension.cpp \
           shadewidget.cpp \
           hoverpoints.cpp

RESOURCES += volume3dviewtesting.qrc

include(../../basicconfextensions.inc)
