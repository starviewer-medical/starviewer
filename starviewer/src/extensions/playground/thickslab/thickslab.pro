# Fitxer generat pel gestor de qmake de kdevelop. 
# ------------------------------------------- 
# Subdirectori relatiu al directori principal del projecte: ./src/extensions/playground/thickslab
# L'objectiu �s una biblioteca:  

FORMS += thickslabextensionbase.ui \
	 qthickslabwidgetbase.ui

HEADERS += thickslabextensionmediator.h \
	   thickslabextension.h \
	   qthickslabwidget.h

SOURCES += thickslabextensionmediator.cpp \
	   thickslabextension.cpp \
           qthickslabwidget.cpp

RESOURCES += thickslab.qrc

include(../../basicconfextensions.inc)
