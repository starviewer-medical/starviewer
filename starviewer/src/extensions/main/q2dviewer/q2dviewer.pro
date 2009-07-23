# Fitxer generat pel gestor de qmake de kdevelop. 
# ------------------------------------------- 
# Subdirectori relatiu al directori principal del projecte: ./src/extensions/playground/q2dviewer
# L'objectiu �s una biblioteca:  

FORMS += q2dviewerextensionbase.ui 
	 

HEADERS += q2dviewerextensionmediator.h \
	   q2dviewerextension.h \
	   q2dviewersettings.h \
	   gridicon.h \
	   tablemenu.h \
	   itemmenu.h \
	   menugridwidget.h

SOURCES += q2dviewerextensionmediator.cpp \
	   q2dviewerextension.cpp \
	   q2dviewersettings.cpp \
	   gridicon.cpp \
	   tablemenu.cpp \
	   itemmenu.cpp \
	   menugridwidget.cpp

RESOURCES += q2dviewer.qrc

EXTENSION_DIR = $$PWD
include(../../basicconfextensions.inc)
