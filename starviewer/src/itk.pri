include(defaultdirectories.pri)

INCLUDEPATH += $${ITKINCLUDEDIR}

ITKLIBSUFFIX = -4.13
ITKLIBS = \
        ITKCommon \
        ITKMetaIO \
        ITKNrrdIO \
        ITKSpatialObjects \
        ITKStatistics \
        itkjpeg \
        itksys \
        itkvnl_algo \
        itkvnl \
        itkvcl \
        itkv3p_netlib \
        ITKVNLInstantiation \
        ITKniftiio \
        ITKznz \
        ITKOptimizers \
        ITKVTK \
        ITKIOGDCM \
        ITKIOImageBase \
        itksys \
        itkdouble-conversion \
        ITKTransform

macx:official_release:ITKLIBS += itkzlib

LIBS += -L$${ITKLIBDIR}

for(lib, ITKLIBS) {
    LIBS += -l$${lib}$${ITKLIBSUFFIX}
}

win32:LIBS += -lShell32
unix:LIBS += -ldl
