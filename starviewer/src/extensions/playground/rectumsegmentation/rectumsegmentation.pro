# Fitxer generat pel gestor de qmake de kdevelop. 
# ------------------------------------------- 
# Subdirectori relatiu al directori principal del projecte: ./src/extensions/rectumsegmentation
# L'objectiu � una biblioteca:  

FORMS += qrectumsegmentationextensionbase.ui 
HEADERS += qrectumsegmentationextension.h \
           rectumsegmentationextensionmediator.h \
           rectumSegmentationMethod.h 
SOURCES += qrectumsegmentationextension.cpp \
           rectumsegmentationextensionmediator.cpp \
           rectumSegmentationMethod.cpp 
TARGETDEPS += ../../../core/libcore.a
LIBS += ../../../core/libcore.a
INCLUDEPATH += ../../../core
MOC_DIR = ../../../../tmp/moc
UI_DIR = ../../../../tmp/ui
OBJECTS_DIR = ../../../../tmp/obj
QMAKE_CXXFLAGS_RELEASE += -Wno-deprecated
QMAKE_CXXFLAGS_DEBUG += -Wno-deprecated
CONFIG += warn_on \
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
