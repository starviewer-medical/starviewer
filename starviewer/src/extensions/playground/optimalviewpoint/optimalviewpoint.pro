# Fitxer generat pel gestor de qmake de kdevelop. 
# ------------------------------------------- 
# Subdirectori relatiu al directori principal del projecte: ./src/extensions/playground/optimalviewpoint
# L'objectiu és una aplicaci??:  

FORMS += optimalviewpointinputparametersformbase.ui \
         qoptimalviewpointextensionbase.ui \
         qshowhidewidgetbase.ui 
HEADERS += doublehistogram.h \
           histogram.h \
           optimalviewpoint.h \
           optimalviewpointinputparametersform.h \
           optimalviewpointparameters.h \
           optimalviewpointplane.h \
           optimalviewpointplanehelper.h \
           optimalviewpointviewer.h \
           optimalviewpointvolume.h \
           vtkInteractorStyleJoystickActorGgg.h \
           vtkInteractorStyleSwitchGgg.h \
           optimalviewpointextensionmediator.h \
           qoptimalviewpointextension.h \
           slicer.h \
           qshowhidewidget.h \
           vtkVolumeRayCastCompositeFunctionViewpointSaliency.h \
           vtkVolumeRayCastCompositeFunctionFx.h \
           saliencyvoxelshader.h \
           vtkInteractorStyleFasterTrackballActor.h 
SOURCES += doublehistogram.cpp \
           histogram.cpp \
           optimalviewpoint.cpp \
           optimalviewpointinputparametersform.cpp \
           optimalviewpointparameters.cpp \
           optimalviewpointplane.cpp \
           optimalviewpointplanehelper.cpp \
           optimalviewpointviewer.cpp \
           optimalviewpointvolume.cpp \
           vtkInteractorStyleJoystickActorGgg.cxx \
           vtkInteractorStyleSwitchGgg.cxx \
           optimalviewpointextensionmediator.cpp \
           qoptimalviewpointextension.cpp \
           slicer.cpp \
           qshowhidewidget.cpp \
           vtkVolumeRayCastCompositeFunctionViewpointSaliency.cxx \
           vtkVolumeRayCastCompositeFunctionFx.cxx \
           saliencyvoxelshader.cpp \
           vtkInteractorStyleFasterTrackballActor.cxx 
RESOURCES += optimalviewpoint.qrc
include(../../basicconfextensions.inc)
