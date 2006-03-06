# Fitxer generat pel gestor de qmake de kdevelop. 
# ------------------------------------------- 
# Subdirectori relatiu al directori principal del projecte: ./src/tools
# L'objectiu és una biblioteca:  

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
           distance.h \
           point.h 
SOURCES += mathtools.cpp \
           distance.cpp \
           point.cpp 
include(../itk.inc)
include(../vtk.inc)
