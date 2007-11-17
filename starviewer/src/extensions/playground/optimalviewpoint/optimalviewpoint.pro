# Fitxer generat pel gestor de qmake de kdevelop. 
# ------------------------------------------- 
# Subdirectori relatiu al directori principal del projecte: ./src/extensions/playground/optimalviewpoint
# L'objectiu � una biblioteca:  

FORMS += optimalviewpointinputparametersformbase.ui \
         qtransferfunctioneditorbyvaluesbase.ui \
         qtransferfunctionintervaleditorbase.ui \
         qoptimalviewpointextensionbase.ui 
HEADERS += doublehistogram.h \
           histogram.h \
           hoverpoints.h \
           optimalviewpoint.h \
           optimalviewpointinputparametersform.h \
           optimalviewpointparameters.h \
           optimalviewpointplane.h \
           optimalviewpointplanehelper.h \
           optimalviewpointviewer.h \
           optimalviewpointvolume.h \
           povspherecloud.h \
           qcolorspinbox.h \
           qtransferfunctioneditorbyvalues.h \
           qtransferfunctionintervaleditor.h \
           shadewidget.h \
           vector3.h \
           vtk4DLinearRegressionGradientEstimator.h \
           vtkInteractorStyleJoystickActorGgg.h \
           vtkInteractorStyleSwitchGgg.h \
           vtkInteractorStyleTrackballActorGgg.h \
           vtkVolumeRayCastCompositeFunctionOptimalViewpoint.h \
           optimalviewpointextensionmediator.h \
           qoptimalviewpointextension.h \
           transferfunction.h \
           transferfunctionio.h \
           slicer.h \
           qtransferfunctioneditor.h \
           qtransferfunctioneditorbygradient.h 
SOURCES += doublehistogram.cpp \
           histogram.cpp \
           hoverpoints.cpp \
           optimalviewpoint.cpp \
           optimalviewpointinputparametersform.cpp \
           optimalviewpointparameters.cpp \
           optimalviewpointplane.cpp \
           optimalviewpointplanehelper.cpp \
           optimalviewpointviewer.cpp \
           optimalviewpointvolume.cpp \
           povspherecloud.cpp \
           qcolorspinbox.cpp \
           qtransferfunctioneditorbyvalues.cpp \
           qtransferfunctionintervaleditor.cpp \
           shadewidget.cpp \
           vector3.cpp \
           vtk4DLinearRegressionGradientEstimator.cxx \
           vtkInteractorStyleJoystickActorGgg.cxx \
           vtkInteractorStyleSwitchGgg.cxx \
           vtkInteractorStyleTrackballActorGgg.cxx \
           vtkVolumeRayCastCompositeFunctionOptimalViewpoint.cxx \
           optimalviewpointextensionmediator.cpp \
           qoptimalviewpointextension.cpp \
           transferfunction.cpp \
           transferfunctionio.cpp \
           slicer.cpp \
           qtransferfunctioneditor.cpp \
           qtransferfunctioneditorbygradient.cpp 

RESOURCES += optimalviewpoint.qrc

include(../../basicconfextensions.inc)
