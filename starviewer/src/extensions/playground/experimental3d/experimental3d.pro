# Fitxer generat pel gestor de qmake de kdevelop.
# -------------------------------------------
# Subdirectori relatiu al directori principal del projecte: ./src/extensions/playground/experimental3d
# L'objectiu s una aplicaci??:
FORMS += qexperimental3dextensionbase.ui \
    qviewpointdistributionwidgetbase.ui \
    qgraphictransferfunctioneditorbase.ui
HEADERS += experimental3dextensionmediator.h \
    qexperimental3dextension.h \
    experimental3dsettings.h \
    qexperimental3dviewer.h \
    experimental3dvolume.h \
    qviewpointdistributionwidget.h \
    colorbleedingvoxelshader.h \
    volumereslicer.h \
    histogram.h \
    informationtheory.h \
    qbasicgraphictransferfunctioneditor.h \
    qgraphictransferfunctioneditor.h \
    vmivoxelshader2.h \
    voxelsaliencyvoxelshader.h \
    vomivoxelshader.h \
    colorvomivoxelshader.h \
    opacityvoxelshader.h \
    celshadingvoxelshader.h \
    vomicoolwarmvoxelshader.h \
    coolwarmvoxelshader.h \
    viewpointinformationchannel.h \
    filteringambientocclusionvoxelshader.h
SOURCES += experimental3dextensionmediator.cpp \
    qexperimental3dextension.cpp \
    experimental3dsettings.cpp \
    qexperimental3dviewer.cpp \
    experimental3dvolume.cpp \
    qviewpointdistributionwidget.cpp \
    colorbleedingvoxelshader.cpp \
    volumereslicer.cpp \
    histogram.cpp \
    informationtheory.cpp \
    qbasicgraphictransferfunctioneditor.cpp \
    qgraphictransferfunctioneditor.cpp \
    vmivoxelshader2.cpp \
    voxelsaliencyvoxelshader.cpp \
    vomivoxelshader.cpp \
    colorvomivoxelshader.cpp \
    opacityvoxelshader.cpp \
    celshadingvoxelshader.cpp \
    vomicoolwarmvoxelshader.cpp \
    coolwarmvoxelshader.cpp \
    viewpointinformationchannel.cpp \
    filteringambientocclusionvoxelshader.cpp
RESOURCES += experimental3d.qrc
EXTENSION_DIR = $$PWD
cuda { 
    HEADERS += cudaviewpointinformationchannel.h \
        qcudarenderwindow.h \
        cudafiltering.h
    SOURCES += qcudarenderwindow.cpp
    CUSOURCES += cudaviewpointinformationchannel.cu \
        cudafiltering.cu
    OTHER_FILES += cudaviewpointinformationchannel.cu \
        cudafiltering.cu
}
include(../../basicconfextensions.inc)
include(../../../cuda.inc)
QT += opengl
