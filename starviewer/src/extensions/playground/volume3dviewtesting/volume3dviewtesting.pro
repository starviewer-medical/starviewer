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
