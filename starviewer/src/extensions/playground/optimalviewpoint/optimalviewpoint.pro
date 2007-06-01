# Fitxer generat pel gestor de qmake de kdevelop. 
# ------------------------------------------- 
# Subdirectori relatiu al directori principal del projecte: ./src/extensions/playground/optimalviewpoint
# L'objectiu és una biblioteca:  

FORMS += optimalviewpointinputparametersformbase.ui \
         qtransferfunctioneditorbyvaluesbase.ui \
         qtransferfunctionintervaleditorbase.ui \
         qoptimalviewpointextensionbase.ui 
HEADERS += gradienteditor2.h \
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
           transferfunction.h 
SOURCES += gradienteditor2.cpp \
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
           transferfunction.cpp 
TARGETDEPS += ../../../interface/libinterface.a \
../../../core/libcore.a \
../../../inputoutput/libinputoutput.a
LIBS += ../../../interface/libinterface.a \
../../../core/libcore.a \
../../../inputoutput/libinputoutput.a
INCLUDEPATH += ../../../interface \
../../../core \
../../../inputoutput
MOC_DIR = ../../../../tmp/moc
UI_DIR = ../../../../tmp/ui
OBJECTS_DIR = ../../../../tmp/obj
QMAKE_CXXFLAGS_RELEASE += -Wno-deprecated
QMAKE_CXXFLAGS_DEBUG += -Wno-deprecated
CONFIG += release \
warn_on \
qt \
opengl \
thread \
x11 \
staticlib \
exceptions \
stl
TEMPLATE = lib
include(../../../vtk.inc)
include(../../../itk.inc)
include(../../../dcmtk.inc)
include(../../../compilationtype.inc)
