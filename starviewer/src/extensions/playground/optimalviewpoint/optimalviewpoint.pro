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
           vtk4DLinearRegressionGradientEstimator.h \
           vtkInteractorStyleJoystickActorGgg.h \
           vtkInteractorStyleSwitchGgg.h \
           optimalviewpointextensionmediator.h \
           qoptimalviewpointextension.h \
           slicer.h \
           qshowhidewidget.h \
           vtkVolumeRayCastCompositeFunctionObscurances.h \
           vtkVolumeRayCastCompositeFunctionViewpointSaliency.h \
           vtkVolumeRayCastCompositeFunctionFx.h \
           obscurancevoxelshader.h \
           saliencyvoxelshader.h \
           colorbleedingvoxelshader.h \
           obscurancethread.h \
           vtkInteractorStyleFasterTrackballActor.h \
           obscurancemainthread.h 
SOURCES += doublehistogram.cpp \
           histogram.cpp \
           optimalviewpoint.cpp \
           optimalviewpointinputparametersform.cpp \
           optimalviewpointparameters.cpp \
           optimalviewpointplane.cpp \
           optimalviewpointplanehelper.cpp \
           optimalviewpointviewer.cpp \
           optimalviewpointvolume.cpp \
           vtk4DLinearRegressionGradientEstimator.cxx \
           vtkInteractorStyleJoystickActorGgg.cxx \
           vtkInteractorStyleSwitchGgg.cxx \
           optimalviewpointextensionmediator.cpp \
           qoptimalviewpointextension.cpp \
           slicer.cpp \
           qshowhidewidget.cpp \
           vtkVolumeRayCastCompositeFunctionObscurances.cxx \
           vtkVolumeRayCastCompositeFunctionViewpointSaliency.cxx \
           vtkVolumeRayCastCompositeFunctionFx.cxx \
           obscurancevoxelshader.cpp \
           saliencyvoxelshader.cpp \
           colorbleedingvoxelshader.cpp \
           obscurancethread.cpp \
           vtkInteractorStyleFasterTrackballActor.cxx \
           obscurancemainthread.cpp 
RESOURCES += optimalviewpoint.qrc
include(../../basicconfextensions.inc)
