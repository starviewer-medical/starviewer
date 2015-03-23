include(defaultdirectories.pri)

INCLUDEPATH += $${VTKINCLUDEDIR}

VTKLIBSUFFIX = -6.2
VTKLIBS = \
    vtkCommonCore \
    vtkCommonDataModel \
    vtkCommonExecutionModel \
    vtkCommonMath \
    vtkCommonMisc \
    vtkCommonTransforms \
    vtkFiltersCore \
    vtkFiltersSources \
    vtkGUISupportQt \
    vtkImagingCore \
    vtkImagingGeneral \
    vtkImagingHybrid \
    vtkImagingMath \
    vtkInteractionStyle \
    vtkInteractionWidgets \
    vtkIOImage \
    vtkRenderingAnnotation \
    vtkRenderingCore \
    vtkRenderingFreeType \
    vtkRenderingImage \
    vtkRenderingOpenGL \
    vtkRenderingVolume \
    vtkRenderingVolumeOpenGL

LIBS += -L$${VTKLIBDIR}

for(lib, VTKLIBS) {
    LIBS += -l$${lib}$${VTKLIBSUFFIX}
}
