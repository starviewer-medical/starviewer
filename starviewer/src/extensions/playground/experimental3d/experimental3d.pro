# Fitxer generat pel gestor de qmake de kdevelop.
# -------------------------------------------
# Subdirectori relatiu al directori principal del projecte: ./src/extensions/playground/experimental3d
# L'objectiu �s una aplicaci??:
FORMS += qexperimental3dextensionbase.ui \
    qviewpointdistributionwidgetbase.ui \
    qgraphictransferfunctioneditorbase.ui
HEADERS += experimental3dextensionmediator.h \
    qexperimental3dextension.h \
    qexperimental3dviewer.h \
    experimental3dvolume.h \
    qviewpointdistributionwidget.h \
    colorbleedingvoxelshader.h \
    volumereslicer.h \
    histogram.h \
    informationtheory.h \
    qbasicgraphictransferfunctioneditor.h \
    qgraphictransferfunctioneditor.h
SOURCES += experimental3dextensionmediator.cpp \
    qexperimental3dextension.cpp \
    qexperimental3dviewer.cpp \
    experimental3dvolume.cpp \
    qviewpointdistributionwidget.cpp \
    colorbleedingvoxelshader.cpp \
    volumereslicer.cpp \
    histogram.cpp \
    informationtheory.cpp \
    qbasicgraphictransferfunctioneditor.cpp \
    qgraphictransferfunctioneditor.cpp
RESOURCES += experimental3d.qrc
include(../../basicconfextensions.inc)
