# Fitxer generat pel gestor de qmake de kdevelop. 
# ------------------------------------------- 
# Subdirectori relatiu al directori principal del projecte: ./src/extensions/playground/multiq2dviewer
# L'objectiu �s una biblioteca:  

FORMS += multiq2dviewerextensionbase.ui \
         q2dviewerwidgetbase.ui \
         gridiconbase.ui \
         menugridwidgetbase.ui
         
HEADERS += multiq2dviewerextensionmediator.h \
           multiq2dviewerextension.h \
           q2dviewerwidget.h \
           patientitemmodel.h \
           patientitem.h \
           interactivewidget.h \
           gridicon.h \
           menugridwidget.h \
           itemmenu.h \
           tablemenu.h 
SOURCES += multiq2dviewerextensionmediator.cpp \
           multiq2dviewerextension.cpp \
           q2dviewerwidget.cpp \
           patientitemmodel.cpp \
           patientitem.cpp \
           interactivewidget.cpp \
           gridicon.cpp \
           menugridwidget.cpp \
           itemmenu.cpp \
           tablemenu.cpp 
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
