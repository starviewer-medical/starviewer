include(defaultdirectories.pri)

INCLUDEPATH += $${ITKINCLUDEDIR}

ITKLIBSUFFIX = -4.6
ITKLIBS = \
        ITKCommon \
        ITKDICOMParser \
        ITKEXPAT \
        ITKFEM \
        ITKMetaIO \
        ITKNrrdIO \
        ITKSpatialObjects \
        ITKStatistics \
        itkjpeg \
        itksys \
        itkvcl \
        itkvnl \
        itkvnl_algo \
        ITKVNLInstantiation \
        ITKniftiio \
        ITKznz \
        ITKOptimizers \
        ITKIOImageBase \
        ITKVTK \
        ITKIOGDCM \
        itksys \
        itkdouble-conversion

win32 {
 ITKLIBS += itktiff \
         itkpng \
         itkv3p_netlib \
         itkzlib

 LIBS += -lShell32
}

LIBS += -L$${ITKLIBDIR}

for(lib, ITKLIBS) {
    LIBS += -l$${lib}$${ITKLIBSUFFIX}
}


