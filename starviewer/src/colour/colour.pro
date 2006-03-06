# Fitxer generat pel gestor de qmake de kdevelop. 
# ------------------------------------------- 
# Subdirectori relatiu al directori principal del projecte: ./src/colour
# L'objectiu és una biblioteca:  

TEMPLATE = lib 
CONFIG += debug \
          warn_on \
          staticlib 
QMAKE_CXXFLAGS_DEBUG += -Wno-deprecated 
QMAKE_CXXFLAGS_RELEASE += -Wno-deprecated 
OBJECTS_DIR = ../../tmp/obj 
UI_DIR = ../../tmp/ui 
MOC_DIR = ../../tmp/moc 
include(../vtk.inc)
include(../itk.inc)
