# Fitxer generat pel gestor de qmake de kdevelop.
# -------------------------------------------
# Subdirectori relatiu al directori principal del projecte: ./src/extensions/playground/optimalviewpoint
# L'objectiu s una aplicaci??:
FORMS += optimalviewpointinputparametersformbase.ui \
    qoptimalviewpointextensionbase.ui \
    qshowhidewidgetbase.ui
HEADERS += qinputparameters.h \
    parameters.h \
    doublehistogram.h \
    optimalviewpoint.h \
    optimalviewpointinputparametersform.h \
    optimalviewpointparameters.h \
    optimalviewpointplane.h \
    optimalviewpointplanehelper.h \
    optimalviewpointviewer.h \
    optimalviewpointvolume.h \
    vtkInteractorStyleSwitchGgg.h \
    optimalviewpointextensionmediator.h \
    qoptimalviewpointextension.h \
    slicer.h \
    qshowhidewidget.h \
    vtkVolumeRayCastCompositeFunctionViewpointSaliency.h \
    saliencyvoxelshader.h \
    vtkInteractorStyleFasterTrackballActor.h \
    oldhistogram.h
SOURCES += qinputparameters.cpp \
    parameters.cpp \
    doublehistogram.cpp \
    optimalviewpoint.cpp \
    optimalviewpointinputparametersform.cpp \
    optimalviewpointparameters.cpp \
    optimalviewpointplane.cpp \
    optimalviewpointplanehelper.cpp \
    optimalviewpointviewer.cpp \
    optimalviewpointvolume.cpp \
    vtkInteractorStyleSwitchGgg.cxx \
    optimalviewpointextensionmediator.cpp \
    qoptimalviewpointextension.cpp \
    slicer.cpp \
    qshowhidewidget.cpp \
    vtkVolumeRayCastCompositeFunctionViewpointSaliency.cxx \
    saliencyvoxelshader.cpp \
    vtkInteractorStyleFasterTrackballActor.cxx \
    oldhistogram.cpp
RESOURCES += optimalviewpoint.qrc
include(../../basicconfextensions.inc)
