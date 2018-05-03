include(defaultdirectories.pri)

INCLUDEPATH += $${ITKINCLUDEDIR}

ITKLIBSUFFIX = -4.10
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

win32 {
 ITKLIBS += itktiff \
         itkpng \
         itkzlib

 LIBS += -lShell32
}

LIBS += -L$${ITKLIBDIR}

for(lib, ITKLIBS) {
    LIBS += -l$${lib}$${ITKLIBSUFFIX}
}

unix {
    LIBS += -ldl
}
