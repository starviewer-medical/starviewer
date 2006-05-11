# Fitxer generat pel gestor de qmake de kdevelop. 
# ------------------------------------------- 
# Subdirectori relatiu al directori principal del projecte: ./src/tools
# L'objectiu és una biblioteca:  

LIBS += -llog4cxx 
INCLUDEPATH += /usr/include/log4cxx/ 
MOC_DIR = ../../tmp/moc 
UI_DIR = ../../tmp/ui 
OBJECTS_DIR = ../../tmp/obj 
QMAKE_CXXFLAGS_RELEASE += -Wno-deprecated 
QMAKE_CXXFLAGS_DEBUG += -Wno-deprecated 
CONFIG += debug \
          warn_on \
          qt \
          staticlib 
TEMPLATE = lib 
HEADERS += itkImageToVTKImageFilter.h \
           itkVTKImageToImageFilter.h \
           mathtools.h \
           itkQtAdaptor.h \
           harddiskinformation.h \
           distance.h \
           point.h \
           logging.h 
SOURCES += mathtools.cpp \
           harddiskinformation.cpp \
           distance.cpp \
           point.cpp 
include(../itk.inc)
include(../vtk.inc)
