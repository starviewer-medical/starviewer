# Fitxer generat pel gestor de qmake de kdevelop. 
# ------------------------------------------- 
# Subdirectori relatiu al directori principal del projecte: ./src/extensions/playground/volume3dviewtesting
# L'objectiu �s una aplicaci??:  

FORMS += qvolume3dviewtestingextensionbase.ui 
HEADERS += volume3dviewtestingextensionmediator.h \
           qvolume3dviewtestingextension.h \
 vtkVolumeRayCastSingleVoxelShaderCompositeFunction.h
SOURCES += volume3dviewtestingextensionmediator.cpp \
           qvolume3dviewtestingextension.cpp \
 vtkVolumeRayCastSingleVoxelShaderCompositeFunction.cxx
RESOURCES += volume3dviewtesting.qrc
include(../../basicconfextensions.inc)
