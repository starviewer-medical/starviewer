# Fitxer generat pel gestor de qmake de kdevelop. 
# ------------------------------------------- 
# Subdirectori relatiu al directori principal del projecte: ./src/segmentation
# L'objectiu és una biblioteca:  


SOURCES += imagecomplexity3d.cpp \
           imagecomplexity3dhistogram.cpp \
           imagecomplexity3dnode.cpp \
           imagecomplexity3dregion.cpp 
HEADERS += imagecomplexity3d.h \
           imagecomplexity3dhistogram.h \
           imagecomplexity3dnode.h \
           imagecomplexity3dregion.h 

TEMPLATE = lib 
CONFIG += debug \
          warn_on \
          staticlib 
QMAKE_CXXFLAGS_DEBUG += -Wno-deprecated 
QMAKE_CXXFLAGS_RELEASE += -Wno-deprecated 
OBJECTS_DIR = ../../tmp/obj 
UI_DIR = ../../tmp/ui 
MOC_DIR = ../../tmp/moc 
INCLUDEPATH += ../../src/tools 
LIBS += ../../src/tools/libtools.a 
TARGETDEPS += ../../src/tools/libtools.a 

include(../vtk.inc)

include(../itk.inc)


