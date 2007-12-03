# Fitxer generat pel gestor de qmake de kdevelop. 
# ------------------------------------------- 
# Subdirectori relatiu al directori principal del projecte: ./src/extensions/playground/thickslab
# L'objectiu �s una biblioteca:  

FORMS += thickslabextensionbase.ui \
	 qthickslabwidgetbase.ui

HEADERS += thickslabextensionmediator.h \
	   thickslabextension.h \
	   qthickslabwidget.h \
	   itkProjectionImageFilter.h \
	   itkMaximumProjectionImageFilter.h

SOURCES += thickslabextensionmediator.cpp \
	   thickslabextension.cpp \
           qthickslabwidget.cpp \
           itkProjectionImageFilter.txx

RESOURCES += thickslab.qrc

include(../../basicconfextensions.inc)
