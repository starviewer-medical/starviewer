# Fitxer generat pel gestor de qmake de kdevelop. 
# ------------------------------------------- 
# Subdirectori relatiu al directori principal del projecte: ./src/extensions/testing
# L'objectiu és una biblioteca:  

FORMS += testingextension.ui 
HEADERS += testingextension.h \
           testingextensionmediator.h 
SOURCES += testingextension.cpp \
           testingextensionmediator.cpp 
TARGETDEPS += ../../repositories/librepositories.a \
../../tools/libtools.a \
../../core/libcore.a
LIBS += ../../repositories/librepositories.a \
../../tools/libtools.a \
../../core/libcore.a
INCLUDEPATH += ../../tools \
../../core \
../../repositories
MOC_DIR = ../../../tmp/moc
UI_DIR = ../../../tmp/ui
OBJECTS_DIR = ../../../tmp/obj
QMAKE_CXXFLAGS_RELEASE += -Wno-deprecated
QMAKE_CXXFLAGS_DEBUG += -Wno-deprecated
CONFIG += debug \
warn_on \
qt \
opengl \
thread \
x11 \
staticlib \
exceptions \
stl
TEMPLATE = lib
include(../../vtk.inc)
include(../../itk.inc)
