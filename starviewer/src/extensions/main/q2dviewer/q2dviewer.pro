# Fitxer generat pel gestor de qmake de kdevelop. 
# ------------------------------------------- 
# Subdirectori relatiu al directori principal del projecte: ./src/extensions/playground/q2dviewer
# L'objectiu �s una biblioteca:  

FORMS += q2dviewerextensionbase.ui \
	 q2dviewerwidgetbase.ui \
	 gridiconbase.ui \
	 menugridwidgetbase.ui

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

TARGETDEPS += ../../../core/libcore.a
LIBS += ../../../core/libcore.a
INCLUDEPATH += ../../../core
MOC_DIR = ../../../../tmp/moc
UI_DIR = ../../../../tmp/ui
OBJECTS_DIR = ../../../../tmp/obj
QMAKE_CXXFLAGS_RELEASE += -Wno-deprecated
QMAKE_CXXFLAGS_DEBUG += -Wno-deprecated
CONFIG += release \
          warn_on \
          qt \
          opengl \
          thread \
          x11 \
          staticlib \
          exceptions \
          stl
TEMPLATE = lib
include(../../../vtk.inc)
include(../../../itk.inc)
include(../../../dcmtk.inc)
include(../../../compilationtype.inc)
