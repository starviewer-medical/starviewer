# Fitxer generat pel gestor de qmake de kdevelop. 
# ------------------------------------------- 
# Subdirectori relatiu al directori principal del projecte: ./src/extensions/playground/multiq2dviewer
# L'objectiu � una biblioteca:  

FORMS += multiq2dviewerextensionbase.ui \
	 interactivewidgetbase.ui \
	 q2dviewerwidgetbase.ui

HEADERS += multiq2dviewerextensionmediator.h \
	   multiq2dviewerextension.h \
	   q2dviewerwidget.h \
	   patientitemmodel.h

SOURCES += multiq2dviewerextensionmediator.cpp \
           multiq2dviewerextension.cpp \
	   q2dviewerwidget.cpp \
	   patientitemmodel.cpp

TARGETDEPS += ../../../interface/libinterface.a \
              ../../../core/libcore.a \
              ../../../inputoutput/libinputoutput.a
LIBS += ../../../interface/libinterface.a \
        ../../../core/libcore.a \
        ../../../inputoutput/libinputoutput.a
INCLUDEPATH += ../../../interface \
               ../../../core \
               ../../../inputoutput
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
