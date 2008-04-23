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
           povspherecloud.h \
           vector3.h \
           vtk4DLinearRegressionGradientEstimator.h \
           vtkInteractorStyleJoystickActorGgg.h \
           vtkInteractorStyleSwitchGgg.h \
           vtkInteractorStyleTrackballActorGgg.h \
           vtkVolumeRayCastCompositeFunctionOptimalViewpoint.h \
           optimalviewpointextensionmediator.h \
           qoptimalviewpointextension.h \
           slicer.h \
           qshowhidewidget.h \
           obscurancethread.h \
           vtkVolumeRayCastCompositeFunctionObscurances.h \
           obscurancethread2.h \
           vtkVolumeRayCastCompositeFunctionViewpointSaliency.h 
SOURCES += doublehistogram.cpp \
           histogram.cpp \
           optimalviewpoint.cpp \
           optimalviewpointinputparametersform.cpp \
           optimalviewpointparameters.cpp \
           optimalviewpointplane.cpp \
           optimalviewpointplanehelper.cpp \
           optimalviewpointviewer.cpp \
           optimalviewpointvolume.cpp \
           povspherecloud.cpp \
           vector3.cpp \
           vtk4DLinearRegressionGradientEstimator.cxx \
           vtkInteractorStyleJoystickActorGgg.cxx \
           vtkInteractorStyleSwitchGgg.cxx \
           vtkInteractorStyleTrackballActorGgg.cxx \
           vtkVolumeRayCastCompositeFunctionOptimalViewpoint.cxx \
           optimalviewpointextensionmediator.cpp \
           qoptimalviewpointextension.cpp \
           slicer.cpp \
           qshowhidewidget.cpp \
           obscurancethread.cpp \
           vtkVolumeRayCastCompositeFunctionObscurances.cxx \
           obscurancethread2.cpp \
           vtkVolumeRayCastCompositeFunctionViewpointSaliency.cxx 
RESOURCES += optimalviewpoint.qrc
include(../../basicconfextensions.inc)
