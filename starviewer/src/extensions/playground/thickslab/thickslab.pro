# Fitxer generat pel gestor de qmake de kdevelop. 
# ------------------------------------------- 
# Subdirectori relatiu al directori principal del projecte: ./src/extensions/playground/thickslab
# L'objectiu �s una biblioteca:  

FORMS += thickslabextensionbase.ui \
	 thickslabwidgetbase.ui

HEADERS += thickslabextensionmediator.h \
	   thickslabextension.h \
	   thickslabwidget.h

SOURCES += thickslabextensionmediator.cpp \
	   thickslabextension.cpp \
           thickslabwidget.cpp

RESOURCES += thickslab.qrc

include(../../basicconfextensions.inc)
