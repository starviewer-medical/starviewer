# Fitxer generat pel gestor de qmake de kdevelop. 
# ------------------------------------------- 
# Subdirectori relatiu al directori principal del projecte: ./src/extensions/playground/q2dviewer
# L'objectiu �s una biblioteca:  

FORMS += q2dviewerextensionbase.ui \
	 q2dviewerwidgetbase.ui

HEADERS += q2dviewerextensionmediator.h \
	   q2dviewerextension.h \
	   q2dviewerwidget.h \
	   gridicon.h \
	   tablemenu.h \
	   itemmenu.h \
	   menugridwidget.h

SOURCES += q2dviewerextensionmediator.cpp \
	   q2dviewerextension.cpp \
           q2dviewerwidget.cpp \
	   gridicon.cpp \
	   tablemenu.cpp \
	   itemmenu.cpp \
	   menugridwidget.cpp

RESOURCES += q2dviewer.qrc

include(../../basicconfextensions.inc)
