# Fitxer generat pel gestor de qmake de kdevelop. 
# ------------------------------------------- 
# Subdirectori relatiu al directori principal del projecte: ./src/repositories
# L'objectiu és una biblioteca:  

TEMPLATE = lib
CONFIG += debug \
warn_on \
thread \
staticlib \
stl
QMAKE_CXXFLAGS_DEBUG += -Wno-deprecated
QMAKE_CXXFLAGS_RELEASE += -Wno-deprecated
OBJECTS_DIR = ../../tmp/obj
UI_DIR = ../../tmp/ui
MOC_DIR = ../../tmp/moc
INCLUDEPATH += ../../src/tools
LIBS += ../tools/libtools.a
include(../itk.inc)
include(../vtk.inc)
TARGETDEPS += ../tools/libtools.a
SOURCES += volume.cpp \
           volumerepository.cpp \
           identifier.cpp \
           repositorybase.cpp \
           repository.cpp \
           volumesourceinformation.cpp \
           resourcecollection.cpp 
HEADERS += volume.h \
           volumerepository.h \
           identifier.h \
           repositorybase.h \
           repository.h \
           volumesourceinformation.h \
           resourcecollection.h 
