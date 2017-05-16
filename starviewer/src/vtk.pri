include(defaultdirectories.pri)

INCLUDEPATH += $${VTKINCLUDEDIR}

VTKLIBSUFFIX = -7.0
VTKLIBS = \
    vtkCommonCore \
    vtkCommonDataModel \
    vtkCommonExecutionModel \
    vtkCommonMath \
    vtkCommonMisc \
    vtkCommonTransforms \
    vtkFiltersCore \
    vtkFiltersGeneral \
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
    vtkRenderingOpenGL2 \
    vtkRenderingVolume \
    vtkRenderingVolumeOpenGL2

LIBS += -L$${VTKLIBDIR}

for(lib, VTKLIBS) {
    LIBS += -l$${lib}$${VTKLIBSUFFIX}
}
